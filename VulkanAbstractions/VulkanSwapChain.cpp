#include "VulkanSwapChain.h"
#include <stdexcept>

namespace vk {

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface) {
		SwapChainSupportDetails details = {};
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, surface, &details.capabilities);

		uint32_t format_count = 0;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, nullptr);
		if (format_count > 0) {
			details.formats.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, surface, &format_count, details.formats.data());
		}

		uint32_t present_mode_count = 0;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, nullptr);
		if (present_mode_count > 0) {
			details.present_modes.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, surface, &present_mode_count, details.present_modes.data());
		}
		return details;
	}

	
	VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(std::vector<VkPresentModeKHR>& present_modes) {
		for (VkPresentModeKHR & mode : present_modes) {
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR) { // best mode possible, return immediately
				return VK_PRESENT_MODE_MAILBOX_KHR;
			}
		}
		return VK_PRESENT_MODE_FIFO_KHR; // this mode is guaranteed to be available
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height) {
		if (capabilities.currentExtent.width == UINT32_MAX) {// surface size is undefined, we can use our desired width and height
			return VkExtent2D{ width, height };
		}
		return capabilities.currentExtent; // else we have to use the size the swapchain wants
	}

	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR>& formats) {
		for (VkSurfaceFormatKHR& format : formats) {
			if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return format;
			}
		}
		return formats[0];
	}

	uint32_t VulkanSwapChain::ChooseImageCount(VkSurfaceCapabilitiesKHR capabilities) {
		uint32_t desired_count = capabilities.minImageCount + 1;
		if (capabilities.maxImageCount > 0 && capabilities.maxImageCount < desired_count) {
			desired_count = capabilities.maxImageCount;
		}
		return desired_count;
	}
	
	void VulkanSwapChain::Create(VkDevice logical_device, VkPhysicalDevice physical_device, VkSurfaceKHR surface, uint32_t width, uint32_t height) {
	
		VkSwapchainKHR old_swap_chain = this->swap_chain;

		this->logical_device = logical_device;
		SwapChainSupportDetails details = QuerySwapChainSupport(physical_device, surface);
		this->swap_surface_format = ChooseSwapSurfaceFormat(details.formats);
		this->present_mode = ChooseSwapPresentMode(details.present_modes);
		this->swap_extent = ChooseSwapExtent(details.capabilities, width, height);
		this->image_count = ChooseImageCount(details.capabilities);
		
		VkSwapchainCreateInfoKHR create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = surface;
		create_info.minImageCount = this->image_count;
		create_info.imageFormat = this->swap_surface_format.format;
		create_info.imageColorSpace = this->swap_surface_format.colorSpace;
		create_info.imageExtent = this->swap_extent;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		create_info.preTransform = details.capabilities.currentTransform;
		create_info.imageArrayLayers = 1;
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.queueFamilyIndexCount = 0;
		create_info.pQueueFamilyIndices = nullptr;
		create_info.presentMode = this->present_mode;
		create_info.oldSwapchain = old_swap_chain;
		create_info.clipped = VK_TRUE;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		// allow transfer source and destination on swapchain image if such feature is supported
		if (details.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
			create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		if (details.capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			create_info.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}
		if (vkCreateSwapchainKHR(logical_device, &create_info, nullptr, &this->swap_chain) != VK_SUCCESS) {
			throw std::runtime_error("fail to create swapchain");
		}
		// delete old swap chain and its image views
		if (old_swap_chain != VK_NULL_HANDLE) {
			Cleanup(old_swap_chain);
		}

		// Create new image views
		CreateSwapChainImageViews();
	}
	
	void VulkanSwapChain::Destroy() {
		Cleanup(swap_chain);
	}

	void VulkanSwapChain::Cleanup(VkSwapchainKHR& swap_chain) {
		for (uint32_t i = 0; i < image_views.size(); i++) {
			vkDestroyImageView(logical_device, image_views[i], nullptr);
		}
		image_views.clear();
		vkDestroySwapchainKHR(logical_device, swap_chain, nullptr);
		swap_chain = VK_NULL_HANDLE;
	}

	void VulkanSwapChain::CreateSwapChainImageViews() {
		vkGetSwapchainImagesKHR(logical_device, this->swap_chain, &this->image_count, nullptr);
		std::vector<VkImage> images(this->image_count);
		vkGetSwapchainImagesKHR(logical_device, this->swap_chain, &this->image_count, images.data());
		image_views.resize(this->image_count);
		for (uint32_t i = 0; i < image_views.size(); i++) {
			VkImageViewCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.pNext = nullptr;
			create_info.format = this->swap_surface_format.format;
			create_info.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.flags = 0;
			create_info.image = images[i];
			if (vkCreateImageView(logical_device, &create_info, nullptr, &image_views[i]) != VK_SUCCESS) {
				throw std::runtime_error("fail to create image view");
			}
		}
	}
}