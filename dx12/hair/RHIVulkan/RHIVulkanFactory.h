#pragma once

#include "RHIVulkanCore.h"

namespace RHI::RHIVulkan
{
	class RHIVulkanFactory : public RHIFactory
	{
	public:
		RHIVulkanFactory() = default;
		~RHIVulkanFactory();

		core::error Load() override;
		std::vector<RHIAdapterDesc> AdapterDescs() const override;
		std::shared_ptr<RHIDevice> CreateDevice(uint64_t id) const override;

	public:
		VkInstance Instance() const { return _instance; }
		
	private:
		VkInstance _instance = nullptr;
		std::vector<VkLayerProperties> _availableLayers;
		std::vector<std::vector<VkExtensionProperties>> _availableExtensions;
#ifdef _DEBUG
		VkDebugReportCallbackEXT _callback = nullptr;
#endif
	};
}
