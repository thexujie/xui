#pragma once

#include "RHI/RHI.h"
#include "RHIVulkanDevice.h"

namespace RHI::RHIVulkan
{
	class RHIVulkanResource : public RHIResource
	{
	public:
		RHIVulkanResource(RHIVulkanDevice * device) : _device(device) {}
		virtual ~RHIVulkanResource() = default;

		core::error Create(const ResourceArgs & args);
		void SetName(const std::u8string & name) override;

	public:
		void * Data() override { return _pointer; }
		const void * Data() const override { return _pointer; }
		core::sizeu Size() const override { return _args.size; }

public:
		const ResourceArgs & Args() const { return _args; }
		VkBuffer BufferResource() const { return _buffer; }
		
	private:
		RHIVulkanDevice * _device = nullptr;
		ResourceArgs _args;
		VkBuffer _buffer = nullptr;
		void * _pointer = nullptr;
	};
}
