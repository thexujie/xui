/*
 * Vulkan Samples
 *
 * Copyright (C) 2015-2016 Valve Corporation
 * Copyright (C) 2015-2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

 /*
 VULKAN_SAMPLE_SHORT_DESCRIPTION
 create and destroy Vulkan instance
 */

 /* This is part of the draw cube progression */

#include "stdafx.h"
#include <iostream>
#include <vulkan.h>

#define APP_SHORT_NAME "vulkansamples_instance"
#pragma comment(lib, "C:\\Program Files\\Vulkan 1.1.114.0\\Lib\\vulkan-1.lib")

int main(int argc, char * argv[]) {
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pNext = NULL;
	app_info.pApplicationName = APP_SHORT_NAME;
	app_info.applicationVersion = 1;
	app_info.pEngineName = APP_SHORT_NAME;
	app_info.engineVersion = 1;
	app_info.apiVersion = VK_API_VERSION_1_0;

	// initialize the VkInstanceCreateInfo structure
	VkInstanceCreateInfo inst_info = {};
	inst_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_info.pNext = NULL;
	inst_info.flags = 0;
	inst_info.pApplicationInfo = &app_info;
	inst_info.enabledExtensionCount = 0;
	inst_info.ppEnabledExtensionNames = NULL;
	inst_info.enabledLayerCount = 0;
	inst_info.ppEnabledLayerNames = NULL;

	VkInstance inst;
	VkResult res;

	res = vkCreateInstance(&inst_info, NULL, &inst);
	if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
		std::cout << "cannot find a compatible Vulkan ICD\n";
		exit(-1);
	}
	else if (res) {
		std::cout << "unknown error\n";
		exit(-1);
	}


	uint32_t gpu_count = 1;
	res = vkEnumeratePhysicalDevices(inst, &gpu_count, NULL);
	assert(gpu_count);
	std::vector<VkPhysicalDevice> adapters(gpu_count);
	res = vkEnumeratePhysicalDevices(inst, &gpu_count, adapters.data());
	assert(!res && gpu_count >= 1);


	VkDeviceQueueCreateInfo queue_info = {};
	uint32_t queue_family_count = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(adapters[0], &queue_family_count, NULL);

	std::vector<VkQueueFamilyProperties> queue_props(queue_family_count);
	vkGetPhysicalDeviceQueueFamilyProperties(adapters[0], &queue_family_count, queue_props.data());

	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(adapters[0], &properties);
	
	vkDestroyInstance(inst, NULL);

	/* VULKAN_KEY_END */

	return 0;
}
