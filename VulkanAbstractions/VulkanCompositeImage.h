#pragma once
#include "vulkan/vulkan.h"
#include <unordered_map>

namespace vk {

	class VulkanCompositeImage {
	public:
		void CreateImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageCreateInfo create_info, VkMemoryPropertyFlags mem_properties);
		void CreateImageView(VkImageAspectFlags aspect_flag);
		void DestroyImage();
		void DestroyImageView();
		void Create(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageCreateInfo create_info, VkMemoryPropertyFlags mem_properties, VkImageAspectFlags aspect_flag); // will create both image and image view
		void Destroy(); // will destroy both image and image view
	public:
		VkFormat format;
		VkImageView image_view = VK_NULL_HANDLE;
	private:
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory device_memory = VK_NULL_HANDLE;
		
		VkImageType image_type;
		
		VkExtent3D image_extent;
		VkImageUsageFlags usage_flag;
		VkDevice logical_device;
		static std::unordered_map<VkImageType, VkImageViewType> image_to_view_map;
	};

}