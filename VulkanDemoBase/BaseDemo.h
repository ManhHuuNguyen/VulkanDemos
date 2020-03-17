#pragma once
#define GLFW_INCLUDE_VULKAN
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "GLFW/glfw3.h"
#include "VulkanPhysicalDevice.h"
#include <vector>
#include "VulkanSwapChain.h"
#include "VulkanQueue.h"
#include "VulkanCompositeImage.h"

class BaseDemo {
public:
	void Run();

protected:
	// methods must be overriden
	virtual const char* GetWindowTitle() = 0;
	virtual uint32_t GetWindowInitWidth() = 0;
	virtual uint32_t GetWindowInitHeight() = 0;
	virtual std::vector<vk::QueueCreationRequirement> GetQueueFamilyRequirements() = 0;
	virtual bool ShowFPS() = 0;
	virtual void Draw() = 0;
	virtual void CreatePermanentResources() = 0; // for resources not recreated when window is resized, such as vertex buffer
	virtual void CleanupPermanentResources() = 0;
	virtual void CreateNonPermanentResources() = 0; // for resources recreated when window is resized, such as graphic pipeline
	virtual void CleanupNonPermanentResources() = 0;
	// methods can be overriden
	virtual std::vector<const char*> GetDeviceExtensions();
	virtual std::vector<const char*> GetValidationLayers();
	virtual std::vector<const char*> GetRequiredInstanceExtensions();

	void RecreateSwapChain();// to be called when window resizes

private:
	void InitWindow();
	void InitVulkan();
	void MainLoop();
	void Cleanup();
	void Render();
	
	void CleanupSwapChain();

	// methods called in initVulkan
	void CreateVulkanInstance();
	void CreateDebugMessenger();
	void CreateSurface();
	void PickPhysicalDeviceAndCreateLogicalDevice();
	void CreateGraphicAndPresentCommandPool();
	void CreateSyncObjects();
	void CreateSwapChain();
	void CreateDepthStencil();
	void CreateRenderpass();
	void CreateFramebuffers();
	void SetupImagesInflightFences();
	// static methods
	static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

public:
	GLFWwindow* window;
	bool framebuffer_resized;
	VkInstance instance;
	VkDebugUtilsMessengerEXT debug_messenger;
	VkSurfaceKHR surface;
	VkPhysicalDevice physical_device = VK_NULL_HANDLE;
	VkDevice logical_device;
	std::vector<vk::VulkanQueue> queues;
	vk::VulkanSwapChain vulkan_swap_chain;
	std::vector<VkSemaphore> image_available_semaphores;
	std::vector<VkSemaphore> render_finished_semaphores;
	std::vector<VkFence> cmdbuffers_inflight;
	VkCommandPool command_pool;
	std::vector<VkCommandBuffer> draw_cmd_buffers;
	vk::VulkanCompositeImage depth_stencil;
	VkRenderPass renderpass;
	std::vector<VkFramebuffer> swapchain_framebuffers;
	std::vector<VkFence> images_inflight;
	uint32_t current_frame;
	uint32_t fps_count = 0;
#ifdef NDEBUG
	const static bool VALIDATION_LAYER_ENABLED = false;
#else
	const static bool VALIDATION_LAYER_ENABLED = true;
#endif
	const uint32_t MAX_FRAMES_INFLIGHT = 5;
};


#define MAIN_METHOD(ChildDemo)  \
	int main(int argc, char** argv) {BaseDemo* demo = new ChildDemo(); \
	demo->Run(); \
	return 0; \
}
