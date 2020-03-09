#include "VulkanCompositeImage.h"
#include <stdexcept>
#include "VulkanPhysicalDevice.h"
#include "VulkanHelper.h"

namespace vk {

	std::unordered_map<VkImageType, VkImageViewType> VulkanCompositeImage::image_to_view_map {
		{VK_IMAGE_TYPE_1D, VK_IMAGE_VIEW_TYPE_1D},
		{VK_IMAGE_TYPE_2D, VK_IMAGE_VIEW_TYPE_2D},
		{VK_IMAGE_TYPE_3D, VK_IMAGE_VIEW_TYPE_3D},
	};

	// properties: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkMemoryPropertyFlagBits.html
	void VulkanCompositeImage::CreateImage(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageCreateInfo create_info, VkMemoryPropertyFlags mem_properties) {
		if (this->image != VK_NULL_HANDLE) {
			throw std::runtime_error("VkImage is already created\n");
		}
		this->image_type = create_info.imageType;
		this->format = create_info.format;
		this->image_extent = create_info.extent;
		this->usage_flag = create_info.usage;
		this->logical_device = logical_device;
		//create
		if (vkCreateImage(this->logical_device, &create_info, nullptr, &this->image) != VK_SUCCESS) {
			throw std::runtime_error("fail to create image");
		}
		VkMemoryRequirements mem_req = {};
		vkGetImageMemoryRequirements(this->logical_device, this->image, &mem_req);
		vk::init::AllocateMemory(logical_device, physical_device, mem_req, mem_properties, &this->device_memory);
		vkBindImageMemory(logical_device, this->image, this->device_memory, 0);
	}

	// aspect flags: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkImageAspectFlagBits.html
	void VulkanCompositeImage::CreateImageView(VkImageAspectFlags aspect_flag) {
		if (this->image == VK_NULL_HANDLE) {
			throw std::runtime_error("Needs to create VkImage before creating image view");
		}
		if (this->image_view != VK_NULL_HANDLE) {
			throw std::runtime_error("Image view is already created");
		}
		if (aspect_flag & VK_IMAGE_ASPECT_STENCIL_BIT && format < VK_FORMAT_D16_UNORM_S8_UINT) {
			// Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT
			throw std::runtime_error("Stencil aspect should only be set on depth + stencil formats (VK_FORMAT_D16_UNORM_S8_UINT..VK_FORMAT_D32_SFLOAT_S8_UINT");
		}
		VkImageViewCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		create_info.viewType = image_to_view_map[this->image_type];
		create_info.image = this->image;
		create_info.format = this->format;
		create_info.subresourceRange.baseMipLevel = 0;
		create_info.subresourceRange.levelCount = 1;
		create_info.subresourceRange.baseArrayLayer = 0;
		create_info.subresourceRange.layerCount = 1;
		create_info.subresourceRange.aspectMask = aspect_flag;
		if (vkCreateImageView(logical_device, &create_info, nullptr, &this->image_view) != VK_SUCCESS) {
			throw std::runtime_error("fail to create image view");
		}
	}

	void VulkanCompositeImage::DestroyImage() {
		if (this->image == VK_NULL_HANDLE) {
			throw std::runtime_error("Image is not yet created");
		}
		if (this->image_view != VK_NULL_HANDLE) {
			throw std::runtime_error("Needs to destroy image view first before destroying image");
		}
		vkFreeMemory(this->logical_device, this->device_memory, nullptr);
		vkDestroyImage(this->logical_device, this->image, nullptr);
		this->image = VK_NULL_HANDLE;
		this->device_memory = VK_NULL_HANDLE;
	}

	void VulkanCompositeImage::DestroyImageView() {
		if (this->image_view == VK_NULL_HANDLE) {
			throw std::runtime_error("Image view is not yet created");
		}
		vkDestroyImageView(this->logical_device, this->image_view, nullptr);
		this->image_view = VK_NULL_HANDLE;
	}

	void VulkanCompositeImage::Create(VkPhysicalDevice physical_device, VkDevice logical_device, VkImageCreateInfo create_info, VkMemoryPropertyFlags properties, VkImageAspectFlags aspect_flag) {
		CreateImage(physical_device, logical_device, create_info, properties);
		CreateImageView(aspect_flag);
	}

	void VulkanCompositeImage::Destroy() {
		DestroyImageView();
		DestroyImage();
	}
}