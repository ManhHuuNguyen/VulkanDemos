#pragma once
#include "vulkan/vulkan.h"
#include <vector>

namespace vk {

	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> present_modes;
	};

	SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice physical_device, VkSurfaceKHR surface);

	namespace {}

	class VulkanSwapChain {
	public:
		//create new swapchain, destroy old swapchain if exists
		void Create(VkDevice logical_device, VkPhysicalDevice physical_device, VkSurfaceKHR surface, uint32_t width, uint32_t height);
		
		void Destroy();
		
	private:
		VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR>& present_modes);

		VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR capabilities, uint32_t width, uint32_t height);

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR>& formats);

		uint32_t ChooseImageCount(VkSurfaceCapabilitiesKHR capabilities);

		void CreateSwapChainImageViews();

		void Cleanup(VkSwapchainKHR& swap_chain);

	public:
		uint32_t image_count;
		VkExtent2D swap_extent;
		VkSurfaceFormatKHR swap_surface_format;
		std::vector<VkImageView> image_views;
		VkSwapchainKHR swap_chain = VK_NULL_HANDLE;
	private:
		
		VkDevice logical_device;
		VkPresentModeKHR present_mode;
		
	};
}