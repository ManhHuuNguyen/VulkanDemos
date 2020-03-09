#include "BaseDemo.h"
#include <stdexcept>
#include <iostream>
#include "VulkanValidationLayers.h"
#include "VulkanLogicalDevice.h"
#include "VulkanHelper.h"
#include <array>
#include <chrono>


void BaseDemo::InitWindow() {
	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

	window = glfwCreateWindow(GetWindowInitWidth(), GetWindowInitHeight(), GetWindowTitle(), nullptr, nullptr);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, BaseDemo::FramebufferResizeCallback);
}
void BaseDemo::Run() {
	InitWindow();
	InitVulkan();
	MainLoop();
	Cleanup();
}


void BaseDemo::MainLoop() {
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		Render();
	}
}

void BaseDemo::InitVulkan() {
	//permanent resources
	CreateVulkanInstance();
	if (VALIDATION_LAYER_ENABLED) {
		CreateDebugMessenger();
	}
	CreateSurface();
	PickPhysicalDeviceAndCreateLogicalDevice();
	CreateGraphicAndPresentCommandPool();
	CreateSyncObjects();
	CreatePermanentResources();
	//non-permanent resources
	CreateSwapChain();
	CreateDepthStencil();
	CreateRenderpass();
	CreateFramebuffers();
	SetupImagesInflightFences();
	CreateNonPermanentResources();
}

void BaseDemo::Cleanup() {
	vkDeviceWaitIdle(logical_device);
	//cleanup vulkan
	// non-permanent resources
	CleanupSwapChain();
	// permanent resources
	CleanupPermanentResources();
	for (uint32_t i = 0; i < MAX_FRAMES_INFLIGHT; i++) {
		vkDestroySemaphore(logical_device, image_available_semaphores[i], nullptr);
		vkDestroySemaphore(logical_device, render_finished_semaphores[i], nullptr);
		vkDestroyFence(logical_device, cmdbuffers_inflight[i], nullptr);
	}
	vkDestroyCommandPool(logical_device, command_pool, nullptr);
	vkDestroyDevice(logical_device, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	if (VALIDATION_LAYER_ENABLED) {
		vk::DestroyDebugUtilsMessengerEXT(instance, debug_messenger, nullptr);
	}
	vkDestroyInstance(instance, nullptr);
	//cleanup window
	glfwDestroyWindow(window);
	glfwTerminate();
}

void BaseDemo::CreateVulkanInstance() {
	std::vector<const char*> validation_layers = GetValidationLayers();
	if (VALIDATION_LAYER_ENABLED && !vk::AreValidationLayersSupported(validation_layers)) {
		throw std::runtime_error("validation layer is enabled but is not supported");
	}
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.pApplicationName = GetWindowTitle();
	app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.pEngineName = "Manhs";
	app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	app_info.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	create_info.pApplicationInfo = &app_info;

	//global instance extensions
	std::vector<const char*> extensions = GetRequiredInstanceExtensions();
	create_info.ppEnabledExtensionNames = extensions.data();
	create_info.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	
	// global validation layers
	VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {};
	if (VALIDATION_LAYER_ENABLED) {
		create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
		create_info.ppEnabledLayerNames = validation_layers.data();
		vk::PopulateDebugMessengerCreateInfo(debug_create_info);
		//add debug_create_info to create_info struct
		create_info.pNext = &debug_create_info;
	}
	else {
		create_info.enabledLayerCount = 0;
		create_info.ppEnabledLayerNames = nullptr;
		create_info.pNext = nullptr;
	}
	if (vkCreateInstance(&create_info, nullptr, &instance) != VK_SUCCESS) {
		throw std::runtime_error("fail to create vk instance");
	}
	
}

std::vector<const char*> BaseDemo::GetRequiredInstanceExtensions() {
	uint32_t glfw_extension_count = 0;
	const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count); // all extensions required by glfw
	std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count); //start and end for vector range constructor
	if (VALIDATION_LAYER_ENABLED) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}
	return extensions;
}


