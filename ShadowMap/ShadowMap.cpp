#include "BaseDemo.h"
#include "glm/glm.hpp"
#include "VulkanHelper.h"
#include "VulkanCompositeBuffer.h"
#include "VulkanGraphicPipeline.h"
#include "glm\gtx\transform.hpp"
#include "Light.h"
#include <array>
#include <chrono>

struct PerObject {
	alignas(16) glm::mat4 model_matrix;
	alignas(16) glm::vec3 color;
};

struct PerCamera {
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 proj;
};

struct PerLight {
	alignas(16) glm::mat4  light_space_matrix;
};

struct UBOData {
	unsigned char* data = nullptr;
	uint32_t total_size;
	uint32_t stride;
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

const std::vector<Vertex> wall = {
	{glm::vec3(-10.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(10.0f, 0.0f, 10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(10.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
	{glm::vec3(-10.0f, 0.0f, -10.0f), glm::vec3(0.0f, 1.0f, 0.0f)},
};

const std::vector<uint16_t> wall_indices = {0, 1, 2, 2, 3, 0};

const std::vector<PerObject> objects_data = { // 6 matrices 
	{glm::translate(glm::vec3(6.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f)}, //red cube
	{glm::translate(glm::vec3(-6.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 0.0f)}, //yellow cube
	{glm::translate(glm::vec3(0.0, 0.0f, 5.0f)), glm::vec3(1.0f, 0.0f, 1.0f)}, //purple cube
	{glm::translate(glm::vec3(-6.0f, 0.0f, -5.0f)), glm::vec3(1.0f, 0.0f, 0.0f)}, //red cube
	{glm::translate(glm::vec3(2.0f, 3.0f, 8.0f)), glm::vec3(1.0f, 1.0f, 0.0f)}, // yellow cube
	{glm::translate(glm::vec3(0.0f, 6.0f, 4.0f)), glm::vec3(0.0f, 1.0f, 1.0f)}, // cyan cube
	{glm::translate(glm::vec3(0.0f, -5.0f, 0.0f)), glm::vec3(0.2f, 0.2f, 0.2f)}, //floor
};

const uint32_t num_cubes = 6;

class ShadowMapDemo : public BaseDemo {
private:
	UBOData per_object_data;
	vk::VulkanCompositeBuffer cube_vertex_buffer;
	vk::VulkanCompositeBuffer cube_index_buffer;
	vk::VulkanCompositeBuffer wall_vertex_buffer;
	vk::VulkanCompositeBuffer wall_index_buffer;
	VkDescriptorSetLayout depth_descriptor_set_layout;
	VkDescriptorSetLayout draw_descriptor_set_layout;
	VkSampler sampler;
	std::vector<vk::VulkanCompositeImage> depth_attachments;
	VkRenderPass depth_renderpass;
	std::vector<VkFramebuffer> depth_framebuffers;
	VkPipelineLayout depth_pipeline_layout;
	VkPipelineLayout draw_pipeline_layout;
	VkPipeline depth_pipeline;
	VkPipeline draw_pipeline;
	std::vector<vk::VulkanCompositeBuffer> light_uniform_buffers;
	std::vector<vk::VulkanCompositeBuffer> per_object_uniform_buffers;
	std::vector<vk::VulkanCompositeBuffer> per_light_uniform_buffers;
	std::vector<vk::VulkanCompositeBuffer> per_camera_uniform_buffers;
	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> depth_descriptor_sets;
	std::vector<VkDescriptorSet> draw_descriptor_sets;
	std::vector<VkCommandBuffer> offscreen_draw_cmd_buffers;

	const static int offscreen_framebuffer_width = 2048;
	const static int offscreen_framebuffer_height = 2048;
public:

	const char* GetWindowTitle() override {
		return "Shadow Map";
	}

	uint32_t GetWindowInitWidth() override {
		return 800;
	}

	uint32_t GetWindowInitHeight() override {
		return 600;
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

	bool ShowFPS() override {
		return false;
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
	}

	void CreateNonPermanentResources() override {
		CreateAttachments();
		CreateDepthRenderpass();
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
		CleanupRenderpass();
		CleanupAttachments();
		
		
	}

	void CreateVertexAndIndexBuffers() {
		VkDeviceSize cube_vertex_buffer_size = sizeof(Vertex) * cube.size();
		VkDeviceSize cube_index_buffer_size = sizeof(cube_indices[0]) * cube_indices.size();
		VkDeviceSize wall_vertex_buffer_size = sizeof(Vertex) * wall.size();
		VkDeviceSize wall_index_buffer_size = sizeof(wall_indices[0]) * wall_indices.size();

		vk::VulkanCompositeBuffer cube_staging_vertex_buffer;
		cube_staging_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		cube_staging_vertex_buffer.CopyFromHostData(const_cast<Vertex*>(cube.data()), cube_vertex_buffer_size, 0);

		vk::VulkanCompositeBuffer cube_staging_index_buffer;
		cube_staging_index_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		cube_staging_index_buffer.CopyFromHostData(const_cast<uint16_t*>(cube_indices.data()), cube_index_buffer_size, 0);

		vk::VulkanCompositeBuffer wall_staging_vertex_buffer;
		wall_staging_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, wall_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		wall_staging_vertex_buffer.CopyFromHostData(const_cast<Vertex*>(wall.data()), wall_vertex_buffer_size, 0);

		vk::VulkanCompositeBuffer wall_staging_index_buffer;
		wall_staging_index_buffer.CreateBuffer(this->logical_device, this->physical_device, wall_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		wall_staging_index_buffer.CopyFromHostData(const_cast<uint16_t*>(wall_indices.data()), wall_index_buffer_size, 0);

		this->cube_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		this->cube_index_buffer.CreateBuffer(this->logical_device, this->physical_device, cube_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		this->wall_vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, wall_vertex_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		this->wall_index_buffer.CreateBuffer(this->logical_device, this->physical_device, wall_index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		// create fences
		VkFence transfer_finished_fences[4];
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[0]); // create fence in unsignaled state
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[1]); // create fence in unsignaled state
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[2]); // create fence in unsignaled state
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[3]); // create fence in unsignaled state

		this->cube_vertex_buffer.TransferFromAnotherBuffer(cube_staging_vertex_buffer, this->command_pool, 0, 0, cube_vertex_buffer_size, this->queues[0], transfer_finished_fences[0]);
		this->cube_index_buffer.TransferFromAnotherBuffer(cube_staging_index_buffer, this->command_pool, 0, 0, cube_index_buffer_size, this->queues[0], transfer_finished_fences[1]);
		this->wall_vertex_buffer.TransferFromAnotherBuffer(wall_staging_vertex_buffer, this->command_pool, 0, 0, wall_vertex_buffer_size, this->queues[0], transfer_finished_fences[2]);
		this->wall_index_buffer.TransferFromAnotherBuffer(wall_staging_index_buffer, this->command_pool, 0, 0, wall_index_buffer_size, this->queues[0], transfer_finished_fences[3]);

		vkWaitForFences(this->logical_device, 4, transfer_finished_fences, VK_TRUE, UINT64_MAX);

		//cleanup fences and staging buffers
		vkDestroyFence(this->logical_device, transfer_finished_fences[0], nullptr);
		vkDestroyFence(this->logical_device, transfer_finished_fences[1], nullptr);
		vkDestroyFence(this->logical_device, transfer_finished_fences[2], nullptr);
		vkDestroyFence(this->logical_device, transfer_finished_fences[3], nullptr);

		wall_staging_index_buffer.DestroyBuffer();
		wall_staging_vertex_buffer.DestroyBuffer();
		cube_staging_index_buffer.DestroyBuffer();
		cube_staging_vertex_buffer.DestroyBuffer();
	}

	void CleanupVertexAndIndexBuffers() {
		this->wall_index_buffer.DestroyBuffer();
		this->wall_vertex_buffer.DestroyBuffer();
		this->cube_index_buffer.DestroyBuffer();
		this->cube_vertex_buffer.DestroyBuffer();
	}

	void CreateDescriptorSetLayouts() {
		std::vector<VkDescriptorSetLayoutBinding> depth_layout_bindings = {
			vk::init::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT),
		};
		vk::init::CreateDescriptorSetLayout(this->logical_device, depth_layout_bindings, &this->depth_descriptor_set_layout);

		std::vector<VkDescriptorSetLayoutBinding> draw_layout_bindings = {
			vk::init::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(3, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT)
		};
		vk::init::CreateDescriptorSetLayout(this->logical_device, draw_layout_bindings, &this->draw_descriptor_set_layout);
	}

	void CleanupDescriptorSetLayouts() {
		vkDestroyDescriptorSetLayout(this->logical_device, this->draw_descriptor_set_layout, nullptr);
		vkDestroyDescriptorSetLayout(this->logical_device, this->depth_descriptor_set_layout, nullptr);
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

	void CreateAttachments() {
		VkImageCreateInfo depth_create_info = {};
		depth_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		depth_create_info.imageType = VK_IMAGE_TYPE_2D;
		depth_create_info.format = vk::GetSupportedDepthFormat(this->physical_device);
		depth_create_info.extent = { this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, 1 };
		depth_create_info.mipLevels = 1;
		depth_create_info.arrayLayers = 1;
		depth_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		depth_create_info.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
		this->depth_attachments.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->depth_attachments.size(); i++) {
			this->depth_attachments[i].Create(this->physical_device, this->logical_device, depth_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);
		}
	}

	void CleanupAttachments() {
		for (uint32_t i = 0; i < this->depth_attachments.size(); i++) {
			this->depth_attachments[i].Destroy();
		}
	}

	void CreateDepthRenderpass() {
		VkAttachmentDescription depth_attachment_desc = {};
		depth_attachment_desc.format = this->depth_attachments[0].format;
		depth_attachment_desc.samples = VK_SAMPLE_COUNT_1_BIT;
		depth_attachment_desc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment_desc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		depth_attachment_desc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment_desc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment_desc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment_desc.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;

		VkAttachmentReference depth_ref = { 0, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL };

		VkSubpassDescription subpass_desc = {};
		subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass_desc.colorAttachmentCount = 0;
		subpass_desc.pColorAttachments = nullptr;
		subpass_desc.pDepthStencilAttachment = &depth_ref;

		std::vector<VkSubpassDependency> dependencies = { {}, {} };
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[0].srcAccessMask = 0;
		dependencies[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		dependencies[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		//make sure that subsequent fragment shader stage of after-commands wont start until we have finished writing to the depth attachment
		// flush to make sure that the depth_attachment_output's write is visible to the fragment_shader's read. 
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		dependencies[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		dependencies[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		dependencies[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		VkRenderPassCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		create_info.attachmentCount = 1;
		create_info.pAttachments = &depth_attachment_desc;
		create_info.subpassCount = 1;
		create_info.pSubpasses = &subpass_desc;
		create_info.dependencyCount = static_cast<uint32_t>(dependencies.size());
		create_info.pDependencies = dependencies.data();

		if (vkCreateRenderPass(this->logical_device, &create_info, nullptr, &depth_renderpass) != VK_SUCCESS) {
			throw std::runtime_error("fail to create offscreen renderpass");
		}
	}

	void CleanupRenderpass() {
		vkDestroyRenderPass(this->logical_device, this->depth_renderpass, nullptr);
	}

	void CreateFramebuffers() {
		std::vector<VkImageView> attachments(1);
		this->depth_framebuffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < depth_framebuffers.size(); i++) {
			attachments[0] = this->depth_attachments[i].image_view;
			vk::init::CreateFrameBuffer(this->logical_device, this->depth_renderpass, attachments,
				this->offscreen_framebuffer_width, this->offscreen_framebuffer_height, &this->depth_framebuffers[i]);
		}
	}

	void CleanupFramebuffers() {
		for (uint32_t i = 0; i < depth_framebuffers.size(); i++) {
			vkDestroyFramebuffer(this->logical_device, this->depth_framebuffers[i], nullptr);
		}
	}

	void CreatePipelines() {
		CreateDepthPipeline();
		CreateDrawPipeline();
	}

	void CreateDepthPipeline() {
		VkShaderModule vert_shader_module = vk::CreateShaderModule(logical_device, "shaders/depth_vert.spv");
		VkShaderModule frag_shader_module = vk::CreateShaderModule(logical_device, "shaders/depth_frag.spv");

		VkPipelineShaderStageCreateInfo vert_shader_create_info = vk::CreateShaderStageCreateInfo(vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
		VkPipelineShaderStageCreateInfo frag_shader_create_info = vk::CreateShaderStageCreateInfo(frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_create_info, frag_shader_create_info };

		std::vector<VkVertexInputBindingDescription> input_binding_descs = Vertex::GetBindingDescriptions();
		std::vector<VkVertexInputAttributeDescription> input_attrib_descs = Vertex::GetAttributeDescriptions();
		VkPipelineVertexInputStateCreateInfo vertex_input_info = vk::CreateVertexInputStateCreateInfo(input_binding_descs, input_attrib_descs);
		VkPipelineInputAssemblyStateCreateInfo assembly_state_info = vk::CreateInputAssemblyStateCreateInfo(false, VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)this->offscreen_framebuffer_width;
		viewport.height = (float)this->offscreen_framebuffer_height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		std::vector<VkViewport> viewports = { viewport };

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = { this->offscreen_framebuffer_width, this->offscreen_framebuffer_height };
		std::vector<VkRect2D> scissors = { scissor };
		VkPipelineViewportStateCreateInfo viewport_state_info = vk::CreateViewportStateCreateInfo(viewports, scissors);

		VkPipelineRasterizationStateCreateInfo rasterizer = vk::CreateRasterizationStateCreateInfo(false, false, VK_POLYGON_MODE_FILL, 1.0f,
			VK_CULL_MODE_BACK_BIT, VK_FRONT_FACE_COUNTER_CLOCKWISE, false);

		VkPipelineMultisampleStateCreateInfo multisample = vk::CreateMultisampleStateCreateInfo(false, VK_SAMPLE_COUNT_1_BIT);

		VkPipelineColorBlendAttachmentState blend_attachment_state = vk::CreateColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, false);

		std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states = { blend_attachment_state };

		VkPipelineDepthStencilStateCreateInfo depth_stencil = vk::CreateDepthStencilStateCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, false);

		VkPipelineColorBlendStateCreateInfo color_blend_state = vk::CreateColorBlendStateCreateInfo(false, blend_attachment_states);

		std::vector<VkPushConstantRange> constant_ranges;
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts = { this->depth_descriptor_set_layout };
		vk::CreatePipelineLayout(this->logical_device, descriptor_set_layouts, constant_ranges, &this->depth_pipeline_layout);

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
		pipeline_info.layout = this->depth_pipeline_layout; 
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = nullptr; // optional
		pipeline_info.basePipelineIndex = -1; // optional

		pipeline_info.renderPass = this->depth_renderpass;

		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->depth_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create draw graphic pipeline");
		}

		vkDestroyShaderModule(this->logical_device, frag_shader_module, nullptr);
		vkDestroyShaderModule(this->logical_device, vert_shader_module, nullptr);
	}

	void CreateDrawPipeline() {
		VkShaderModule vert_shader_module = vk::CreateShaderModule(logical_device, "shaders/debug_vert.spv");
		VkShaderModule frag_shader_module = vk::CreateShaderModule(logical_device, "shaders/debug_frag.spv");

		VkPipelineShaderStageCreateInfo vert_shader_create_info = vk::CreateShaderStageCreateInfo(vert_shader_module, VK_SHADER_STAGE_VERTEX_BIT);
		VkPipelineShaderStageCreateInfo frag_shader_create_info = vk::CreateShaderStageCreateInfo(frag_shader_module, VK_SHADER_STAGE_FRAGMENT_BIT);

		VkPipelineShaderStageCreateInfo shader_stages[] = { vert_shader_create_info, frag_shader_create_info };

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

		VkPipelineColorBlendAttachmentState blend_attachment_state = vk::CreateColorBlendAttachmentState(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT, false);

		std::vector<VkPipelineColorBlendAttachmentState> blend_attachment_states = { blend_attachment_state };

		VkPipelineDepthStencilStateCreateInfo depth_stencil = vk::CreateDepthStencilStateCreateInfo(true, true, VK_COMPARE_OP_LESS, false, false);

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
		pipeline_info.layout = this->draw_pipeline_layout;
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
		vkDestroyPipeline(this->logical_device, this->depth_pipeline, nullptr);
		vkDestroyPipelineLayout(this->logical_device, this->depth_pipeline_layout, nullptr);
	}

	void CreateUniformBuffers() {
		VkDeviceSize per_light_size = sizeof(PerLight);
		this->per_light_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->per_light_uniform_buffers.size(); i++) {
			this->per_light_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, per_light_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		VkDeviceSize per_object_size = this->per_object_data.total_size;
		this->per_object_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < per_object_uniform_buffers.size(); i++) {
			this->per_object_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, per_object_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		VkDeviceSize per_camera_size = sizeof(PerCamera);
		this->per_camera_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->per_camera_uniform_buffers.size(); i++) {
			this->per_camera_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, per_camera_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		VkDeviceSize light_size = sizeof(cg::PointLight);
		this->light_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->light_uniform_buffers.size(); i++) {
			this->light_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, light_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE,
				VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
	}

	void CleanupUniformBuffers() {
		for (uint32_t i = 0; i < this->light_uniform_buffers.size(); i++) {
			this->light_uniform_buffers[i].DestroyBuffer();
		}
		for (uint32_t i = 0; i < this->per_camera_uniform_buffers.size(); i++) {
			this->per_camera_uniform_buffers[i].DestroyBuffer();
		}
		for (uint32_t i = 0; i < this->per_object_uniform_buffers.size(); i++) {
			this->per_object_uniform_buffers[i].DestroyBuffer();
		}
		for (uint32_t i = 0; i < this->per_light_uniform_buffers.size(); i++) {
			this->per_light_uniform_buffers[i].DestroyBuffer();
		}
	}

	void CreateUboDataArrays() {
		uint32_t min_ubuffer_alignment = vk::GetMinUniformBufferAlignment(this->physical_device);
		this->per_object_data.stride = vk::util::CalculateObjectSize(sizeof(PerObject), min_ubuffer_alignment);
		this->per_object_data.total_size = this->per_object_data.stride * objects_data.size();
		this->per_object_data.data = reinterpret_cast<unsigned char*>(operator new(this->per_object_data.total_size));
	}

	void CleanupUboDataArrays() {
		delete[] this->per_object_data.data;
		this->per_object_data.stride = 0;
		this->per_object_data.total_size = 0;
	}

	void CreateDescriptorPool() {
		std::vector<VkDescriptorPoolSize> poolsizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER , this->vulkan_swap_chain.image_count * 4}, // for per light, per camera, point light
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, this->vulkan_swap_chain.image_count }, // for shadow map
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, this->vulkan_swap_chain.image_count * 2} //for per object
		};
		vk::init::CreateDescriptorPool(this->logical_device, poolsizes, this->vulkan_swap_chain.image_count * 7, &this->descriptor_pool);
	}

	void CreateDescriptorSets() {
		CreateDepthDescriptorSets();
		CreateDrawDescriptorSets();
	}

	void CreateDepthDescriptorSets() {
		this->depth_descriptor_sets.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkDescriptorSetLayout> layouts(this->vulkan_swap_chain.image_count, this->depth_descriptor_set_layout);
		vk::init::AllocateDescriptorSets(this->logical_device, this->descriptor_pool, layouts, depth_descriptor_sets);
		std::vector<VkWriteDescriptorSet> descriptor_writes(2);
		for (uint32_t i = 0; i < depth_descriptor_sets.size(); i++) {
			VkDescriptorBufferInfo binding0_info = vk::init::CreateDescriptorBufferInfo(this->per_light_uniform_buffers[i].buffer, 0, sizeof(PerLight));
			descriptor_writes[0] = vk::init::CreateWriteDescriptorSet(this->depth_descriptor_sets[i], 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &binding0_info, nullptr);
			VkDescriptorBufferInfo binding1_info = vk::init::CreateDescriptorBufferInfo(this->per_object_uniform_buffers[i].buffer, 0, sizeof(PerObject)); //https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDescriptorBufferInfo.html
			descriptor_writes[1] = vk::init::CreateWriteDescriptorSet(this->depth_descriptor_sets[i], 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &binding1_info, nullptr);
			vkUpdateDescriptorSets(this->logical_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
		}
	}

	void CreateDrawDescriptorSets() {
		this->draw_descriptor_sets.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkDescriptorSetLayout> layouts(this->vulkan_swap_chain.image_count, this->draw_descriptor_set_layout);
		vk::init::AllocateDescriptorSets(this->logical_device, this->descriptor_pool, layouts, this->draw_descriptor_sets);
		std::vector<VkWriteDescriptorSet> descriptor_writes(5);
		for (uint32_t i = 0; i < draw_descriptor_sets.size(); i++) {
			VkDescriptorBufferInfo binding0_info = vk::init::CreateDescriptorBufferInfo(this->per_camera_uniform_buffers[i].buffer, 0, sizeof(PerCamera));
			descriptor_writes[0] = vk::init::CreateWriteDescriptorSet(this->draw_descriptor_sets[i], 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &binding0_info, nullptr);
			VkDescriptorBufferInfo binding1_info = vk::init::CreateDescriptorBufferInfo(this->per_object_uniform_buffers[i].buffer, 0, sizeof(PerObject));
			descriptor_writes[1] = vk::init::CreateWriteDescriptorSet(this->draw_descriptor_sets[i], 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &binding1_info, nullptr);
			VkDescriptorBufferInfo binding2_info = vk::init::CreateDescriptorBufferInfo(this->per_light_uniform_buffers[i].buffer, 0, sizeof(PerLight));
			descriptor_writes[2] = vk::init::CreateWriteDescriptorSet(this->draw_descriptor_sets[i], 2, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &binding2_info, nullptr);
			VkDescriptorBufferInfo binding3_info = vk::init::CreateDescriptorBufferInfo(this->light_uniform_buffers[i].buffer, 0, sizeof(cg::PointLight));
			descriptor_writes[3] = vk::init::CreateWriteDescriptorSet(this->draw_descriptor_sets[i], 3, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &binding3_info, nullptr);
			VkDescriptorImageInfo binding4_info = vk::init::CreateDescriptorImageInfo(this->sampler, this->depth_attachments[i].image_view, VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL);
			descriptor_writes[4] = vk::init::CreateWriteDescriptorSet(this->draw_descriptor_sets[i], 4, 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, nullptr, &binding4_info);
			vkUpdateDescriptorSets(this->logical_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);
		}
	}

	void CreateOffscreenDrawCmdBuffers() {
		this->offscreen_draw_cmd_buffers.resize(this->vulkan_swap_chain.image_count);
		vk::init::CreateCmdBuffer(this->logical_device, this->command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			this->vulkan_swap_chain.image_count, this->offscreen_draw_cmd_buffers.data());
		VkDeviceSize vertex_offsets[] = { 0 };

		std::vector<VkClearValue> clear_values = { {} };
		clear_values[0].depthStencil = { 1.0f, 0 };

		for (uint32_t i = 0; i < this->offscreen_draw_cmd_buffers.size(); i++) {
			vk::util::BeginCmdBuffer(this->offscreen_draw_cmd_buffers[i], VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT, nullptr);

			// depth renderpass
			vk::util::BeginRenderpass(this->offscreen_draw_cmd_buffers[i], this->depth_renderpass, this->depth_framebuffers[i], { 0,0 },
				{ this->offscreen_framebuffer_width, this->offscreen_framebuffer_height }, clear_values, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->depth_pipeline);
			vkCmdBindVertexBuffers(this->offscreen_draw_cmd_buffers[i], 0, 1, &this->cube_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->offscreen_draw_cmd_buffers[i], this->cube_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			for (uint32_t j = 0; j < num_cubes; j++) { //draw boxes
				uint32_t dynamic_alignment = j * this->per_object_data.stride;
				vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->depth_pipeline_layout, 0, 1, &this->depth_descriptor_sets[i], 1, &dynamic_alignment);
				vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
			}

			vkCmdBindVertexBuffers(this->offscreen_draw_cmd_buffers[i], 0, 1, &this->wall_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->offscreen_draw_cmd_buffers[i], this->wall_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			for (uint32_t j = num_cubes; j < objects_data.size(); j++) {
				uint32_t dynamic_alignment = j * this->per_object_data.stride;
				vkCmdBindDescriptorSets(this->offscreen_draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->depth_pipeline_layout, 0, 1, &this->depth_descriptor_sets[i], 1, &dynamic_alignment);
				vkCmdDrawIndexed(this->offscreen_draw_cmd_buffers[i], static_cast<uint32_t>(wall_indices.size()), 1, 0, 0, 0);
			}

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

			//vkCmdBindVertexBuffers(this->draw_cmd_buffers[i], 0, 1, &this->cube_vertex_buffer.buffer, vertex_offsets);
			//vkCmdBindIndexBuffer(this->draw_cmd_buffers[i], this->cube_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			//for (uint32_t j = 0; j < num_cubes; j++) { //draw boxes
			//	uint32_t dynamic_alignment = j * this->per_object_data.stride;
			//	vkCmdBindDescriptorSets(this->draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
			//		this->draw_pipeline_layout, 0, 1, &this->draw_descriptor_sets[i], 1, &dynamic_alignment);
			//	vkCmdDrawIndexed(this->draw_cmd_buffers[i], static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
			//}

			//vkCmdBindVertexBuffers(this->draw_cmd_buffers[i], 0, 1, &this->wall_vertex_buffer.buffer, vertex_offsets);
			//vkCmdBindIndexBuffer(this->draw_cmd_buffers[i], this->wall_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);

			//for (uint32_t j = num_cubes; j < objects_data.size(); j++) {
			//	uint32_t dynamic_alignment = j * this->per_object_data.stride;
			//	vkCmdBindDescriptorSets(this->draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
			//		this->draw_pipeline_layout, 0, 1, &this->draw_descriptor_sets[i], 1, &dynamic_alignment);
			//	vkCmdDrawIndexed(this->draw_cmd_buffers[i], static_cast<uint32_t>(wall_indices.size()), 1, 0, 0, 0);
			//}
			
			//test code
			vkCmdBindVertexBuffers(this->draw_cmd_buffers[i], 0, 1, &this->cube_vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->draw_cmd_buffers[i], this->cube_index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			uint32_t dynamic_alignment = 0;
			vkCmdBindDescriptorSets(this->draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->draw_pipeline_layout, 0, 1, &this->draw_descriptor_sets[i], 1, &dynamic_alignment);
			vkCmdDraw(this->draw_cmd_buffers[i], 6, 1, 0, 0);
			//test code

			vkCmdEndRenderPass(this->draw_cmd_buffers[i]);

			if (vkEndCommandBuffer(this->draw_cmd_buffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("fail to end command buffer recording");
			}
		}
	}

	void UpdateUniformBufferData(uint32_t current_image) {
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
		PerCamera mvp;
		mvp.view = glm::lookAt(glm::vec3(0.0, 4.0f, 16.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.proj = glm::perspective(glm::radians(45.0f), this->vulkan_swap_chain.swap_extent.width / (float)this->vulkan_swap_chain.swap_extent.height, 0.1f, 1000.0f);
		mvp.proj[1][1] *= -1;
		this->per_camera_uniform_buffers[current_image].CopyFromHostData(&mvp, sizeof(PerCamera), 0);

		cg::PointLight light;
		light.constant = 1.0f;
		light.linear = 0.0f;
		light.quadratic = 0.0f;
		light.position = glm::vec3(0.0f, 20.0f, 0.0f);
		light.diffuse = glm::vec3(5.0f, 5.0f, 5.0f);
		light.ambient = glm::vec3(0.2f, 0.2f, 0.2f);
		this->light_uniform_buffers[current_image].CopyFromHostData(&light, sizeof(cg::PointLight), 0);

		glm::mat4 light_projection = glm::ortho(-100.0f, 100.0f, -100.0f, 100.0f, 0.1f, 1000.0f);
		PerLight per_light;
		glm::mat4 light_view = glm::lookAt(light.position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		per_light.light_space_matrix = light_projection * light_view;
		this->per_light_uniform_buffers[current_image].CopyFromHostData(&per_light, sizeof(PerLight), 0);

		// used to draw cubes
		unsigned char* obj_ptr = this->per_object_data.data;
		for (uint32_t i = 0; i < objects_data.size(); i++) {
			PerObject per_obj = objects_data[i];
			per_obj.model_matrix = per_obj.model_matrix;
			*reinterpret_cast<PerObject*>(obj_ptr) = per_obj;
			obj_ptr += this->per_object_data.stride;
		}
		this->per_object_uniform_buffers[current_image].CopyFromHostData(this->per_object_data.data, this->per_object_data.total_size, 0);
		
	}

}; 

MAIN_METHOD(ShadowMapDemo)