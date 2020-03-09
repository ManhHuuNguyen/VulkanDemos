#include "VulkanQueue.h"
#include <stdexcept>
#include "VulkanHelper.h"

namespace vk {

	VulkanQueue::VulkanQueue(uint32_t family_index, uint32_t index, VkQueue queue, VulkanQueueCharacteristic characteristic) {
		this->family_index = family_index;
		this->queue = queue;
		this->characteristic = characteristic;
		this->index = index;
	}

	VulkanQueue VulkanQueue::GetQueue(VkDevice logical_device, uint32_t family_index, uint32_t index, VulkanQueueCharacteristic characteristic) {
		VkQueue queue;
		vkGetDeviceQueue(logical_device, family_index, index, &queue);
		return VulkanQueue(family_index, index, queue, characteristic);
	}

	void VulkanQueue::SubmitSingleCmdBuffer(std::vector<VkSemaphore>& wait_semaphores, std::vector<VkPipelineStageFlags>& waitstage_flags,
		VkCommandBuffer command_buffer, std::vector<VkSemaphore>& signal_semaphores, VkFence fence) {
		std::vector<VkCommandBuffer> command_buffers = { command_buffer };
		VkSubmitInfo submit_info = vk::init::CreateSubmitInfo(wait_semaphores, waitstage_flags, command_buffers, signal_semaphores);
		if (vkQueueSubmit(queue, 1, &submit_info, fence) != VK_SUCCESS) {
			throw std::runtime_error("fail to submit command buffer!");
		}
	}

	void VulkanQueue::SubmitMultipleCmdBuffers(std::vector<VkSemaphore>& wait_semaphores, std::vector<VkPipelineStageFlags>& waitstage_flags,
		std::vector<VkCommandBuffer>& command_buffers, std::vector<VkSemaphore>& signal_semaphores, VkFence fence) {
		VkSubmitInfo submit_info = vk::init::CreateSubmitInfo(wait_semaphores, waitstage_flags, command_buffers, signal_semaphores);
		if (vkQueueSubmit(queue, 1, &submit_info, fence) != VK_SUCCESS) {
			throw std::runtime_error("fail to submit multiple command buffers!");
		}
	}

	void VulkanQueue::SubmitMultipleVkSubmitInfos(std::vector<VkSubmitInfo>& submit_infos, VkFence fence) {
		if (vkQueueSubmit(queue, static_cast<uint32_t>(submit_infos.size()), submit_infos.data(), fence) != VK_SUCCESS) {
			throw std::runtime_error("fail to submit multiple VkSubmitInfo!");
		}
	}

	VkResult VulkanQueue::PresentImage(std::vector<VkSemaphore>& wait_semaphores, VkSwapchainKHR swapchain, uint32_t image_index) {
		if (!characteristic.is_present) {
			throw std::runtime_error("cannot present because this is not a present queue");
		}
		VkPresentInfoKHR present_info = {};
		present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		present_info.waitSemaphoreCount = static_cast<uint32_t>(wait_semaphores.size());
		present_info.pWaitSemaphores = wait_semaphores.data();
		VkSwapchainKHR swapchains[] = { swapchain };
		present_info.swapchainCount = 1;
		present_info.pSwapchains = swapchains;
		present_info.pImageIndices = &image_index;
		return vkQueuePresentKHR(queue, &present_info);
	}

	void VulkanQueue::WaitIdle() {
		vkQueueWaitIdle(queue);
	}

}