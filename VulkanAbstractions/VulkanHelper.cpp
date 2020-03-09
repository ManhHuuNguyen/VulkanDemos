#include "VulkanHelper.h"
#include <stdexcept>
#include "VulkanPhysicalDevice.h"

namespace vk {
	namespace init {

		void CreateSemaphore(VkDevice logical_device, VkSemaphore * semaphore) {
			VkSemaphoreCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			if (vkCreateSemaphore(logical_device, &create_info, nullptr, semaphore) != VK_SUCCESS) {
				throw std::runtime_error("fail to create semaphore");
			}
		}

		void CreateFence(VkDevice logical_device, VkFenceCreateFlags flags, VkFence* fence) {
			VkFenceCreateInfo fence_info = {};
			fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			fence_info.flags = flags;
			if (vkCreateFence(logical_device, &fence_info, nullptr, fence) != VK_SUCCESS) {
				throw std::runtime_error("fail to create fence");
			}
		}

		void CreateCmdBuffer(VkDevice logical_device, VkCommandPool command_pool, VkCommandBufferLevel level, uint32_t command_buffer_count, VkCommandBuffer* cmdbuffer_arr) {
			VkCommandBufferAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			alloc_info.commandPool = command_pool;
			alloc_info.level = level;
			alloc_info.commandBufferCount = command_buffer_count;
			if (vkAllocateCommandBuffers(logical_device, &alloc_info, cmdbuffer_arr) != VK_SUCCESS) {
				throw std::runtime_error("fail to allocate command buffers");
			}
		}

		void AllocateMemory(VkDevice logical_device, VkPhysicalDevice physical_device, VkMemoryRequirements mem_req, VkMemoryPropertyFlags mem_properties, VkDeviceMemory* device_memory) {
			VkMemoryAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			alloc_info.allocationSize = mem_req.size;
			alloc_info.memoryTypeIndex = FindMemoryType(physical_device, mem_req.memoryTypeBits, mem_properties);
			if (vkAllocateMemory(logical_device, &alloc_info, nullptr, device_memory) != VK_SUCCESS) {
				throw std::runtime_error("fail to allocaote memory");
			}
		}

		void CreateDescriptorSetLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayoutBinding>& bindings, VkDescriptorSetLayout* descriptor_set_layout) {
			VkDescriptorSetLayoutCreateInfo layout_info = {};
			layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_info.bindingCount = static_cast<uint32_t>(bindings.size());
			layout_info.pBindings = bindings.data();
			if (vkCreateDescriptorSetLayout(logical_device, &layout_info, nullptr, descriptor_set_layout) != VK_SUCCESS) {
				throw std::runtime_error("fail to create descriptor set layout");
			}
		}

		void CreateDescriptorPool(VkDevice logical_device, std::vector<VkDescriptorPoolSize>& poolsizes, uint32_t max_sets, VkDescriptorPool* pool) {
			VkDescriptorPoolCreateInfo create_info = {};
			create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			create_info.poolSizeCount = static_cast<uint32_t>(poolsizes.size());
			create_info.pPoolSizes = poolsizes.data();
			create_info.maxSets = max_sets;
			if (vkCreateDescriptorPool(logical_device, &create_info, nullptr, pool) != VK_SUCCESS) {
				throw std::runtime_error("fail to create descriptor pool");
			}
		}

		void AllocateDescriptorSets(VkDevice logical_device, VkDescriptorPool descriptor_pool, std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkDescriptorSet>& sets) {
			VkDescriptorSetAllocateInfo alloc_info = {};
			alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			alloc_info.descriptorPool = descriptor_pool;
			alloc_info.descriptorSetCount = static_cast<uint32_t>(layouts.size());
			alloc_info.pSetLayouts = layouts.data();
			if (vkAllocateDescriptorSets(logical_device, &alloc_info, sets.data()) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate descriptor sets!");
			}
		}

		void CreateFrameBuffer(VkDevice logical_device, VkRenderPass renderpass, std::vector<VkImageView> & attachments, uint32_t width, uint32_t height, VkFramebuffer * framebuffer) {
			VkFramebufferCreateInfo framebuffer_info = {};
			framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebuffer_info.pNext = nullptr;
			framebuffer_info.renderPass = renderpass;
			framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebuffer_info.pAttachments = attachments.data();
			framebuffer_info.width = width;
			framebuffer_info.height = height;
			framebuffer_info.layers = 1;
			if (vkCreateFramebuffer(logical_device, &framebuffer_info, nullptr, framebuffer) != VK_SUCCESS) {
				throw std::runtime_error("fail to create framebuffer");
			}
			
		}

		VkSubmitInfo CreateSubmitInfo(std::vector<VkSemaphore>& wait_semaphores, std::vector<VkPipelineStageFlags>& waitstage_flags,
			std::vector<VkCommandBuffer>& command_buffers, std::vector<VkSemaphore>& signal_semaphores) {
			VkSubmitInfo submit_info = {};
			submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			submit_info.waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size());
			submit_info.pWaitSemaphores = wait_semaphores.data();
			submit_info.pWaitDstStageMask = waitstage_flags.data();

