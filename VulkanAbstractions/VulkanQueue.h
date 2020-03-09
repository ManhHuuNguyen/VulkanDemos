#pragma once
#include "vulkan/vulkan.h"
#include <vector>
namespace vk {

	struct VulkanQueueCharacteristic {
		bool is_graphic;
		bool is_compute;
		bool is_transfer;
		bool is_sparse_binding;
		bool is_protected;
		bool is_present;
	};

	class VulkanQueue {
	public:
		static VulkanQueue GetQueue(VkDevice logical_device, uint32_t family_index, uint32_t index,VulkanQueueCharacteristic characteristic);

		void SubmitSingleCmdBuffer(std::vector<VkSemaphore> & wait_semaphores, std::vector<VkPipelineStageFlags> & waitstage_flags, 
			VkCommandBuffer command_buffer, std::vector<VkSemaphore>& signal_semaphores, VkFence fence);

		void SubmitMultipleCmdBuffers(std::vector<VkSemaphore>& wait_semaphores, std::vector<VkPipelineStageFlags>& waitstage_flags,
			std::vector<VkCommandBuffer> & command_buffers, std::vector<VkSemaphore>& signal_semaphores, VkFence fence);

		void SubmitMultipleVkSubmitInfos(std::vector<VkSubmitInfo> & submit_infos, VkFence fence);

		VkResult PresentImage(std::vector<VkSemaphore> & wait_semaphores, VkSwapchainKHR swapchain, uint32_t image_index);

		void WaitIdle();
	private:
		VulkanQueue(uint32_t family_index, uint32_t index, VkQueue queue, VulkanQueueCharacteristic characteristic);
	public:
		uint32_t family_index;
		VkQueue queue;
		VulkanQueueCharacteristic characteristic;
		uint32_t index; // the index of the queue, shouldnt be confused with family index
	
	};
}