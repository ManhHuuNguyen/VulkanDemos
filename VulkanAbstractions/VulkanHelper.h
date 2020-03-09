#pragma once
#include "vulkan/vulkan.h"
#include <vector>
#include <iostream>

#define VK_CHECK_RESULT(f, str)																\
{																							\
	VkResult res = (f);																		\
	if (res != VK_SUCCESS) {																\
		std::err << str << "\" in " << __FILE__ << " at line " << __LINE__  << std::endl;   \
		assert(res == VK_SUCCESS);															\
	}																						\
}

namespace vk {
	namespace init {
		 void CreateSemaphore(VkDevice logical_device, VkSemaphore * semaphore);
		 
		 void CreateCmdBuffer(VkDevice logical_device, VkCommandPool command_pool, VkCommandBufferLevel level, uint32_t command_buffer_count, VkCommandBuffer * cmdbuffer_arr);
		 
		 void CreateFence(VkDevice logical_device, VkFenceCreateFlags flags, VkFence* fence);

		 void AllocateMemory(VkDevice logical_device, VkPhysicalDevice physical_device, VkMemoryRequirements mem_req, 
			 VkMemoryPropertyFlags mem_properties, VkDeviceMemory* device_memory);

		 void CreateDescriptorSetLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayoutBinding> & bindings, VkDescriptorSetLayout * descriptor_set_layout);

		 //a descriptor pool can allocate multiple type of descriptor sets. Each type of descripor set is described in a VkDescriptorPoolSize struct
		 // which contains the type of descriptor and the number of descriptor inside a set. 
		 // max_set is the maximum number of descriptor set this pool can allocate
		 void CreateDescriptorPool(VkDevice logical_device, std::vector<VkDescriptorPoolSize> & poolsizes, uint32_t max_sets, VkDescriptorPool* pool);

		 void AllocateDescriptorSets(VkDevice logical_device, VkDescriptorPool descriptor_pool, std::vector<VkDescriptorSetLayout> & layouts, std::vector<VkDescriptorSet> & sets);

		 void CreateFrameBuffer(VkDevice logical_device, VkRenderPass renderpass, std::vector<VkImageView>& attachments, uint32_t width, uint32_t height, VkFramebuffer* framebuffer);
		 
		 VkSubmitInfo CreateSubmitInfo(std::vector<VkSemaphore>& wait_semaphores, std::vector<VkPipelineStageFlags>& waitstage_flags,
			 std::vector<VkCommandBuffer>& command_buffers, std::vector<VkSemaphore>& signal_semaphores);
		 
		 VkVertexInputBindingDescription CreateVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate);

		 VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

		 VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptor_type, uint32_t descriptor_count, VkShaderStageFlags flags);

		 VkWriteDescriptorSet CreateWriteDescriptorSet(VkDescriptorSet descriptor_set, uint32_t dst_binding, uint32_t dst_array_ele,
			 VkDescriptorType type, uint32_t descriptor_count, VkDescriptorBufferInfo* buffer_info, VkDescriptorImageInfo * image_info);

		 VkDescriptorBufferInfo CreateDescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range);

		 VkDescriptorImageInfo CreateDescriptorImageInfo(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout);

		 VkSpecializationMapEntry CreateSpecializationMapEntry(uint32_t constant_id, uint32_t offset, uint32_t size);

		 VkSpecializationInfo CreateSpecializationInfo(std::vector<VkSpecializationMapEntry>& map_entries, uint32_t data_size, void* p_data);
		
	}

	namespace util {
		void BeginCmdBuffer(VkCommandBuffer cmd_buffer, VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo * inheritance_info);
		
		void BeginRenderpass(VkCommandBuffer command_buffer, VkRenderPass renderpass, VkFramebuffer framebuffer, VkOffset2D offset, VkExtent2D extent,
			std::vector<VkClearValue> & clear_values, VkSubpassContents supbass_contents);

		uint32_t GetVkBoolean(bool boolean);

		uint32_t CalculateObjectSize(uint32_t actual_object_size, uint32_t alignment);

	}

}