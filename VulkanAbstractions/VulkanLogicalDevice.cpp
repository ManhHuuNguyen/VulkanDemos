#include "VulkanLogicalDevice.h"
#include <stdexcept>

namespace vk {
	void CreateLogicalDevice(std::vector<QueueCreationRequirement>& reqs, std::vector<uint32_t> & queue_family_indices, std::vector<const char*>& device_extensions, 
		bool VALIDATION_LAYER_ENABLED, std::vector<const char*> & validation_layers, VkPhysicalDevice physical_device, VkDevice & logical_device) {
		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		for (uint32_t i = 0; i < reqs.size(); i++) {
			VkDeviceQueueCreateInfo queue_create_info = {};
			queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queue_create_info.queueFamilyIndex = queue_family_indices[i];
			queue_create_info.queueCount = reqs[i].num_queue;
			queue_create_info.pQueuePriorities = reqs[i].priorities.data();
			queue_create_infos.push_back(queue_create_info);
		}

		VkPhysicalDeviceFeatures device_features = {};
		VkDeviceCreateInfo device_create_info = {};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		device_create_info.pEnabledFeatures = &device_features;
		device_create_info.ppEnabledExtensionNames = device_extensions.data();
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
		if (VALIDATION_LAYER_ENABLED) {
			device_create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
			device_create_info.ppEnabledLayerNames = validation_layers.data();
		}
		else {
			device_create_info.enabledLayerCount = 0;
		}
		if (vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device) != VK_SUCCESS) {
			throw std::runtime_error("fail to create logical device");
		}
	}
}