std::vector<const char*> BaseDemo::GetValidationLayers() {
	return { "VK_LAYER_KHRONOS_validation" };
}

std::vector<const char*> BaseDemo::GetDeviceExtensions() {
	return { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

void BaseDemo::CreateSurface() {
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("fail to create window surface");
	}
}

void BaseDemo::CreateDebugMessenger() {
	VkDebugUtilsMessengerCreateInfoEXT create_info = {};
	vk::PopulateDebugMessengerCreateInfo(create_info);
	if (vk::CreateDebugUtilMessengerEXT(instance, &create_info, nullptr, &debug_messenger) != VK_SUCCESS) {
		throw std::runtime_error("Fail to setup debug messenger");
	}
	
}

void BaseDemo::PickPhysicalDeviceAndCreateLogicalDevice() {
	// pick physical devices
	std::vector<uint32_t> queue_family_indices;
	std::vector<vk::QueueCreationRequirement> queue_family_reqs = GetQueueFamilyRequirements();
	std::vector<const char*> device_extensions = GetDeviceExtensions();
	physical_device = vk::PickPhysicalDevice(instance, surface, queue_family_reqs, device_extensions, queue_family_indices);
	if (physical_device == VK_NULL_HANDLE) {
		throw std::runtime_error("cannot find appropriate physical device");
	}
	// create logical devices
	std::vector<const char*> validation_layers = GetValidationLayers();
	vk::CreateLogicalDevice(queue_family_reqs, queue_family_indices, device_extensions, VALIDATION_LAYER_ENABLED, validation_layers, physical_device, logical_device);
	// get queues
	for (uint32_t i = 0; i < queue_family_indices.size(); i++) {
		for (uint32_t count = 0; count < queue_family_reqs[i].num_queue; count++) {
			vk::VulkanQueue queue = vk::VulkanQueue::GetQueue(logical_device, queue_family_indices[i], count, queue_family_reqs[i].types);
			queues.push_back(queue);
		}
	}
}

void BaseDemo::CreateSwapChain() {
	// create swap chain
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	vulkan_swap_chain.Create(logical_device, physical_device, surface, width, height);
}

void BaseDemo::CreateSyncObjects() {
	image_available_semaphores.resize(MAX_FRAMES_INFLIGHT);
	render_finished_semaphores.resize(MAX_FRAMES_INFLIGHT);
	cmdbuffers_inflight.resize(MAX_FRAMES_INFLIGHT);
	for (uint32_t i = 0; i < MAX_FRAMES_INFLIGHT; i++) {
		vk::init::CreateSemaphore(logical_device, &image_available_semaphores[i]);
		vk::init::CreateSemaphore(logical_device, &render_finished_semaphores[i]);
		vk::init::CreateFence(logical_device, VK_FENCE_CREATE_SIGNALED_BIT, &cmdbuffers_inflight[i]);
	}
}

void BaseDemo::Render() {
	Draw();
	fps_count += 1;
	static auto start_time = std::chrono::high_resolution_clock::now();
	if (ShowFPS()) {
		auto current_time = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
		std::cout << "FPS: " << (fps_count / elapsed) << "\n";
	}
}

void BaseDemo::CreateGraphicAndPresentCommandPool() {
	VkCommandPoolCreateInfo create_info = {};
	create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	if (!queues[0].characteristic.is_graphic || !queues[0].characteristic.is_present) {
		throw std::runtime_error("the first queue in vector must be both graphic and present");
	}
	create_info.queueFamilyIndex = queues[0].family_index; 
	if (vkCreateCommandPool(logical_device, &create_info, nullptr, &command_pool) != VK_SUCCESS) {
		throw std::runtime_error("fail to create graphic/present command queue");
	}
}

void BaseDemo::CreateDepthStencil() {
	VkImageCreateInfo depth_create_info = {};
	depth_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	depth_create_info.imageType = VK_IMAGE_TYPE_2D;
	depth_create_info.format = vk::GetSupportedDepthFormat(physical_device);
	depth_create_info.extent = { vulkan_swap_chain.swap_extent.width, vulkan_swap_chain.swap_extent.height, 1 };
	depth_create_info.mipLevels = 1;
	depth_create_info.arrayLayers = 1;
	depth_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
	depth_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
	depth_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	depth_stencil.Create(physical_device, logical_device, depth_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	
}

void BaseDemo::CreateRenderpass() {
	std::array<VkAttachmentDescription, 2> attachments = {}; // so that elements in array are initialized to zero. 
	//without '= {}', they won't be and I will have to specify every field
	//color attachment
	attachments[0].format = vulkan_swap_chain.swap_surface_format.format;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
	// depth attachment
	attachments[1].format = depth_stencil.format;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference color_ref = {};
	color_ref.attachment = 0;
	color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	VkAttachmentReference depth_ref = {};
	depth_ref.attachment = 1;
	depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass_desc = {};
	subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass_desc.colorAttachmentCount = 1;
	subpass_desc.pColorAttachments = &color_ref;
	subpass_desc.pDepthStencilAttachment = &depth_ref;
	subpass_desc.inputAttachmentCount = 0;
	subpass_desc.pInputAttachments = nullptr;
	subpass_desc.preserveAttachmentCount = 0;
	subpass_desc.pPreserveAttachments = nullptr;
	subpass_desc.pResolveAttachments = nullptr;
	
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	dependency.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

	VkRenderPassCreateInfo render_pass_info = {};
	render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	render_pass_info.attachmentCount = static_cast<uint32_t>(attachments.size());
	render_pass_info.pAttachments = attachments.data();
	render_pass_info.subpassCount = 1;
	render_pass_info.pSubpasses = &subpass_desc;
	render_pass_info.dependencyCount = 1;
	render_pass_info.pDependencies = &dependency;

	if (vkCreateRenderPass(logical_device, &render_pass_info, nullptr, &renderpass) != VK_SUCCESS) {
		throw std::runtime_error("fail to create render pass");
	}
}

void BaseDemo::CreateFramebuffers() {
	std::vector<VkImageView> attachments(2);
	attachments[1] = depth_stencil.image_view;
	swapchain_framebuffers.resize(vulkan_swap_chain.image_count);
	for (uint32_t i = 0; i < swapchain_framebuffers.size(); i++) {
		attachments[0] = vulkan_swap_chain.image_views[i];
		vk::init::CreateFrameBuffer(logical_device, renderpass, attachments, vulkan_swap_chain.swap_extent.width, vulkan_swap_chain.swap_extent.height, &swapchain_framebuffers[i]);
	}
}

void BaseDemo::RecreateSwapChain() {
	// handle minimization
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}

	vkDeviceWaitIdle(logical_device);
	CleanupSwapChain();

	CreateSwapChain();
	CreateDepthStencil();
	CreateRenderpass();
	CreateFramebuffers();
	SetupImagesInflightFences();
	CreateNonPermanentResources();
}

void BaseDemo::CleanupSwapChain() {
	CleanupNonPermanentResources();
	images_inflight.clear();// clear everything away, when swapchain is recreated
	for (VkFramebuffer framebuffer : swapchain_framebuffers) {
		vkDestroyFramebuffer(logical_device, framebuffer, nullptr);
	}
	vkDestroyRenderPass(logical_device, renderpass, nullptr);
	depth_stencil.Destroy();
	vulkan_swap_chain.Destroy();
}

void BaseDemo::SetupImagesInflightFences() {
	images_inflight.resize(vulkan_swap_chain.image_count, VK_NULL_HANDLE);
}

// static methods
void BaseDemo::FramebufferResizeCallback(GLFWwindow* window, int width, int height) {
	auto app = reinterpret_cast<BaseDemo*>(glfwGetWindowUserPointer(window));
	app->framebuffer_resized = true;
}

