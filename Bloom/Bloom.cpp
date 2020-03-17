#include "BaseDemo.h"
#include <iostream>
#include "VulkanHelper.h"
#include "glm/glm.hpp"
#include <array>
#include "VulkanGraphicPipeline.h"
#include "VulkanCompositeBuffer.h"
#include "glm\gtx\transform.hpp"
#include "Light.h"
#include <chrono>


std::vector<cg::PointLight> point_lights = {
	{glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(5.0f, 5.0f, 5.0f), 15.0f, 0.19f, 0.032f},
	{glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(5.0f, 5.0f, 5.0f), 15.0f, 0.19f, 0.032f},
	{glm::vec3(0.0f, 4.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(5.0f, 5.0f, 5.0f), 15.0f, 0.19f, 0.032f},
	{glm::vec3(0.0f, 6.0f, 0.0f), glm::vec3(0.05f, 0.05f, 0.05f), glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(5.0f, 5.0f, 5.0f), 15.0f, 0.19f, 0.032f}
};

struct PerObject {
	alignas(16) glm::mat4 model_matrix;
	alignas(16) glm::vec3 color;
};

struct PerCamera {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct Vertex {
	alignas(16) glm::vec3 pos;
	alignas(16) glm::vec3 normal;

	static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions() {

		return {
			vk::init::CreateVertexInputBindingDescription(0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX)
		};
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
		return {
			//vertex data
			vk::init::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, pos)),
			vk::init::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, normal)),
		};
	}
};

struct QuadVertex {
	alignas(8) glm::vec2 pos;
	alignas(8) glm::vec2 texcoord;

	static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions() {
		return { vk::init::CreateVertexInputBindingDescription(0, sizeof(QuadVertex), VK_VERTEX_INPUT_RATE_VERTEX) };
	}

	static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions() {
		return {
			vk::init::CreateVertexInputAttributeDescription(0, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(QuadVertex, pos)),
			vk::init::CreateVertexInputAttributeDescription(0, 1, VK_FORMAT_R32G32_SFLOAT, offsetof(QuadVertex, texcoord))
		};
	}
};

