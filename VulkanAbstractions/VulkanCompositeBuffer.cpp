#include "VulkanCompositeBuffer.h"
#include <stdexcept>
#include "VulkanHelper.h"

namespace vk {

	void VulkanCompositeBuffer::CreateBuffer(VkDevice logical_device, VkPhysicalDevice physical_device, VkDeviceSize size, VkBufferUsageFlags usage, 
		VkSharingMode sharing_mode, VkMemoryPropertyFlags mem_properties) {
		if (this->buffer != VK_NULL_HANDLE) {
			throw std::runtime_error("this buffer is already created");
		}
		this->logical_device = logical_device;
		this->usage = usage;
		this->mem_properties = mem_properties;
		this->size = size;
		VkBufferCreateInfo buffer_info = {};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = this->size;
		buffer_info.usage = this->usage;
		buffer_info.sharingMode = sharing_mode;
		if (vkCreateBuffer(logical_device, &buffer_info, nullptr, &this->buffer) != VK_SUCCESS) {
			throw std::runtime_error("fail to create buffer");
		}
		VkMemoryRequirements mem_req;
		vkGetBufferMemoryRequirements(this->logical_device, this->buffer, &mem_req);
		vk::init::AllocateMemory(this->logical_device, physical_device, mem_req, this->mem_properties, &this->buffer_memory);
		vkBindBufferMemory(this->logical_device, this->buffer, this->buffer_memory, 0);
	}

	void VulkanCompositeBuffer::DestroyBuffer() {
		if (this->buffer == VK_NULL_HANDLE) {
			throw std::runtime_error("this buffer is not yet created or is already destroyed");
		}
		vkDestroyBuffer(this->logical_device, this->buffer, nullptr);
		vkFreeMemory(this->logical_device, this->buffer_memory, nullptr);
		this->buffer = VK_NULL_HANDLE;
		this->buffer_memory = VK_NULL_HANDLE;
	}

	void VulkanCompositeBuffer::CopyFromHostData(void* data, uint32_t data_size, uint32_t offset) {
		if (this->buffer == VK_NULL_HANDLE) {
			throw std::runtime_error("this buffer is not yet created or is already destroyed");
		}
		if ((this->mem_properties & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == 0) {
			throw std::runtime_error("this buffer memory is not host visible");
		}
		void* mapped_region;
		vkMapMemory(this->logical_device, this->buffer_memory, offset, data_size, 0, &mapped_region);
		memcpy(mapped_region, data, data_size);
		vkUnmapMemory(this->logical_device, this->buffer_memory);
	}

	void VulkanCompositeBuffer::TransferFromAnotherBuffer(VulkanCompositeBuffer& src_buffers, VkCommandPool pool, uint32_t src_offset, uint32_t dst_offset, uint32_t size, 
		VulkanQueue& queue, VkFence wait_fence) {
		if (this->buffer == VK_NULL_HANDLE || src_buffers.buffer == VK_NULL_HANDLE) {
			throw std::runtime_error("either buffer is not yet created or is already destroyed");
		}
		if ((this->usage & VK_BUFFER_USAGE_TRANSFER_DST_BIT) == 0) {
			throw std::runtime_error("this buffer cannot be usage as a transfer destination");
		}
		if ((src_buffers.usage & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) == 0) {
			throw std::runtime_error("src buffer cannot be used as a transfer source");
		}
		VkCommandBufferAllocateInfo alloc_info = {};
		alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc_info.commandPool = pool;
		alloc_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(this->logical_device, &alloc_info, &command_buffer);
		vk::util::BeginCmdBuffer(command_buffer, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT, nullptr);
		VkBufferCopy copy_region = {};
		copy_region.srcOffset = src_offset;
		copy_region.dstOffset = dst_offset;
		copy_region.size = size;
		vkCmdCopyBuffer(command_buffer, src_buffers.buffer, this->buffer, 1, &copy_region);
		if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) {
			throw std::runtime_error("fail to end command buffer recording");
		}
		std::vector<VkSemaphore> semaphores;
		std::vector<VkPipelineStageFlags> flags;
		queue.SubmitSingleCmdBuffer(semaphores, flags, command_buffer, semaphores, wait_fence);
	}
}