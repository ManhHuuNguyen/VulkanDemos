#pragma once
#include "vulkan/vulkan.h"
#include "VulkanQueue.h"
//TODO: now this is a one buffer one memory allocation type of situation
// In real applications, you often need an association of one allocation, multiple buffers
//We need to change the name of this class to make clear of the one-to-one relationship, and also the name of VulkanCompositeImage as well, which sufffers from the same thing
// Later we also need to have a many-to-one relationship class
namespace vk {

	class VulkanCompositeBuffer {
	public:
		void CreateBuffer(VkDevice logical_device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags usage, VkSharingMode sharing_mode, VkMemoryPropertyFlags mem_properties);
		void DestroyBuffer();
		void CopyFromHostData(void * data, uint32_t data_size, uint32_t offset);
		void TransferFromAnotherBuffer(VulkanCompositeBuffer & src_buffers, VkCommandPool pool, uint32_t src_offset, uint32_t dst_offset, uint32_t size, 
			VulkanQueue & queue, VkFence wait_fence);
	public:
		VkMemoryPropertyFlags mem_properties;
		VkBufferUsageFlags usage;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory buffer_memory = VK_NULL_HANDLE;
		VkDeviceSize size;
	private:
		VkDevice logical_device = VK_NULL_HANDLE;
	};
}