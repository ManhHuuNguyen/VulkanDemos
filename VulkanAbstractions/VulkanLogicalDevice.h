#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include "VulkanPhysicalDevice.h"

namespace vk {
	void CreateLogicalDevice(std::vector<QueueCreationRequirement>& reqs, std::vector<uint32_t>& queue_family_indices, std::vector<const char*>& device_extensions,
		bool VALIDATION_LAYER_ENABLED, std::vector<const char*>& validation_layers, VkPhysicalDevice physical_device, VkDevice& logical_device);
}