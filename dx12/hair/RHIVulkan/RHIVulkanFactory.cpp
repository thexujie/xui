#include "stdafx.h"
#include "RHIVulkanFactory.h"
#include "RHIVulkanDevice.h"

#pragma comment(lib, "C:\\Program Files\\Vulkan 1.1.114.0\\Lib\\vulkan-1.lib")

namespace RHI::RHIVulkan
{
	//bool checkValidationLayerSupport(std::vector<const char *> validationLayers) {
	//	uint32_t layerCount;
	//	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
	//	std::vector<VkLayerProperties> availableLayers(layerCount);
	//	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	//	for (const char * layerName : validationLayers) {
	//		bool layerFound = false;
	//		for (const auto & layerProperties : availableLayers) {
	//			if (strcmp(layerName, layerProperties.layerName) == 0) {
	//				layerFound = true;
	//				break;
	//			}
	//		}
	//		if (!layerFound) {
	//			return false;
	//		}
	//	}
	//	return true;
	//}
	
#define APP_SHORT_NAME "RHIVulkanFactory"
	
	VKAPI_ATTR VkBool32 VKAPI_CALL VulkanDebugReportCallback(
		VkDebugReportFlagsEXT       flags,
		VkDebugReportObjectTypeEXT  objectType,
		uint64_t                    object,
		size_t                      location,
		int32_t                     messageCode,
		const char * pLayerPrefix,
		const char * pMessage,
		void * pUserData)
	{
		core::err() << __FUNCTION__ " >" << pMessage;
		return VK_FALSE;
	}

	RHIVulkanFactory::~RHIVulkanFactory()
	{
#if _DEBUG
		if (_instance && _callback)
		{
			PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(_instance, "vkDestroyDebugReportCallbackEXT"));
			vkDestroyDebugReportCallbackEXT(_instance, _callback, nullptr);
			_callback = nullptr;
		}
#endif
	}
	
	core::error RHIVulkanFactory::Load()
	{
		uint32_t nlayers = 0;
		vkEnumerateInstanceLayerProperties(&nlayers, nullptr);
		std::vector<VkLayerProperties> availableLayers(nlayers);
		vkEnumerateInstanceLayerProperties(&nlayers, availableLayers.data());

		std::vector<std::vector<VkExtensionProperties>> availableExtensions(nlayers);
		for (uint32_t ilayer = 0; ilayer < nlayers; ++ilayer)
		{
			uint32_t nextensions = 0;
			vkEnumerateInstanceExtensionProperties(availableLayers[ilayer].layerName, &nextensions, nullptr);

			availableExtensions[ilayer].resize(nextensions);
			vkEnumerateInstanceExtensionProperties(availableLayers[ilayer].layerName, &nextensions, availableExtensions[ilayer].data());
		}

		_availableLayers = availableLayers;
		_availableExtensions = availableExtensions;
		
		VkApplicationInfo app_info = {};
		app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		app_info.pNext = NULL;
		app_info.pApplicationName = APP_SHORT_NAME;
		app_info.applicationVersion = 1;
		app_info.pEngineName = APP_SHORT_NAME;
		app_info.engineVersion = 1;
		app_info.apiVersion = VK_API_VERSION_1_0;

		std::vector<const char *> extensions =
		{
#ifdef _DEBUG
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
#endif
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
		};
		
		std::vector<const char *> layers = 
		{
#ifdef _DEBUG
			"VK_LAYER_LUNARG_standard_validation"
#endif
		};

		VkInstanceCreateInfo inst_info = {};
		inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		inst_info.pNext = NULL;
		inst_info.flags = 0;
		inst_info.pApplicationInfo = &app_info;
		inst_info.enabledExtensionCount = extensions.size();
		inst_info.ppEnabledExtensionNames = extensions.data();
		inst_info.enabledLayerCount = layers.size();
		inst_info.ppEnabledLayerNames = layers.data();

		VkResult result = VK_SUCCESS;
		VkInstance instance;
		result = vkCreateInstance(&inst_info, NULL, &instance);
		if (result == VK_ERROR_INCOMPATIBLE_DRIVER) 
		{
			core::err() << __FUNCTION__" vkCreateInstance failed <VK_ERROR_INCOMPATIBLE_DRIVER>, " << result;
			return core::e_inner;
		}
		else if (result) 
		{
			core::err() << __FUNCTION__" vkCreateInstance failed <VK_ERROR_INCOMPATIBLE_DRIVER>, " << result;
			return core::e_inner;
		}


#if _DEBUG
		PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugReportCallbackEXT"));
		PFN_vkDebugReportMessageEXT vkDebugReportMessageEXT = reinterpret_cast<PFN_vkDebugReportMessageEXT>(vkGetInstanceProcAddr(instance, "vkDebugReportMessageEXT"));
		PFN_vkDestroyDebugReportCallbackEXT vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugReportCallbackEXT"));
		VkDebugReportCallbackCreateInfoEXT callbackCreateInfo = {};
		callbackCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
		callbackCreateInfo.pNext = nullptr;
		callbackCreateInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
		callbackCreateInfo.pfnCallback = VulkanDebugReportCallback;
		callbackCreateInfo.pUserData = nullptr;
		result = vkCreateDebugReportCallbackEXT(instance, &callbackCreateInfo, nullptr, &_callback);
		if (result)
		{
			core::war() << __FUNCTION__" vkCreateDebugReportCallbackEXT failed, " << result;
		}
#endif

		_instance = instance;
		return core::ok;
	}

	std::vector<RHIAdapterDesc> RHIVulkanFactory::AdapterDescs() const
	{
		VkResult res = VK_SUCCESS;
		std::vector<RHIAdapterDesc> adapterDescs;

		uint32_t ndevcies = 1;
		res = vkEnumeratePhysicalDevices(_instance, &ndevcies, NULL);
		std::vector<VkPhysicalDevice> adapters(ndevcies);
		res = vkEnumeratePhysicalDevices(_instance, &ndevcies, adapters.data());
		assert(!res && ndevcies >= 1);

		for (uint32_t iadapter = 0; iadapter< ndevcies; ++iadapter)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(adapters[iadapter], &properties);

			RHIAdapterDesc desc;
			desc.id = properties.deviceID;
			desc.name = core::astr_u8str(properties.deviceName);
			desc.flags.set(RHIAdapterFlag::CacheCoherentUMA, properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU);
			desc.flags.set(RHIAdapterFlag::Software, properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU);
			adapterDescs.emplace_back(desc);
		}
		return adapterDescs;
	}

	std::shared_ptr<RHIDevice> RHIVulkanFactory::CreateDevice(uint64_t id) const
	{
		VkResult res = VK_SUCCESS;
		std::vector<RHIAdapterDesc> adapterDescs;
		uint32_t ndevcies = 1;
		res = vkEnumeratePhysicalDevices(_instance, &ndevcies, NULL);
		std::vector<VkPhysicalDevice> adapters(ndevcies);
		res = vkEnumeratePhysicalDevices(_instance, &ndevcies, adapters.data());
		assert(!res && ndevcies >= 1);
		VkPhysicalDevice adapter = nullptr;
		for (uint32_t iadapter = 0; iadapter < ndevcies; ++iadapter)
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(adapters[iadapter], &properties);
			if(properties.deviceID == id)
			{
				adapter = adapters[iadapter];
				break;
			}
		}

		if (!adapter)
			return nullptr;

		auto device = std::make_shared<RHIVulkanDevice>(const_cast<RHIVulkanFactory *>(this));
		auto err = device->Create(adapter);
		if (err)
			return nullptr;
		return device;
	}
}