			submit_info.commandBufferCount = static_cast<uint32_t>(command_buffers.size());
			submit_info.pCommandBuffers = command_buffers.data();

			submit_info.signalSemaphoreCount = static_cast<uint32_t>(signal_semaphores.size());
			submit_info.pSignalSemaphores = signal_semaphores.data();
			return submit_info;
		}

		VkVertexInputBindingDescription CreateVertexInputBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate input_rate) {
			VkVertexInputBindingDescription desc = {};
			desc.binding = binding;
			desc.stride = stride;
			desc.inputRate = input_rate;
			return desc;
		}

		VkVertexInputAttributeDescription CreateVertexInputAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset) {
			VkVertexInputAttributeDescription desc = {};
			desc.binding = binding;
			desc.location = location;
			desc.format = format;
			desc.offset = offset;
			return desc;
		}

		VkDescriptorSetLayoutBinding CreateDescriptorSetLayoutBinding(uint32_t binding, VkDescriptorType descriptor_type, uint32_t descriptor_count, VkShaderStageFlags flags) {
			VkDescriptorSetLayoutBinding layout_binding = {};
			layout_binding.binding = binding;
			layout_binding.descriptorType = descriptor_type;
			layout_binding.descriptorCount = descriptor_count;
			layout_binding.stageFlags = flags;
			return layout_binding;
		}

		VkWriteDescriptorSet CreateWriteDescriptorSet(VkDescriptorSet descriptor_set, uint32_t dst_binding, uint32_t dst_array_ele, 
			VkDescriptorType type, uint32_t descriptor_count, VkDescriptorBufferInfo * buffer_info, VkDescriptorImageInfo* image_info) {
			VkWriteDescriptorSet descriptor_write = {};
			descriptor_write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptor_write.dstSet = descriptor_set;
			descriptor_write.dstBinding = dst_binding;
			descriptor_write.dstArrayElement = dst_array_ele;
			descriptor_write.descriptorType = type;
			descriptor_write.descriptorCount = descriptor_count;
			descriptor_write.pBufferInfo = buffer_info;
			descriptor_write.pImageInfo = image_info;
			return descriptor_write;
		}

		VkDescriptorBufferInfo CreateDescriptorBufferInfo(VkBuffer buffer, VkDeviceSize offset, VkDeviceSize range) {
			VkDescriptorBufferInfo binding_info = {};
			binding_info.buffer = buffer;
			binding_info.offset = offset;
			binding_info.range = range;
			return binding_info;
		}

		VkDescriptorImageInfo CreateDescriptorImageInfo(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout) {
			VkDescriptorImageInfo binding_info = {};
			binding_info.sampler = sampler;
			binding_info.imageView = image_view;
			binding_info.imageLayout = image_layout;
			return binding_info;
		}

		VkSpecializationMapEntry CreateSpecializationMapEntry(uint32_t constant_id, uint32_t offset, uint32_t size) {
			VkSpecializationMapEntry map_entry = {};
			map_entry.constantID = constant_id;
			map_entry.offset = offset;
			map_entry.size = size;
			return map_entry;
		}

		VkSpecializationInfo CreateSpecializationInfo(std::vector<VkSpecializationMapEntry> & map_entries, uint32_t data_size, void * p_data) {
			VkSpecializationInfo specialization_info = {};
			specialization_info.mapEntryCount = static_cast<uint32_t>(map_entries.size());
			specialization_info.pMapEntries = map_entries.data();
			specialization_info.dataSize = data_size;
			specialization_info.pData = p_data;
			return specialization_info;
		}

	}

	namespace util {
		void BeginCmdBuffer(VkCommandBuffer cmd_buffer, VkCommandBufferUsageFlags flags, VkCommandBufferInheritanceInfo* inheritance_info) {
			VkCommandBufferBeginInfo begin_info = {};
			begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			begin_info.flags = flags;
			begin_info.pInheritanceInfo = inheritance_info;
			if (vkBeginCommandBuffer(cmd_buffer, &begin_info) != VK_SUCCESS) {
				throw std::runtime_error("fail to start recording command buffer");
			}
		}

		void BeginRenderpass(VkCommandBuffer command_buffer, VkRenderPass renderpass, VkFramebuffer framebuffer, VkOffset2D offset, VkExtent2D extent, 
			std::vector<VkClearValue> & clear_values, VkSubpassContents supbass_contents) {
			VkRenderPassBeginInfo render_pass_info = {};
			render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			render_pass_info.renderPass = renderpass;
			render_pass_info.framebuffer = framebuffer;
			render_pass_info.renderArea.offset = offset;
			render_pass_info.renderArea.extent = extent;

			render_pass_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
			render_pass_info.pClearValues = clear_values.data();

			vkCmdBeginRenderPass(command_buffer, &render_pass_info, supbass_contents);
		}

		uint32_t GetVkBoolean(bool boolean) {
			return (boolean)? 1: 0;
		}

		uint32_t CalculateObjectSize(uint32_t actual_object_size, uint32_t alignment) {
			uint32_t times =  (actual_object_size % alignment == 0)? (actual_object_size / alignment): (actual_object_size / alignment) + 1;
			return times * alignment;
		}

	}
}