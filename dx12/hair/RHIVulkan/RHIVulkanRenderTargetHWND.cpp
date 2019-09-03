#include "stdafx.h"
#include "RHIVulkanRenderTargetHWND.h"
#include "RHIVulkanCommandList.h"
#include "RHIVulkanCommandQueue.h"
#include "RHIVulkanFactory.h"

namespace RHI::RHIVulkan
{
	core::error RHIVulkanRenderTargetHWND::Create(RHICommandQueue * cmdqueue, const RenderTargetArgs & args)
	{
		if (!_device)
			return core::e_state;

		HRESULT hr = S_OK;

		auto device = _device->Device();
		auto adapter = _device->Adapter();
		assert(device);
		assert(adapter);

		VkWin32SurfaceCreateInfoKHR createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.pNext = NULL;
		createInfo.hinstance = (HINSTANCE)win32::instance();
		createInfo.hwnd = (HWND)args.hwnd;

		VkSurfaceKHR surface = nullptr;
		auto result = vkCreateWin32SurfaceKHR(_device->Factory()->Instance(), &createInfo, NULL, &surface);
		assert(result == VK_SUCCESS);
		if (result)
		{
			core::err() << __FUNCTION__" vkCreateWin32SurfaceKHR failed, " << result;
			return core::e_inner;
		}

		uint32_t nqueuefamilies = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &nqueuefamilies, NULL);

		std::vector<VkQueueFamilyProperties> queueFamilyProperties(nqueuefamilies);
		vkGetPhysicalDeviceQueueFamilyProperties(adapter, &nqueuefamilies, queueFamilyProperties.data());

		// Iterate over each queue to learn whether it supports presenting:
		std::vector<VkBool32> supportsPresent(nqueuefamilies);
		for (uint32_t index = 0; index < nqueuefamilies; index++)
			vkGetPhysicalDeviceSurfaceSupportKHR(adapter, index, surface, &supportsPresent[index]);

		uint32_t graphics_queue_family_index = core::nposu32;
		uint32_t present_queue_family_index = core::nposu32;
		for (uint32_t iprop = 0; iprop < nqueuefamilies; ++iprop)
		{
			if ((queueFamilyProperties[iprop].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphics_queue_family_index == core::nposu32)
					graphics_queue_family_index = iprop;

				if (supportsPresent[iprop] == VK_TRUE)
				{
					graphics_queue_family_index = iprop;
					present_queue_family_index = iprop;
					break;
				}
			}
		}

		if (present_queue_family_index == core::nposu32)
		{
			// If didn't find a queue that supports both graphics and present, then
			// find a separate present queue.
			for (size_t iprop = 0; iprop < nqueuefamilies; ++iprop)
				if (supportsPresent[iprop] == VK_TRUE)
				{
					present_queue_family_index = iprop;
					break;
				}
		}

		if (graphics_queue_family_index == core::nposu32 || present_queue_family_index == core::nposu32)
		{
			core::err() << __FUNCTION__ << " Could not find a queues for graphics and present\n";
			exit(-1);
		}

		VkSurfaceCapabilitiesKHR surfCapabilities;
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(adapter, surface, &surfCapabilities);
		assert(result == VK_SUCCESS);
		if (args.nbuffers < surfCapabilities.minImageCount)
		{
			core::err() << __FUNCTION__ " args.nbuffer < " << surfCapabilities.minImageCount;
			return core::e_args;
		}