const std::vector<Vertex> cube = {
	// front face
	{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
	{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
	{glm::vec3(1.0f,  1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
	{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f)},
	// back face
	{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
	{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
	{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
	{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, 0.0f, -1.0f)},
	//right face
	{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
	{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
	{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
	{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(1.0f, 0.0f, 0.0f)},
	// left face
	{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
	{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
	{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
	{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(-1.0f, 0.0f, 0.0f)},
	// top face
	{glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(-1.0f, 1.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	// down face
	{glm::vec3(-1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
	{glm::vec3(1.0f, -1.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
	{glm::vec3(1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)},
	{glm::vec3(-1.0f, -1.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)}
};

const std::vector<uint16_t> cube_indices = {
	//front face
	0, 1, 2,
	2, 3, 0,
	//back face
	4, 5, 6,
	6, 7, 4,
	//right face
	8, 9, 10,
	10, 11, 8,
	//left face
	12, 13, 14, 
	14, 15, 12,
	// top face
	16, 17, 18,
	18, 19, 16,
	// down face
	20, 21, 22,
	22, 23, 20
};

// (0, 0) is at bottom left
const std::vector<QuadVertex> quad = {
	{{-1.0f, 1.0f}, {0.0f, 1.0f}},
	{{1.0f, 1.0f}, {1.0f, 1.0f}},
	{{1.0f, -1.0f}, {1.0f, 0.0f}},
	{{-1.0f, -1.0f}, {0.0f, 0.0f}}
};

const std::vector<uint16_t> quad_indices = {
	0, 1, 2,
	2, 3, 0
};

const std::vector<PerObject> boxes_data = { // 10 matrices 
	{glm::translate(glm::vec3(0.0f, 0.0f, 0.0f)), glm::vec3(2.0f, 2.0f, 2.0f)}, //light source
	{glm::translate(glm::vec3(0.0f, 2.0f, 0.0f)), glm::vec3(2.0f, 2.0f, 2.0f)}, //light source
	{glm::translate(glm::vec3(0.0f, 4.0f, 0.0f)), glm::vec3(2.0f, 2.0f, 2.0f)}, // light source
	{glm::translate(glm::vec3(0.0f, 6.0f, 0.0f)), glm::vec3(2.0f, 2.0f, 2.0f)}, // light source
	{glm::translate(glm::vec3(6.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f)}, //red
	{glm::translate(glm::vec3(-6.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 0.0f)}, //yellow
	{glm::translate(glm::vec3(0.0, 0.0f, 5.0f)), glm::vec3(1.0f, 0.0f, 1.0f)}, //purple
	{glm::translate(glm::vec3(-6.0f, 0.0f, -5.0f)), glm::vec3(1.0f, 0.0f, 0.0f)}, //red
	{glm::translate(glm::vec3(2.0f, 3.0f, 8.0f)), glm::vec3(1.0f, 1.0f, 0.0f)}, // yellow
	{glm::translate(glm::vec3(0.0f, 6.0f, 4.0f)), glm::vec3(0.0f, 1.0f, 1.0f)} // cyan
};

struct UBOData {
	unsigned char * data = nullptr;
	uint32_t total_size;
	uint32_t stride;
};

class BloomDemo : public BaseDemo {
private:
	UBOData per_object_data;
	UBOData per_light_data;

	VkRenderPass firstpass_renderpass;
	VkRenderPass blur_renderpass;

	std::vector<vk::VulkanCompositeImage> firstpass_color_attachments;
	std::vector<vk::VulkanCompositeImage> light_color_attachments;
	vk::VulkanCompositeImage firstpass_depth_attachment;
	vk::VulkanCompositeImage light_depth_attachment;
	std::vector<vk::VulkanCompositeImage> vertical_blur_attachments;
	std::vector<vk::VulkanCompositeImage> horizontal_blur_attachments;

	std::vector<VkFramebuffer> firstpass_framebuffers;
	std::vector<VkFramebuffer> light_framebuffers;
	std::vector<VkFramebuffer> vertical_blur_framebuffers;
	std::vector<VkFramebuffer> horizontal_blur_framebuffers;

	VkPipelineLayout firstpass_pipeline_layout;
	VkPipelineLayout blur_pipeline_layout;
	VkPipelineLayout draw_pipeline_layout;

	vk::VulkanCompositeBuffer cube_vertex_buffer;
	vk::VulkanCompositeBuffer cube_index_buffer;
	vk::VulkanCompositeBuffer quad_vertex_buffer;
	vk::VulkanCompositeBuffer quad_index_buffer;

	VkDescriptorSetLayout firstpass_descriptor_set_layout;
	VkDescriptorSetLayout blur_descriptor_set_layout;
	VkDescriptorSetLayout draw_descriptor_set_layout;

	VkPipeline firstpass_pipeline;
	VkPipeline firstpass_light_pipeline;
	VkPipeline light_pipeline;
	VkPipeline light_firstpass_pipeline;
	VkPipeline horizontal_pipeline;
	VkPipeline vertical_pipeline;
	VkPipeline draw_pipeline;

	std::vector<vk::VulkanCompositeBuffer> light_uniform_buffers;
	std::vector<vk::VulkanCompositeBuffer> per_camera_uniform_buffers;
	std::vector<vk::VulkanCompositeBuffer> per_obj_uniform_buffers;

	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> vertical_blur_descriptor_sets;
	std::vector<VkDescriptorSet> horizontal_blur_descriptor_sets;
	std::vector<VkDescriptorSet> firstpass_descriptor_sets;
	std::vector<VkDescriptorSet> draw_descriptor_sets;

	std::vector<VkCommandBuffer> offscreen_draw_cmd_buffers;

	const static uint32_t offscreen_framebuffer_width = 256;
	const static uint32_t offscreen_framebuffer_height = 256;

	VkSampler sampler;

public:
	const char* GetWindowTitle() override {
		return "Bloom Demo";
	}

	uint32_t GetWindowInitWidth() override {
		return 800;
	}

	uint32_t GetWindowInitHeight() override {
		return 600;
	}

	bool ShowFPS() override {
		return true;
	}

	void CreatePermanentResources() override {
		CreateUboDataArrays();
		CreateVertexAndIndexBuffers();
		CreateDescriptorSetLayouts();
		CreateTextureSampler();
	}

	void CleanupPermanentResources() override {
		vkDestroySampler(this->logical_device, this->sampler, nullptr);
		CleanupDescriptorSetLayouts();
		CleanupVertexAndIndexBuffers();
		CleanupUboDataArrays();
	};

	void CreateNonPermanentResources() override {
		CreateAttachments();
		CreateRenderpasses();//maybe dont need to be in non-permanent cause it doesnt have anything to do with resizing or even attachment
		CreateFramebuffers();
		CreatePipelines();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateOffscreenDrawCmdBuffers();
		CreateDrawCmdBuffers();
	}

	void CleanupNonPermanentResources() override {
		vkFreeCommandBuffers(this->logical_device, this->command_pool, static_cast<uint32_t>(this->draw_cmd_buffers.size()), this->draw_cmd_buffers.data());
		vkFreeCommandBuffers(this->logical_device, this->command_pool, static_cast<uint32_t>(this->offscreen_draw_cmd_buffers.size()), this->offscreen_draw_cmd_buffers.data());
		vkDestroyDescriptorPool(this->logical_device, this->descriptor_pool, nullptr);
		CleanupUniformBuffers();
		CleanupPipelines();
		CleanupFramebuffers();
		CleanupRenderpasses();
		CleanupAttachments();
	}


	std::vector<vk::QueueCreationRequirement> GetQueueFamilyRequirements() override {
		vk::QueueCreationRequirement req0 = {};
		req0.types.is_graphic = true;
		req0.types.is_present = true;
		req0.types.is_transfer = true;
		req0.num_queue = 1;
		for (uint32_t i = 0; i < req0.num_queue; i++) {
			req0.priorities.push_back(1.0f);
		}
		return { req0 };
	}

	void CreateAttachments() {
		CreateFirstpassAndLightpassAttachments();
		CreateBlurAttachment();
	}

	void CleanupAttachments() {
		CleanupBlurAttachment();
		CleanupFirstpassAttachments();
	}

	void CreateFirstpassAndLightpassAttachments() {
		//create color attachments
		VkImageCreateInfo color_attachment_create_info = {};
		color_attachment_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		color_attachment_create_info.imageType = VK_IMAGE_TYPE_2D;
		color_attachment_create_info.format = VK_FORMAT_R32G32B32A32_SFLOAT; // hdr
		color_attachment_create_info.extent = { this->vulkan_swap_chain.swap_extent.width, this->vulkan_swap_chain.swap_extent.height, 1 };
		color_attachment_create_info.mipLevels = 1;
		color_attachment_create_info.arrayLayers = 1;
		color_attachment_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		color_attachment_create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		this->firstpass_color_attachments.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->firstpass_color_attachments.size(); i++) {
			this->firstpass_color_attachments[i].Create(this->physical_device, this->logical_device,
				color_attachment_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		}
		color_attachment_create_info.extent = { this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, 1 };
		this->light_color_attachments.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->light_color_attachments.size(); i++) {
			this->light_color_attachments[i].Create(this->physical_device, this->logical_device,
				color_attachment_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		}

		// create depth attachment
		VkImageCreateInfo depth_create_info = {};
		depth_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depth_create_info.imageType = VK_IMAGE_TYPE_2D;
		depth_create_info.format = vk::GetSupportedDepthFormat(this->physical_device);
		depth_create_info.extent = { this->vulkan_swap_chain.swap_extent.width, this->vulkan_swap_chain.swap_extent.height, 1 };
		depth_create_info.mipLevels = 1;
		depth_create_info.arrayLayers = 1;
		depth_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		depth_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		this->firstpass_depth_attachment.Create(this->physical_device, this->logical_device, depth_create_info,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
		depth_create_info.extent = { this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, 1 };
		this->light_depth_attachment.Create(this->physical_device, this->logical_device, depth_create_info,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void CleanupFirstpassAttachments() {
		this->light_depth_attachment.Destroy();
		this->firstpass_depth_attachment.Destroy();
		for (uint32_t i = 0; i < this->light_color_attachments.size(); i++) {
			this->light_color_attachments[i].Destroy();
		}
		for (uint32_t i = 0; i < this->firstpass_color_attachments.size(); i++) {
			this->firstpass_color_attachments[i].Destroy();
		}
	}

	void CreateBlurAttachment() {
		VkImageCreateInfo color_attachment_create_info = {};
		color_attachment_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		color_attachment_create_info.imageType = VK_IMAGE_TYPE_2D;
		color_attachment_create_info.format = VK_FORMAT_R32G32B32A32_SFLOAT; // hdr
		color_attachment_create_info.extent = { this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, 1 };
		color_attachment_create_info.mipLevels = 1;
		color_attachment_create_info.arrayLayers = 1;
		color_attachment_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		color_attachment_create_info.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;

		this->vertical_blur_attachments.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->vertical_blur_attachments.size(); i++) {
			this->vertical_blur_attachments[i].Create(this->physical_device, this->logical_device,
				color_attachment_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		}
		this->horizontal_blur_attachments.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->horizontal_blur_attachments.size(); i++) {
			this->horizontal_blur_attachments[i].Create(this->physical_device, this->logical_device,
				color_attachment_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	void CleanupBlurAttachment() {
		for (uint32_t i = 0; i < this->horizontal_blur_attachments.size(); i++) {
			this->horizontal_blur_attachments[i].Destroy();
		}
		for (uint32_t i = 0; i < this->vertical_blur_attachments.size(); i++) {
			this->vertical_blur_attachments[i].Destroy();
		}
	}

	void CreateRenderpasses() {
		CreateFirstpassRenderpass();
		CreateBlurRenderpass();
	}

	void CleanupRenderpasses() {
		vkDestroyRenderPass(this->logical_device, this->blur_renderpass, nullptr);
		vkDestroyRenderPass(this->logical_device, this->firstpass_renderpass, nullptr);
	}

	void CreateBlurRenderpass() {
		VkAttachmentDescription color_attachment = {};
		color_attachment.format = this->vertical_blur_attachments[0].format;
		color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::vector<VkAttachmentReference> color_refs = { {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} };

		VkSubpassDescription subpass_desc = {};
		subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_desc.colorAttachmentCount = static_cast<uint32_t>(color_refs.size());
		subpass_desc.pColorAttachments = color_refs.data();
		subpass_desc.pDepthStencilAttachment = nullptr;

		//makes sure that fragment_shader of after-commands wont start until we have finished writing to color attachment
		// flushes to make sure that the color_attachment's write is visible to the fragment_shader's read
		std::vector<VkSubpassDependency> dependencies = { {}, {} };
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		//makes sure that fragment_shader of after-commands wont start until we have finished writing to color attachment
		// flushes to make sure that the color_attachment's write is visible to the fragment_shader's read
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		VkRenderPassCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		create_info.attachmentCount = 1;
		create_info.pAttachments = &color_attachment;
		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass_desc;
		create_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
		create_info.pDependencies = dependencies.data();

		if (vkCreateRenderPass(this->logical_device, &create_info, nullptr, &blur_renderpass) != VK_SUCCESS) {
			throw std::runtime_error("fail to create offscreen renderpass");
		}
	}

	void CreateFirstpassRenderpass() {
		std::array<VkAttachmentDescription, 2> attachments = {};
		//color attachments
		attachments[0].format = this->firstpass_color_attachments[0].format;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// depth attachment
		attachments[1].format = this->firstpass_depth_attachment.format;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		std::vector<VkAttachmentReference> color_refs = { {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} };
		VkAttachmentReference depth_ref = { 1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpass_desc = {};
		subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_desc.colorAttachmentCount = static_cast<uint32_t>(color_refs.size());
		subpass_desc.pColorAttachments = color_refs.data();
		subpass_desc.pDepthStencilAttachment = &depth_ref;

		//makes sure that the content of the framebuffer is read before you start writing new values to it
		std::vector<VkSubpassDependency> dependencies = { {}, {} };
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].srcAccessMask = 0;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		//make sure that subsequent fragment shader stage of after-commands wont start until we have finished writing to the color attachment
		// flush to make sure that the color_attachment_output's write is visible to the fragment_shader's read. 
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		VkRenderPassCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
		create_info.pAttachments = attachments.data();
		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass_desc;
		create_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
		create_info.pDependencies = dependencies.data();

		if (vkCreateRenderPass(this->logical_device, &create_info, nullptr, &firstpass_renderpass) != VK_SUCCESS) {
			throw std::runtime_error("fail to create offscreen renderpass");
		}
	}

	void CreateDescriptorSetLayouts() {
		std::vector<VkDescriptorSetLayoutBinding> firstpass_layout_bindings = {
			vk::init::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(point_lights.size()), VK_SHADER_STAGE_FRAGMENT_BIT) //4 is number of lights
		};
		
		vk::init::CreateDescriptorSetLayout(this->logical_device, firstpass_layout_bindings, &this->firstpass_descriptor_set_layout);

		std::vector<VkDescriptorSetLayoutBinding> blur_layout_bindings = { 
			vk::init::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT) 
		};
		vk::init::CreateDescriptorSetLayout(this->logical_device, blur_layout_bindings, &this->blur_descriptor_set_layout);

		std::vector<VkDescriptorSetLayoutBinding> draw_layout_bindings = {
			vk::init::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
		};
		vk::init::CreateDescriptorSetLayout(this->logical_device, draw_layout_bindings, &this->draw_descriptor_set_layout);
	}

	void CleanupDescriptorSetLayouts() {
		vkDestroyDescriptorSetLayout(this->logical_device, this->draw_descriptor_set_layout, nullptr);
		vkDestroyDescriptorSetLayout(this->logical_device, this->blur_descriptor_set_layout, nullptr);
		vkDestroyDescriptorSetLayout(this->logical_device, this->firstpass_descriptor_set_layout, nullptr);
	}

	void CreatePipelines() {
		CreateFirstpassPipeline();
		CreateBlurPipelines();
		CreateDrawPipeline();
	}

	void CreateFirstpassPipeline() {
		//firstpass shader
		VkShaderModule firstpass_vert_shader_module = vk::CreateShaderModule(this->logical_device, "shaders/firstpass_vert.spv");
		VkShaderModule firstpass_frag_shader_module = vk::CreateShaderModule(this->logical_device, "shaders/firstpass_frag.spv");
		VkPipelineShaderStageCreateInfo firstpass_vert_shader_create_info = vk::CreateShaderStageCreateInfo(firstpass_vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
		VkPipelineShaderStageCreateInfo firstpass_frag_shader_create_info = vk::CreateShaderStageCreateInfo(firstpass_frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

		//light shader
		VkShaderModule light_vert_shader_module = vk::CreateShaderModule(this->logical_device, "shaders/light_vert.spv");
		VkShaderModule light_frag_shader_module = vk::CreateShaderModule(this->logical_device, "shaders/light_frag.spv");
		VkPipelineShaderStageCreateInfo light_vert_shader_create_info = vk::CreateShaderStageCreateInfo(light_vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
		VkPipelineShaderStageCreateInfo light_frag_shader_create_info = vk::CreateShaderStageCreateInfo(light_frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

		VkPipelineShaderStageCreateInfo shader_stages[] = { firstpass_vert_shader_create_info, firstpass_frag_shader_create_info };

		std::vector<VkVertexInputBindingDescription> input_binding_descs = Vertex::GetBindingDescriptions();
		std::vector<VkVertexInputAttributeDescription> input_attrib_descs = Vertex::GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertex_input_info = vk::CreateVertexInputStateCreateInfo(input_binding_descs, input_attrib_descs);
		VkPipelineInputAssemblyStateCreateInfo assembly_state_info = vk::CreateInputAssemblyStateCreateInfo(false, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)this->vulkan_swap_chain.swap_extent.width;
		viewport.height = (float)this->vulkan_swap_chain.swap_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		std::vector<VkViewport> viewports = { viewport };

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { this->vulkan_swap_chain.swap_extent.width, this->vulkan_swap_chain.swap_extent.height };
		std::vector<VkRect2D> scissors = { scissor };
		VkPipelineViewportStateCreateInfo viewport_state_info = vk::CreateViewportStateCreateInfo(viewports, scissors);

		VkPipelineRasterizationStateCreateInfo rasterizer = vk::CreateRasterizationStateCreateInfo(false, false, VK_POLYGON_MODE_FILL, 1.0f,
			VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false);

		VkPipelineMultisampleStateCreateInfo multisample = vk::CreateMultisampleStateCreateInfo(false, VK_SAMPLE_COUNT_1_BIT);

		std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states = { vk::CreateColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, false) };

		VkPipelineDepthStencilStateCreateInfo depth_stencil = vk::CreateDepthStencilStateCreateInfo(true, true, VK_COMPARE_OP_LESS, false, false);

		VkPipelineColorBlendStateCreateInfo color_blend_state = vk::CreateColorBlendStateCreateInfo(false, blend_attachment_states);

		std::vector<VkDynamicState> states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamic_state = vk::CreateDynamicStateCreateInfo(states);

		std::vector<VkPushConstantRange> constant_ranges;
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts = { this->firstpass_descriptor_set_layout };
		vk::CreatePipelineLayout(this->logical_device, descriptor_set_layouts, constant_ranges, &this->firstpass_pipeline_layout);

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &assembly_state_info;
		pipeline_info.pViewportState = &viewport_state_info;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisample;
		pipeline_info.pDepthStencilState = &depth_stencil; // optional
		pipeline_info.pColorBlendState = &color_blend_state;
		pipeline_info.pDynamicState = nullptr; // optional
		pipeline_info.layout = this->firstpass_pipeline_layout;
		pipeline_info.renderPass = this->firstpass_renderpass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = nullptr; // optional
		pipeline_info.basePipelineIndex = -1; // optional

		// first pass pipeline
		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->firstpass_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create firstpass pipeline");
		}
		//first pass light pipeline
		shader_stages[0] = light_vert_shader_create_info;
		shader_stages[1] = light_frag_shader_create_info;
		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->firstpass_light_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create firstpass pipeline");
		}

		//Create light pipeline
		viewports[0].width = (float)this->offscreen_framebuffer_width;
		viewports[0].height = (float)this->offscreen_framebuffer_height;
		scissors[0].extent.width = this->offscreen_framebuffer_width;
		scissors[0].extent.height = this->offscreen_framebuffer_height;

		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->light_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create light pipeline");
		}
		//ligth first pass pipeline
		shader_stages[0] = firstpass_vert_shader_create_info;
		shader_stages[1] = firstpass_frag_shader_create_info;
		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->light_firstpass_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create light pipeline");
		}

		vkDestroyShaderModule(this->logical_device, light_frag_shader_module, nullptr);
		vkDestroyShaderModule(this->logical_device, light_vert_shader_module, nullptr);

		vkDestroyShaderModule(this->logical_device, firstpass_frag_shader_module, nullptr);
		vkDestroyShaderModule(this->logical_device, firstpass_vert_shader_module, nullptr);
	}

	void CreateBlurPipelines() {
		VkShaderModule vert_shader_module = vk::CreateShaderModule(logical_device, "shaders/blur_vert.spv");
		VkShaderModule frag_shader_module = vk::CreateShaderModule(logical_device, "shaders/blur_frag.spv");

		VkPipelineShaderStageCreateInfo vert_shader_create_info = vk::CreateShaderStageCreateInfo(vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
		VkPipelineShaderStageCreateInfo frag_shader_create_info = vk::CreateShaderStageCreateInfo(frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_create_info, frag_shader_create_info };

		std::vector<VkVertexInputBindingDescription> input_binding_descs = QuadVertex::GetBindingDescriptions();
		std::vector<VkVertexInputAttributeDescription> input_attrib_descs = QuadVertex::GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertex_input_info = vk::CreateVertexInputStateCreateInfo(input_binding_descs, input_attrib_descs);
		VkPipelineInputAssemblyStateCreateInfo assembly_state_info = vk::CreateInputAssemblyStateCreateInfo(false, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		std::vector<VkViewport> viewports = { {} };
		viewports[0].x = 0.0f;
		viewports[0].y = 0.0f;
		viewports[0].width = (float)this->offscreen_framebuffer_width;
		viewports[0].height = (float)this->offscreen_framebuffer_height;
		viewports[0].minDepth = 0.0f;
		viewports[0].maxDepth = 1.0f;

		std::vector<VkRect2D> scissors = { {} };
		scissors[0].offset = { 0, 0 };
		scissors[0].extent = { this->offscreen_framebuffer_width, this->offscreen_framebuffer_height };
		VkPipelineViewportStateCreateInfo viewport_state_info = vk::CreateViewportStateCreateInfo(viewports, scissors);

		VkPipelineRasterizationStateCreateInfo rasterizer = vk::CreateRasterizationStateCreateInfo(false, false, VK_POLYGON_MODE_FILL, 1.0f,
			VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false);

		VkPipelineMultisampleStateCreateInfo multisample = vk::CreateMultisampleStateCreateInfo(false, VK_SAMPLE_COUNT_1_BIT);

		VkPipelineColorBlendAttachmentState blend_attachment_state = vk::CreateColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, false);

		std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states = { blend_attachment_state };

		VkPipelineDepthStencilStateCreateInfo depth_stencil = vk::CreateDepthStencilStateCreateInfo(false, false, VK_COMPARE_OP_LESS, false, false);// no depth test necessary

		VkPipelineColorBlendStateCreateInfo color_blend_state = vk::CreateColorBlendStateCreateInfo(false, blend_attachment_states);

		std::vector<VkDynamicState> states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamic_state = vk::CreateDynamicStateCreateInfo(states);

		std::vector<VkPushConstantRange> constant_ranges;
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts = { this->blur_descriptor_set_layout };
		vk::CreatePipelineLayout(this->logical_device, descriptor_set_layouts, constant_ranges, &this->blur_pipeline_layout);

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &assembly_state_info;
		pipeline_info.pViewportState = &viewport_state_info;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisample;
		pipeline_info.pDepthStencilState = &depth_stencil; // optional
		pipeline_info.pColorBlendState = &color_blend_state;
		pipeline_info.pDynamicState = nullptr; // optional
		pipeline_info.layout = this->blur_pipeline_layout;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = nullptr; // optional
		pipeline_info.basePipelineIndex = -1; // optional
		pipeline_info.renderPass = this->blur_renderpass;

		uint32_t blur_dir = 0; // 0 is vertical 1 is horizontal. We are creating vertical pipeline here

		std::vector<VkSpecializationMapEntry> specialization_map_entries = { vk::init::CreateSpecializationMapEntry(0, 0, sizeof(uint32_t)) };
		VkSpecializationInfo specialization_info = vk::init::CreateSpecializationInfo(specialization_map_entries, sizeof(blur_dir), &blur_dir);
		
		shader_stages[1].pSpecializationInfo = &specialization_info;

		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->vertical_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create vertical graphic pipeline");
		}

		blur_dir = 1;
		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->horizontal_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create horizontal graphic pipeline");
		}

		vkDestroyShaderModule(this->logical_device, frag_shader_module, nullptr);
		vkDestroyShaderModule(this->logical_device, vert_shader_module, nullptr);
	}

	void CreateDrawPipeline() {
		VkShaderModule vert_shader_module = vk::CreateShaderModule(logical_device, "shaders/final_vert.spv");
		VkShaderModule frag_shader_module = vk::CreateShaderModule(logical_device, "shaders/final_frag.spv");

		VkPipelineShaderStageCreateInfo vert_shader_create_info = vk::CreateShaderStageCreateInfo(vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
		VkPipelineShaderStageCreateInfo frag_shader_create_info = vk::CreateShaderStageCreateInfo(frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_create_info, frag_shader_create_info };

		std::vector<VkVertexInputBindingDescription> input_binding_descs = QuadVertex::GetBindingDescriptions();
		std::vector<VkVertexInputAttributeDescription> input_attrib_descs = QuadVertex::GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertex_input_info = vk::CreateVertexInputStateCreateInfo(input_binding_descs, input_attrib_descs);
		VkPipelineInputAssemblyStateCreateInfo assembly_state_info = vk::CreateInputAssemblyStateCreateInfo(false, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)this->vulkan_swap_chain.swap_extent.width;
		viewport.height = (float)this->vulkan_swap_chain.swap_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		std::vector<VkViewport> viewports = { viewport };

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { this->vulkan_swap_chain.swap_extent.width, this->vulkan_swap_chain.swap_extent.height };
		std::vector<VkRect2D> scissors = { scissor };
		VkPipelineViewportStateCreateInfo viewport_state_info = vk::CreateViewportStateCreateInfo(viewports, scissors);

		VkPipelineRasterizationStateCreateInfo rasterizer = vk::CreateRasterizationStateCreateInfo(false, false, VK_POLYGON_MODE_FILL, 1.0f,
			VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false);

		VkPipelineMultisampleStateCreateInfo multisample = vk::CreateMultisampleStateCreateInfo(false, VK_SAMPLE_COUNT_1_BIT);

		VkPipelineColorBlendAttachmentState blend_attachment_state = vk::CreateColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, false);

		std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states = { blend_attachment_state };

		VkPipelineDepthStencilStateCreateInfo depth_stencil = vk::CreateDepthStencilStateCreateInfo(false, false, VK_COMPARE_OP_LESS, false, false);// no depth test necessary

		VkPipelineColorBlendStateCreateInfo color_blend_state = vk::CreateColorBlendStateCreateInfo(false, blend_attachment_states);

		std::vector<VkPushConstantRange> constant_ranges;
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts = { this->draw_descriptor_set_layout };
		vk::CreatePipelineLayout(this->logical_device, descriptor_set_layouts, constant_ranges, &this->draw_pipeline_layout);

		VkGraphicsPipelineCreateInfo pipeline_info = {};
		pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeline_info.stageCount = 2;
		pipeline_info.pStages = shader_stages;
		pipeline_info.pVertexInputState = &vertex_input_info;
		pipeline_info.pInputAssemblyState = &assembly_state_info;
		pipeline_info.pViewportState = &viewport_state_info;
		pipeline_info.pRasterizationState = &rasterizer;
		pipeline_info.pMultisampleState = &multisample;
		pipeline_info.pDepthStencilState = &depth_stencil; // optional
		pipeline_info.pColorBlendState = &color_blend_state;
		pipeline_info.pDynamicState = nullptr; // optional
		pipeline_info.layout = this->draw_pipeline_layout; // uses same pipeline layout as blur
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = nullptr; // optional
		pipeline_info.basePipelineIndex = -1; // optional

		pipeline_info.renderPass = this->renderpass;

		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->draw_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create draw graphic pipeline");
		}

		vkDestroyShaderModule(this->logical_device, frag_shader_module, nullptr);
		vkDestroyShaderModule(this->logical_device, vert_shader_module, nullptr);
	}

	void CleanupPipelines() {
		vkDestroyPipeline(this->logical_device, this->draw_pipeline, nullptr);
		vkDestroyPipelineLayout(this->logical_device, this->draw_pipeline_layout, nullptr);
		vkDestroyPipeline(this->logical_device, this->horizontal_pipeline, nullptr);
		vkDestroyPipeline(this->logical_device, this->vertical_pipeline, nullptr);
		vkDestroyPipelineLayout(this->logical_device, this->blur_pipeline_layout, nullptr);
		vkDestroyPipeline(this->logical_device, this->light_firstpass_pipeline, nullptr);
		vkDestroyPipeline(this->logical_device, this->light_pipeline, nullptr);
		vkDestroyPipeline(this->logical_device, this->firstpass_light_pipeline, nullptr);
		vkDestroyPipeline(this->logical_device, this->firstpass_pipeline, nullptr);
		vkDestroyPipelineLayout(this->logical_device, this->firstpass_pipeline_layout, nullptr);
	}

	void CreateFramebuffers() {
		CreateFirstpassAndLightFramebuffers();
		CreateBlurFramebuffers();
	}

	void CleanupFramebuffers() {
		//cleanup blur frame buffers
		for (VkFramebuffer blur_framebuffer : this->horizontal_blur_framebuffers) {
			vkDestroyFramebuffer(this->logical_device, blur_framebuffer, nullptr);
		}
		for (VkFramebuffer blur_framebuffer : this->vertical_blur_framebuffers) {
			vkDestroyFramebuffer(this->logical_device, blur_framebuffer, nullptr);
		}
		// cleanup light framebuffers
		for (VkFramebuffer light_framebuffer : this->light_framebuffers) {
			vkDestroyFramebuffer(this->logical_device, light_framebuffer, nullptr);
		}
		// cleanup firstpass frame buffers
		for (VkFramebuffer firstpass_framebuffer : this->firstpass_framebuffers) {
			vkDestroyFramebuffer(this->logical_device, firstpass_framebuffer, nullptr);
		}
	}

	void CreateFirstpassAndLightFramebuffers() {
		// create first pass framebuffers
		this->firstpass_framebuffers.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkImageView> attachments(2);
		attachments[1] = this->firstpass_depth_attachment.image_view;
		for (uint32_t i = 0; i < this->firstpass_framebuffers.size(); i++) {
			attachments[0] = this->firstpass_color_attachments[i].image_view;
			vk::init::CreateFrameBuffer(this->logical_device, this->firstpass_renderpass, attachments,
				this->vulkan_swap_chain.swap_extent.width, this->vulkan_swap_chain.swap_extent.height, &this->firstpass_framebuffers[i]);
		}
		// create light framebuffers
		this->light_framebuffers.resize(this->vulkan_swap_chain.image_count);
		attachments[1] = this->light_depth_attachment.image_view;
		for (uint32_t i = 0; i < this->light_framebuffers.size(); i++) {
			attachments[0] = this->light_color_attachments[i].image_view;
			vk::init::CreateFrameBuffer(this->logical_device, this->firstpass_renderpass, attachments,
				this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, &this->light_framebuffers[i]);
		}
	}

	void CreateBlurFramebuffers() {
		this->vertical_blur_framebuffers.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkImageView> attachments(1);
		for (uint32_t i = 0; i < this->vertical_blur_framebuffers.size(); i++) {
			attachments[0] = this->vertical_blur_attachments[i].image_view;
			vk::init::CreateFrameBuffer(this->logical_device, this->blur_renderpass, attachments,
				this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, &this->vertical_blur_framebuffers[i]);
		}
		this->horizontal_blur_framebuffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->horizontal_blur_framebuffers.size(); i++) {
			attachments[0] = this->horizontal_blur_attachments[i].image_view;
			vk::init::CreateFrameBuffer(this->logical_device, this->blur_renderpass, attachments,
				this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, &this->horizontal_blur_framebuffers[i]);
		}
	}

	void CreateVertexAndIndexBuffers() {
		VkDeviceSize cube_vertex_buffer_size = sizeof(Vertex) * cube.size();
		VkDeviceSize cube_index_buffer_size = sizeof(cube_indices[0]) * cube_indices.size();
		VkDeviceSize quad_vertex_buffer_size = sizeof(QuadVertex) * quad.size();
		VkDeviceSize quad_index_buffer_size = sizeof(quad_indices[0]) * quad_indices.size();
		//create temporary staging buffers then copy from host
		vk::VulkanCompositeBuffer cube_staging_vertex_buffer;
		cube_staging_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		cube_staging_vertex_buffer.CopyFromHostData(const_cast<Vertex*>(cube.data()), cube_vertex_buffer_size, 0);

		vk::VulkanCompositeBuffer quad_staging_vertex_buffer;
		quad_staging_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, quad_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		quad_staging_vertex_buffer.CopyFromHostData(const_cast<QuadVertex*>(quad.data()), quad_vertex_buffer_size, 0);

		vk::VulkanCompositeBuffer cube_staging_index_buffer;
		cube_staging_index_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		cube_staging_index_buffer.CopyFromHostData(const_cast<uint16_t*>(cube_indices.data()), cube_index_buffer_size, 0);

		vk::VulkanCompositeBuffer quad_staging_index_buffer;
		quad_staging_index_buffer.CreateBuffer(this->logical_device, this->physical_device, quad_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		quad_staging_index_buffer.CopyFromHostData(const_cast<uint16_t*>(quad_indices.data()), quad_index_buffer_size, 0);

		// create vertex buffers
		this->cube_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		this->quad_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, quad_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		// create index buffer
		this->cube_index_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		this->quad_index_buffer.CreateBuffer(this->logical_device, this->physical_device, quad_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// create fences
		VkFence transfer_finished_fences[4];
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[0]); // create fence in unsignaled state
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[1]); // create fence in unsignaled state
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[2]); // create fence in unsignaled state
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[3]); // create fence in unsignaled state
		//transfer
		this->cube_vertex_buffer.TransferFromAnotherBuffer(cube_staging_vertex_buffer, this->command_pool, 0, 0, cube_vertex_buffer_size, this->queues[0], transfer_finished_fences[0]);
		this->quad_vertex_buffer.TransferFromAnotherBuffer(quad_staging_vertex_buffer, this->command_pool, 0, 0, quad_vertex_buffer_size, this->queues[0], transfer_finished_fences[1]);
		this->cube_index_buffer.TransferFromAnotherBuffer(cube_staging_index_buffer, this->command_pool, 0, 0, cube_index_buffer_size, this->queues[0], transfer_finished_fences[2]);
		this->quad_index_buffer.TransferFromAnotherBuffer(quad_staging_index_buffer, this->command_pool, 0, 0, quad_index_buffer_size, this->queues[0], transfer_finished_fences[3]);
		// wait for transfer to finish
		vkWaitForFences(this->logical_device, 4, transfer_finished_fences, VK_TRUE, UINT64_MAX);

		//cleanup fences and staging buffers
		vkDestroyFence(this->logical_device, transfer_finished_fences[0], nullptr);
		vkDestroyFence(this->logical_device, transfer_finished_fences[1], nullptr);
		vkDestroyFence(this->logical_device, transfer_finished_fences[2], nullptr);
		vkDestroyFence(this->logical_device, transfer_finished_fences[3], nullptr);

		quad_staging_index_buffer.DestroyBuffer();
		cube_staging_index_buffer.DestroyBuffer();

		quad_staging_vertex_buffer.DestroyBuffer();
		cube_staging_vertex_buffer.DestroyBuffer();
	}

	void CleanupVertexAndIndexBuffers() {
		this->quad_index_buffer.DestroyBuffer();
		this->cube_index_buffer.DestroyBuffer();
		this->quad_vertex_buffer.DestroyBuffer();
		this->cube_vertex_buffer.DestroyBuffer();
	}

	void CreateUniformBuffers() {
		
		VkDeviceSize light_uniform_buffer_size = this->per_light_data.total_size;
		this->light_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->light_uniform_buffers.size(); i++) {
			this->light_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, light_uniform_buffer_size,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		VkDeviceSize mvp_uniform_buffer_size = sizeof(PerCamera);
		this->per_camera_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->per_camera_uniform_buffers.size(); i++) {
			this->per_camera_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, mvp_uniform_buffer_size, 
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		VkDeviceSize model_uniform_buffer_size = this->per_object_data.total_size;
		this->per_obj_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->per_obj_uniform_buffers.size(); i++) {
			this->per_obj_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, model_uniform_buffer_size,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			// in real world, dont do host coherent, but do a manual flush of the updated buffer memory part for better performance. ie:  vkFlushMappedMemoryRanges.
		}

	}

	void CleanupUniformBuffers() {
		for (uint32_t i = 0; i < this->per_obj_uniform_buffers.size(); i++) {
			this->per_obj_uniform_buffers[i].DestroyBuffer();
		}
		for (uint32_t i = 0; i < this->per_camera_uniform_buffers.size(); i++) {
			this->per_camera_uniform_buffers[i].DestroyBuffer();
		}
		for (uint32_t i = 0; i < this->light_uniform_buffers.size(); i++) {
			this->light_uniform_buffers[i].DestroyBuffer();
		}
	}

	void CreateDescriptorPool() {
		// 5 uniform buffer descriptor (light * 4 and per camera) 
		// 4 combined image sampler (for vertical blur, horizontal blur and 2 for screen render)  
		// 1 uniform_buffer_dynamic (for per object)
		std::vector<VkDescriptorPoolSize> poolsizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , this->vulkan_swap_chain.image_count * 5},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, this->vulkan_swap_chain.image_count * 4},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, this->vulkan_swap_chain.image_count}
		};
		vk::init::CreateDescriptorPool(this->logical_device, poolsizes, this->vulkan_swap_chain.image_count * 10, &this->descriptor_pool);
	}

	void CreateDescriptorSets() {
		CreateFirstpassDescriptorSets();
		CreateBlurDescriptorSets();
		CreateDrawDescriptorSets();
	}

	void CreateFirstpassDescriptorSets() {
		this->firstpass_descriptor_sets.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkDescriptorSetLayout> layouts(this->vulkan_swap_chain.image_count, this->firstpass_descriptor_set_layout);
		vk::init::AllocateDescriptorSets(this->logical_device, this->descriptor_pool, layouts, this->firstpass_descriptor_sets);
		for (uint32_t i = 0; i < this->firstpass_descriptor_sets.size(); i++) {

			std::vector<VkWriteDescriptorSet> descriptor_writes;

			VkDescriptorBufferInfo binding0_info = vk::init::CreateDescriptorBufferInfo(this->per_camera_uniform_buffers[i].buffer, 0, sizeof(PerCamera));
			descriptor_writes.push_back(vk::init::CreateWriteDescriptorSet(this->firstpass_descriptor_sets[i], 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &binding0_info, nullptr));

			VkDescriptorBufferInfo binding1_info = vk::init::CreateDescriptorBufferInfo(this->per_obj_uniform_buffers[i].buffer, 0, sizeof(PerObject)); //https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDescriptorBufferInfo.html
			descriptor_writes.push_back(vk::init::CreateWriteDescriptorSet(this->firstpass_descriptor_sets[i], 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &binding1_info, nullptr));

			std::vector<VkDescriptorBufferInfo> arr(point_lights.size());
			for (uint32_t j = 0; j < point_lights.size(); j++) {
				arr[j] = vk::init::CreateDescriptorBufferInfo(this->light_uniform_buffers[i].buffer, static_cast<uint64_t>(this->per_light_data.stride) * j, sizeof(cg::PointLight));
				descriptor_writes.push_back(vk::init::CreateWriteDescriptorSet(this->firstpass_descriptor_sets[i], 2, j, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &arr[j], nullptr));
			}
			vkUpdateDescriptorSets(this->logical_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
			
		}
	}

	void CreateBlurDescriptorSets() {
		this->vertical_blur_descriptor_sets.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkDescriptorSetLayout> layouts(this->vulkan_swap_chain.image_count, this->blur_descriptor_set_layout);
		vk::init::AllocateDescriptorSets(this->logical_device, this->descriptor_pool, layouts, this->vertical_blur_descriptor_sets);
		for (uint32_t i = 0; i < vertical_blur_descriptor_sets.size(); i++) {
			VkDescriptorImageInfo image_info = vk::init::CreateDescriptorImageInfo(sampler, this->light_color_attachments[i].image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			VkWriteDescriptorSet descriptor_write = vk::init::CreateWriteDescriptorSet(this->vertical_blur_descriptor_sets[i], 0, 0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, nullptr, &image_info);
			vkUpdateDescriptorSets(this->logical_device, 1, &descriptor_write, 0, nullptr);
		}

		this->horizontal_blur_descriptor_sets.resize(this->vulkan_swap_chain.image_count);
		vk::init::AllocateDescriptorSets(this->logical_device, this->descriptor_pool, layouts, this->horizontal_blur_descriptor_sets);
		for (uint32_t i = 0; i < horizontal_blur_descriptor_sets.size(); i++) {
			VkDescriptorImageInfo image_info = vk::init::CreateDescriptorImageInfo(sampler, this->vertical_blur_attachments[i].image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			VkWriteDescriptorSet descriptor_write = vk::init::CreateWriteDescriptorSet(this->horizontal_blur_descriptor_sets[i], 0, 0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, nullptr, &image_info);
			vkUpdateDescriptorSets(this->logical_device, 1, &descriptor_write, 0, nullptr);
		}
	}

	void CreateDrawDescriptorSets() {
		this->draw_descriptor_sets.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkDescriptorSetLayout> layouts(this->vulkan_swap_chain.image_count, this->draw_descriptor_set_layout); 
		vk::init::AllocateDescriptorSets(this->logical_device, this->descriptor_pool, layouts, this->draw_descriptor_sets);

		std::array<VkWriteDescriptorSet, 2> descriptor_writes = {};
		for (uint32_t i = 0; i < draw_descriptor_sets.size(); i++) {
			VkDescriptorImageInfo image_info0 = vk::init::CreateDescriptorImageInfo(sampler, this->horizontal_blur_attachments[i].image_view, 
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			descriptor_writes[0] = vk::init::CreateWriteDescriptorSet(this->draw_descriptor_sets[i], 0, 0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, nullptr, &image_info0);

			VkDescriptorImageInfo image_info1 = vk::init::CreateDescriptorImageInfo(sampler, this->firstpass_color_attachments[i].image_view,
				VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
			descriptor_writes[1] = vk::init::CreateWriteDescriptorSet(this->draw_descriptor_sets[i], 1, 0,
				VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, nullptr, &image_info1);
			vkUpdateDescriptorSets(this->logical_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
		}
	}

	void CreateOffscreenDrawCmdBuffers() {
		this->offscreen_draw_cmd_buffers.resize(this->vulkan_swap_chain.image_count);
		vk::init::CreateCmdBuffer(this->logical_device, this->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			this->vulkan_swap_chain.image_count, this->offscreen_draw_cmd_buffers.data());
		VkDeviceSize vertex_offsets[] = { 0 };

		std::vector<VkClearValue> clear_values = { {}, {} };
		clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clear_values[1].depthStencil = { 1.0f, 0 };

		std::vector<VkClearValue> blur_clear_values = { {} };
		blur_clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };

		for (uint32_t i = 0; i < this->offscreen_draw_cmd_buffers.size(); i++) {
			vk::util::BeginCmdBuffer(this->offscreen_draw_cmd_buffers[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr);
			
			// firstpass render pass
			vk::util::BeginRenderpass(this->offscreen_draw_cmd_buffers[i], this->firstpass_renderpass, this->firstpass_framebuffers[i], { 0,0 },
				{ this->vulkan_swap_chain.swap_extent.width, this->vulkan_swap_chain.swap_extent.height }, clear_values, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindVertexBuffers(this->offscreen_draw_cmd_buffers[i], 0, 1, &this->cube_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->offscreen_draw_cmd_buffers[i], this->cube_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindPipeline(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->firstpass_light_pipeline);
			for (uint32_t j = 0; j < point_lights.size(); j++) { //draw light
				uint32_t dynamic_alignment = j * this->per_object_data.stride;
				vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->firstpass_pipeline_layout, 0, 1, &this->firstpass_descriptor_sets[i], 1, &dynamic_alignment);
				vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
			}

			vkCmdBindPipeline(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->firstpass_pipeline);
			for (uint32_t j = point_lights.size(); j < boxes_data.size(); j++) { //draw boxes
				uint32_t dynamic_alignment = j * this->per_object_data.stride;
				vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->firstpass_pipeline_layout, 0, 1, &this->firstpass_descriptor_sets[i], 1, &dynamic_alignment);
				vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
			}

			vkCmdEndRenderPass(this->offscreen_draw_cmd_buffers[i]);

			//light renderpass

			vk::util::BeginRenderpass(this->offscreen_draw_cmd_buffers[i], this->firstpass_renderpass, this->light_framebuffers[i], { 0,0 },
				{ this->offscreen_framebuffer_width, this->offscreen_framebuffer_height }, clear_values, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindVertexBuffers(this->offscreen_draw_cmd_buffers[i], 0, 1, &this->cube_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->offscreen_draw_cmd_buffers[i], this->cube_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			vkCmdBindPipeline(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->light_pipeline);
			for (uint32_t j = 0; j < point_lights.size(); j++) { //draw light
				uint32_t dynamic_alignment = j * this->per_object_data.stride;
				vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->firstpass_pipeline_layout, 0, 1, &this->firstpass_descriptor_sets[i], 1, &dynamic_alignment);
				vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
			}

			vkCmdBindPipeline(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->light_firstpass_pipeline);
			for (uint32_t j = point_lights.size(); j < boxes_data.size(); j++) { //draw boxes
				uint32_t dynamic_alignment = j * this->per_object_data.stride;
				vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->firstpass_pipeline_layout, 0, 1, &this->firstpass_descriptor_sets[i], 1, &dynamic_alignment);
				vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
			}

			vkCmdEndRenderPass(this->offscreen_draw_cmd_buffers[i]);

			//vertical blur render pass
			vk::util::BeginRenderpass(this->offscreen_draw_cmd_buffers[i], this->blur_renderpass, this->vertical_blur_framebuffers[i], { 0,0 },
				{ this->offscreen_framebuffer_width, this->offscreen_framebuffer_height }, blur_clear_values, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->vertical_pipeline);
			vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
				this->blur_pipeline_layout, 0, 1, &this->vertical_blur_descriptor_sets[i], 0, nullptr);
			vkCmdBindVertexBuffers(this->offscreen_draw_cmd_buffers[i], 0, 1, &this->quad_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->offscreen_draw_cmd_buffers[i], this->quad_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(quad_indices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(this->offscreen_draw_cmd_buffers[i]);

			// horizontal blur render pass
			vk::util::BeginRenderpass(this->offscreen_draw_cmd_buffers[i], this->blur_renderpass, this->horizontal_blur_framebuffers[i], { 0,0 },
				{ this->offscreen_framebuffer_width, this->offscreen_framebuffer_height }, blur_clear_values, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->horizontal_pipeline);
			vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
				this->blur_pipeline_layout, 0, 1, &this->horizontal_blur_descriptor_sets[i], 0, nullptr);
			vkCmdBindVertexBuffers(this->offscreen_draw_cmd_buffers[i], 0, 1, &this->quad_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->offscreen_draw_cmd_buffers[i], this->quad_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(quad_indices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(this->offscreen_draw_cmd_buffers[i]);
			if (vkEndCommandBuffer(this->offscreen_draw_cmd_buffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("fail to end command buffer recording");
			}
		}
	}

	void CreateDrawCmdBuffers() {
		std::vector<VkClearValue> clear_values = { {}, {} };
		clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
		clear_values[1].depthStencil = { 1.0f, 0 };

		VkDeviceSize vertex_offsets[] = { 0 };

		this->draw_cmd_buffers.resize(this->vulkan_swap_chain.image_count);
		vk::init::CreateCmdBuffer(this->logical_device, this->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, this->vulkan_swap_chain.image_count, this->draw_cmd_buffers.data());
		for (uint32_t i = 0; i < this->draw_cmd_buffers.size(); i++) {
			vk::util::BeginCmdBuffer(this->draw_cmd_buffers[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr);

			vk::util::BeginRenderpass(this->draw_cmd_buffers[i], this->renderpass,
				this->swapchain_framebuffers[i], { 0,0 }, this->vulkan_swap_chain.swap_extent, clear_values, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(this->draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->draw_pipeline);
			vkCmdBindDescriptorSets(this->draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
				this->draw_pipeline_layout, 0, 1, &this->draw_descriptor_sets[i], 0, nullptr);
			vkCmdBindVertexBuffers(this->draw_cmd_buffers[i], 0, 1, &this->quad_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->draw_cmd_buffers[i], this->quad_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			vkCmdDrawIndexed(this->draw_cmd_buffers[i], static_cast<uint32_t>(quad_indices.size()), 1, 0, 0, 0);

			vkCmdEndRenderPass(this->draw_cmd_buffers[i]);

			if (vkEndCommandBuffer(this->draw_cmd_buffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("fail to end command buffer recording");
			}
		}
	}

	void Draw() override {
		// wait for a previous iteration of the current frame to complete
		vkWaitForFences(this->logical_device, 1, &this->cmdbuffers_inflight[this->current_frame], VK_TRUE, UINT64_MAX);

		uint32_t image_index;
		VkResult result = vkAcquireNextImageKHR(this->logical_device, this->vulkan_swap_chain.swap_chain, UINT64_MAX,
			this->image_available_semaphores[this->current_frame], VK_NULL_HANDLE, &image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			RecreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("fail to acquire swap chain image");
		}
		UpdateUniformBufferData(image_index);

		// Check if a previous frame is using this image (i.e. there is its fence to wait on)
		if (this->images_inflight[image_index] != VK_NULL_HANDLE) {
			vkWaitForFences(this->logical_device, 1, &this->images_inflight[image_index], VK_TRUE, UINT64_MAX);
		}
		this->images_inflight[image_index] = this->cmdbuffers_inflight[this->current_frame]; // set the image as being used by the current frame
		vkResetFences(this->logical_device, 1, &this->cmdbuffers_inflight[current_frame]); // reset fence back to unsignal states so that later frames will have to wait

		// submit offscreen cmd buffers
		std::vector<VkSemaphore> offscreen_wait_semaphores;
		std::vector<VkPipelineStageFlags> offscreen_wait_stages;
		std::vector<VkSemaphore> offscreen_signal_semaphores;
		this->queues[0].SubmitSingleCmdBuffer(offscreen_wait_semaphores, offscreen_wait_stages,
			this->offscreen_draw_cmd_buffers[image_index], offscreen_signal_semaphores, VK_NULL_HANDLE);

		//submit draw cmd buffers
		std::vector<VkSemaphore> wait_semaphores = { this->image_available_semaphores[this->current_frame] };
		std::vector<VkPipelineStageFlags> wait_stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		std::vector<VkSemaphore> signal_semaphores = { this->render_finished_semaphores[this->current_frame] };
		this->queues[0].SubmitSingleCmdBuffer(wait_semaphores, wait_stages, this->draw_cmd_buffers[image_index],
			signal_semaphores, this->cmdbuffers_inflight[this->current_frame]);
		result = this->queues[0].PresentImage(signal_semaphores, this->vulkan_swap_chain.swap_chain, image_index);
		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || this->framebuffer_resized) {
			this->framebuffer_resized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("fail to present swap chain image");
		}
		this->current_frame = (this->current_frame + 1) % MAX_FRAMES_INFLIGHT;
	}

	void UpdateUniformBufferData(uint32_t current_image) {
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
		PerCamera mvp;
		mvp.view = glm::lookAt(glm::vec3(0.0, 20.0f, 16.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.proj = glm::perspective(glm::radians(45.0f), this->vulkan_swap_chain.swap_extent.width / (float)this->vulkan_swap_chain.swap_extent.height, 0.1f, 1000.0f);
		mvp.proj[1][1] *= -1;

		glm::mat4 rotating = glm::rotate(glm::mat4(1.0f), elapsed * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		unsigned char* light_ptr = this->per_light_data.data;
		for (uint32_t i = 0; i < point_lights.size(); i++) {
			//update light data here, only uses in firstpass.frag to shade
			cg::PointLight light = point_lights[i];
			light.position = glm::vec3(mvp.view * glm::vec4(point_lights[i].position, 1.0f)); // need light position in eyeCoord 
			*(reinterpret_cast<cg::PointLight*>(light_ptr)) = light;
			light_ptr += this->per_light_data.stride;
		}
		cg::PointLight* test_ptr = reinterpret_cast<cg::PointLight*>(this->per_light_data.data);
		// uses to draw both light sources and cubes
		unsigned char* obj_ptr = this->per_object_data.data;
		for (uint32_t i = 0; i < boxes_data.size(); i++) {
			PerObject per_obj = boxes_data[i];
			per_obj.model_matrix = per_obj.model_matrix * rotating;
			*reinterpret_cast<PerObject*>(obj_ptr) = per_obj;
			obj_ptr += this->per_object_data.stride;
		}
		this->light_uniform_buffers[current_image].CopyFromHostData(this->per_light_data.data, this->per_light_data.total_size, 0);
		this->per_camera_uniform_buffers[current_image].CopyFromHostData(&mvp, sizeof(PerCamera), 0);
		this->per_obj_uniform_buffers[current_image].CopyFromHostData(this->per_object_data.data, this->per_object_data.total_size, 0);
	}

	void CreateTextureSampler() {
		VkSamplerCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		create_info.magFilter = VK_FILTER_LINEAR;
		create_info.minFilter = VK_FILTER_LINEAR;
		create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		create_info.addressModeV = create_info.addressModeU;
		create_info.addressModeW = create_info.addressModeU;
		create_info.mipLodBias = 0.0f;
		create_info.maxAnisotropy = 1.0f;
		create_info.minLod = 0.0f;
		create_info.maxLod = 1.0f;
		create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		if (vkCreateSampler(this->logical_device, &create_info, nullptr, &this->sampler) != VK_SUCCESS) {
			throw std::runtime_error("fail to create sampler");
		}
	}

	void CreateUboDataArrays() {
		uint32_t min_ubuffer_alignment = vk::GetMinUniformBufferAlignment(this->physical_device);
		this->per_object_data.stride = vk::util::CalculateObjectSize(sizeof(PerObject), min_ubuffer_alignment);
		this->per_object_data.total_size = this->per_object_data.stride * boxes_data.size();
		this->per_object_data.data = reinterpret_cast<unsigned char*>(operator new(this->per_object_data.total_size));

		this->per_light_data.stride = vk::util::CalculateObjectSize(sizeof(cg::PointLight), min_ubuffer_alignment);
		this->per_light_data.total_size = this->per_light_data.stride * point_lights.size();
		this->per_light_data.data = reinterpret_cast<unsigned char*>(operator new(this->per_light_data.total_size));
	}

	void CleanupUboDataArrays() {
		delete[] this->per_light_data.data;
		this->per_light_data.total_size = 0;
		this->per_light_data.stride = 0;

		delete[] this->per_object_data.data;
		this->per_object_data.total_size = 0;
		this->per_object_data.stride = 0;
	}
};

MAIN_METHOD(BloomDemo)