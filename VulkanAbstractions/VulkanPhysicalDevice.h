#pragma once
#include "vulkan/vulkan.h"
#include <optional>
#include <vector>
#include <set>
#include <string>
#include "VulkanQueue.h"

namespace vk {

	struct QueueCreationRequirement {
		VulkanQueueCharacteristic types;
		uint32_t num_queue;
		std::vector<float> priorities; // size of vector must match num_queue
	};

	VkPhysicalDevice PickPhysicalDevice(VkInstance vk_instance, VkSurfaceKHR surface, 
		std::vector<QueueCreationRequirement>& queue_family_requirements, std::vector<const char*> & device_extensions, std::vector<uint32_t>& queue_family_indices);

	bool FindQueueFamilies(VkPhysicalDevice physical_device, VkSurfaceKHR surface, 
		std::vector<QueueCreationRequirement> & queue_family_requirements, std::vector<uint32_t> & queue_family_indices);

	VkFormat GetSupportedDepthFormat(VkPhysicalDevice physical_device);

	uint32_t FindMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties);

	uint32_t GetMinUniformBufferAlignment(VkPhysicalDevice physical_device);
	
	namespace {

		bool IsQueueFamilySuitable(VkQueueFamilyProperties queue_family, QueueCreationRequirement& queue_family_requirement,
			VkSurfaceKHR surface, VkPhysicalDevice physical_device, uint32_t queue_idx);

		bool AreDeviceExtensionsSupported(VkPhysicalDevice physical_device, std::vector<const char*>& required_extensions);

		// check if the physical device can make a swapchain supporting this surface
		bool IsSwapchainAdequate(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

	}

};