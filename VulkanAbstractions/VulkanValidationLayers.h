#pragma once
#include "vulkan/vulkan.h"
#include <vector>

namespace vk {
	
	VkResult CreateDebugUtilMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* p_create_info, 
		const VkAllocationCallbacks* p_allocator, VkDebugUtilsMessengerEXT* p_debug_messenger);
	
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT& debug_messenger, const VkAllocationCallbacks* p_allocator);

	void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& create_info);

	bool AreValidationLayersSupported(std::vector<const char*>& validation_layers);

	namespace {
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
			VkDebugUtilsMessageTypeFlagsEXT message_type,
			const VkDebugUtilsMessengerCallbackDataEXT* p_callback_data,
			void* p_user_data);
	}
}