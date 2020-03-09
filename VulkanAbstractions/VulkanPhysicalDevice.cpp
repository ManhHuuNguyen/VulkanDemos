#include "VulkanPhysicalDevice.h"
#include <vector>
#include <stdexcept>
#include <iostream>
#include "VulkanSwapChain.h"

namespace vk {

	VkPhysicalDevice PickPhysicalDevice(VkInstance vk_instance, VkSurfaceKHR surface,
		std::vector<QueueCreationRequirement>& queue_family_requirements, std::vector<const char*>& device_extensions, std::vector<uint32_t>& queue_family_indices) {

		uint32_t device_count = 0;
		vkEnumeratePhysicalDevices(vk_instance, &device_count, nullptr);
		if (device_count == 0) {
			throw std::runtime_error("fail to find gpu with vulkan support");
		}
		std::vector<VkPhysicalDevice> physical_devices(device_count);
		vkEnumeratePhysicalDevices(vk_instance, &device_count, physical_devices.data());

		for (VkPhysicalDevice& device : physical_devices) {
			if (FindQueueFamilies(device, surface, queue_family_requirements, queue_family_indices) 
				&& AreDeviceExtensionsSupported(device, device_extensions) && 
				IsSwapchainAdequate(device, surface)) {
				// if a physical device satisfy all the queue property requirement
				// supports all the device extensions
				// and capable of creating swapchain for the surface
				return device;
			}
		}
		return VK_NULL_HANDLE;
	}

	// check if a physical device has all the queue family requirements fulfilled. queue_family_indices vector is filled if device actually satisfies
	bool FindQueueFamilies(VkPhysicalDevice physical_device, VkSurfaceKHR surface,
		std::vector<QueueCreationRequirement>& queue_family_requirements, std::vector<uint32_t>& queue_family_indices) {

		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

		for (QueueCreationRequirement& requirement : queue_family_requirements) {
			bool queue_found = false;
			for (uint32_t idx = 0; idx < queue_families.size(); idx++) {
				if (IsQueueFamilySuitable(queue_families[idx], requirement, surface, physical_device, idx)) {
					queue_family_indices.push_back(idx);
					queue_found = true;
					break;
				}
			}
			if (!queue_found) {
				queue_family_indices.clear(); // clear the queue_family_indices vector if this physical device does not satisfy
				return false;
			}
		}
		return true;
	}

	VkFormat GetSupportedDepthFormat(VkPhysicalDevice physical_device) {
		// Since all depth formats may be optional, we need to find a suitable depth format to use
		// Start with the highest precision packed format
		std::vector<VkFormat> depth_formats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};
		for (VkFormat& format : depth_formats) {
			VkFormatProperties format_properties;
			vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
			// Format must support depth stencil attachment for optimal tiling
			if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
				return format;
			}
		}
		throw std::runtime_error("Fail to find appropriate depth format");
	}

	uint32_t FindMemoryType(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags properties) {
		//typeFilter is a bit field of memory types that are suitable
		VkPhysicalDeviceMemoryProperties mem_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_properties);
		for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++) {
			if (type_filter & (1 << i) && (mem_properties.memoryTypes[i].propertyFlags & properties) == properties) {
				// if the memory type the buffer or image require is supported by physical device and cover all the properties
				return i;
			}
		}
		throw std::runtime_error("failed to find suitable memory type");
	}

	uint32_t GetMinUniformBufferAlignment(VkPhysicalDevice physical_device) {
		VkPhysicalDeviceProperties device_property;
		vkGetPhysicalDeviceProperties(physical_device, &device_property);
		return device_property.limits.minUniformBufferOffsetAlignment;
	}

	namespace {
		// check if a queue family satisfies all its requirements
		bool IsQueueFamilySuitable(VkQueueFamilyProperties queue_family, QueueCreationRequirement& queue_family_requirement, VkSurfaceKHR surface, VkPhysicalDevice physical_device, uint32_t queue_idx) {
			if (queue_family_requirement.types.is_graphic && !(queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) {
				return false;
			}

			if (queue_family_requirement.types.is_compute && !(queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)) {
				return false;
			}

			if (queue_family_requirement.types.is_transfer && !(queue_family.queueFlags & VK_QUEUE_TRANSFER_BIT)) {
				return false;
			}

			if (queue_family_requirement.types.is_sparse_binding && !(queue_family.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)) {
				return false;
			}

			if (queue_family_requirement.types.is_protected && !(queue_family.queueFlags & VK_QUEUE_PROTECTED_BIT)) {
				return false;
			}
			if (queue_family_requirement.types.is_present) {
				VkBool32 is_supported = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, queue_idx, surface, &is_supported);
				if (!is_supported) {
					return false;
				}
			}
			return true;
		}

		bool AreDeviceExtensionsSupported(VkPhysicalDevice physical_device, std::vector<const char*>& required_extensions) {
			uint32_t extension_count = 0;
			vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
			std::vector<VkExtensionProperties> available_extensions(extension_count);
			vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

			std::set<std::string> required_extensions_set(required_extensions.begin(), required_extensions.end());
			for (VkExtensionProperties& extension : available_extensions) {
				required_extensions_set.erase(extension.extensionName);
			}
			return required_extensions_set.empty();
		}

		bool IsSwapchainAdequate(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
			SwapChainSupportDetails details = QuerySwapChainSupport(physical_device, surface);
			return !details.formats.empty() && !details.present_modes.empty();
		}

	}
	

}