		VkSwapchainCreateInfoKHR swapchainDesc = {};
		swapchainDesc.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainDesc.pNext = NULL;
		swapchainDesc.surface = surface;
		swapchainDesc.minImageCount = args.nbuffers;
		swapchainDesc.imageFormat = FromFormat(args.format);
		swapchainDesc.imageExtent.width = args.size.cx;
		swapchainDesc.imageExtent.height = args.size.cy;
		swapchainDesc.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		swapchainDesc.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainDesc.imageArrayLayers = 1;
		swapchainDesc.presentMode = VK_PRESENT_MODE_FIFO_KHR;
		swapchainDesc.oldSwapchain = VK_NULL_HANDLE;
		swapchainDesc.clipped = true;
		swapchainDesc.imageColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
		swapchainDesc.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainDesc.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapchainDesc.queueFamilyIndexCount = 0;
		swapchainDesc.pQueueFamilyIndices = NULL;
		uint32_t queueFamilyIndices[2] = { (uint32_t)graphics_queue_family_index, (uint32_t)present_queue_family_index };
		if (graphics_queue_family_index != present_queue_family_index)
		{
			// If the graphics and present queues are from different queue families,
			// we either have to explicitly transfer ownership of images between
			// the queues, or we have to create the swapchain with imageSharingMode
			// as VK_SHARING_MODE_CONCURRENT
			swapchainDesc.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainDesc.queueFamilyIndexCount = 2;
			swapchainDesc.pQueueFamilyIndices = queueFamilyIndices;
		}

		VkSwapchainKHR swapchain = nullptr;
		result = vkCreateSwapchainKHR(device, &swapchainDesc, NULL, &swapchain);
		assert(result == VK_SUCCESS);
		if (result)
		{
			core::err() << __FUNCTION__ " vkCreateSwapchainKHR failed";
			return core::e_args;
		}

		uint32_t nbuffers = 0;
		result = vkGetSwapchainImagesKHR(device, swapchain, &nbuffers, NULL);
		assert(result == VK_SUCCESS);
		assert(nbuffers == args.nbuffers);
		if (result)
		{
			core::err() << __FUNCTION__ " vkGetSwapchainImagesKHR failed";
			return core::e_args;
		}

		std::vector<VkImage> buffers(nbuffers);
		result = vkGetSwapchainImagesKHR(device, swapchain, &nbuffers, buffers.data());
		assert(result == VK_SUCCESS);
		if (result)
		{
			core::err() << __FUNCTION__ " vkGetSwapchainImagesKHR failed";
			return core::e_args;
		}

		std::vector<VkImageView> views(nbuffers);
		for (uint32_t ibuffer = 0; ibuffer < nbuffers; ibuffer++) 
		{
			VkImageViewCreateInfo color_image_view = {};
			color_image_view.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			color_image_view.pNext = NULL;
			color_image_view.flags = 0;
			color_image_view.image = buffers[ibuffer];
			color_image_view.viewType = VK_IMAGE_VIEW_TYPE_2D;
			color_image_view.format = swapchainDesc.imageFormat;
			color_image_view.components.r = VK_COMPONENT_SWIZZLE_R;
			color_image_view.components.g = VK_COMPONENT_SWIZZLE_G;
			color_image_view.components.b = VK_COMPONENT_SWIZZLE_B;
			color_image_view.components.a = VK_COMPONENT_SWIZZLE_A;
			color_image_view.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			color_image_view.subresourceRange.baseMipLevel = 0;
			color_image_view.subresourceRange.levelCount = 1;
			color_image_view.subresourceRange.baseArrayLayer = 0;
			color_image_view.subresourceRange.layerCount = 1;

			result = vkCreateImageView(device, &color_image_view, NULL, &views[ibuffer]);
			assert(result == VK_SUCCESS);
			if (result)
			{
				core::err() << __FUNCTION__ " vkGetSwapchainImagesKHR failed";
				return core::e_args;
			}
		}
		
		//---------------------------------------------------------------
		_args = args;
		_surface = surface;
		_buffers = buffers;
		_views = views;
		_swapchain = swapchain;
		return core::ok;
	}

	void RHIVulkanRenderTargetHWND::SetName(const std::u8string & name) { }

	void RHIVulkanRenderTargetHWND::Begin()
	{
		//_frameIndex = _swapchain->GetCurrentBackBufferIndex();
	}

	void RHIVulkanRenderTargetHWND::Present(uint32_t sync)
	{
		//if (!_swapchain)
		//	return ;

		//_swapchain->Present(sync, 0);
	}

	uint32_t RHIVulkanRenderTargetHWND::FrameIndex() const
	{
		return 0;
		//return _swapchain->GetCurrentBackBufferIndex();
	}
}
