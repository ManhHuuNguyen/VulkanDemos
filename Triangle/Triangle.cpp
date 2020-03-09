#include "BaseDemo.h"
#include <iostream>
#include "VulkanGraphicPipeline.h"
#include "VulkanHelper.h"
#include "glm/glm.hpp"
#include "VulkanCompositeBuffer.h"
#include "glm/gtc/matrix_transform.hpp"
#include <chrono>
#include <array>
#include "VulkanPhysicalDevice.h"
#include "Light.h"
#include "glm\gtx\transform.hpp"



struct UBOData {
	unsigned char* data = nullptr;
	uint32_t total_size;
	uint32_t stride;
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

const std::vector<PerObject> boxes_data = { // 10 matrices 
	{glm::translate(glm::vec3(6.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 0.0f, 0.0f)},
	{glm::translate(glm::vec3(-6.0f, 0.0f, 0.0f)), glm::vec3(1.0f, 1.0f, 0.0f)},
	{glm::translate(glm::vec3(0.0, 0.0f, 5.0f)), glm::vec3(1.0f, 0.0f, 1.0f)},
	{glm::translate(glm::vec3(0.0f, 0.0f, -5.0f)), glm::vec3(1.0f, 0.0f, 0.0f)},

};
class TriangleDemo : public BaseDemo {

private:
	UBOData per_object_data;
	VkDescriptorSetLayout descriptor_set_layout;
	VkPipelineLayout pipeline_layout;
	VkPipeline graphic_pipeline;
	vk::VulkanCompositeBuffer vertex_buffer;
	vk::VulkanCompositeBuffer index_buffer;
	std::vector<vk::VulkanCompositeBuffer> light_uniform_buffers;
	std::vector<vk::VulkanCompositeBuffer> per_camera_uniform_buffers;
	std::vector<vk::VulkanCompositeBuffer> per_obj_uniform_buffers;
	VkDescriptorPool descriptor_pool;
	std::vector<VkDescriptorSet> descriptor_sets;

public:
	const char* GetWindowTitle() override {
		return "Triangle Demo";
	}

	uint32_t GetWindowInitWidth() override {
		return 800;
	}

	uint32_t GetWindowInitHeight() override {
		return 600;
	}

	bool ShowFPS() override {
		return false;
	}

	void Draw() override {
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
		if (this->images_inflight[image_index] != VK_NULL_HANDLE) {
			vkWaitForFences(this->logical_device, 1, &this->images_inflight[image_index], VK_TRUE, UINT64_MAX);
		}
		this->images_inflight[image_index] = this->cmdbuffers_inflight[this->current_frame];
		std::vector<VkSemaphore> wait_semaphores = { this->image_available_semaphores[this->current_frame]};
		std::vector<VkPipelineStageFlags> wait_stages = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		std::vector<VkSemaphore> signal_semaphores = { this->render_finished_semaphores[this->current_frame] };

		vkResetFences(this->logical_device, 1, &this->cmdbuffers_inflight[current_frame]);
		// queue[0] is present and graphic queue
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
		CreateDescriptorSetLayout();
	}

	void CleanupPermanentResources() override {
		vkDestroyDescriptorSetLayout(this->logical_device, this->descriptor_set_layout, nullptr);
		this->index_buffer.DestroyBuffer();
		this->vertex_buffer.DestroyBuffer();
		CleanupUboDataArrays();
	};

	void CreateNonPermanentResources() override {
		CreatePipelines();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateDrawCmdBuffers();
	}

	void CleanupNonPermanentResources() override {
		vkFreeCommandBuffers(this->logical_device, this->command_pool, static_cast<uint32_t>(this->draw_cmd_buffers.size()), this->draw_cmd_buffers.data());
		vkDestroyDescriptorPool(this->logical_device, this->descriptor_pool, nullptr);
		CleanupUniformBuffers();
		CleanupPipelines();
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

			vkCmdBindPipeline(this->draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, this->graphic_pipeline);
			vkCmdBindVertexBuffers(this->draw_cmd_buffers[i], 0, 1, &this->vertex_buffer.buffer, vertex_offsets);
			vkCmdBindIndexBuffer(this->draw_cmd_buffers[i], this->index_buffer.buffer, 0, VK_INDEX_TYPE_UINT16);
			for (uint32_t j = 0; j < boxes_data.size(); j++) { //draw boxes
				uint32_t dynamic_alignment = j * this->per_object_data.stride;
				vkCmdBindDescriptorSets(this->draw_cmd_buffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS,
					this->pipeline_layout, 0, 1, &this->descriptor_sets[i], 1, &dynamic_alignment);
				vkCmdDrawIndexed(this->draw_cmd_buffers[i], static_cast<uint32_t>(cube_indices.size()), 1, 0, 0, 0);
			}
			
			vkCmdEndRenderPass(this->draw_cmd_buffers[i]);

			if (vkEndCommandBuffer(this->draw_cmd_buffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("fail to end command buffer recording");
			}
		}
	}

	void CreatePipelines() {
		VkShaderModule vert_shader_module = vk::CreateShaderModule(this->logical_device, "shaders/firstpass_vert.spv");
		VkShaderModule frag_shader_module = vk::CreateShaderModule(this->logical_device, "shaders/firstpass_frag.spv");

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
		viewport.width = (float) vulkan_swap_chain.swap_extent.width;
		viewport.height = (float) vulkan_swap_chain.swap_extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		std::vector<VkViewport> viewports = { viewport };

		VkRect2D scissor = {};
		scissor.offset = { 0, 0 };
		scissor.extent = vulkan_swap_chain.swap_extent;
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

		std::vector<VkDynamicState> states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		VkPipelineDynamicStateCreateInfo dynamic_state = vk::CreateDynamicStateCreateInfo(states);

		std::vector<VkPushConstantRange> constant_ranges;
		std::vector<VkDescriptorSetLayout> descriptor_set_layouts = {this->descriptor_set_layout};
		vk::CreatePipelineLayout(this->logical_device , descriptor_set_layouts, constant_ranges, &this->pipeline_layout);

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
		pipeline_info.layout = this->pipeline_layout;
		pipeline_info.renderPass = renderpass;
		pipeline_info.subpass = 0;
		pipeline_info.basePipelineHandle = nullptr; // optional
		pipeline_info.basePipelineIndex = -1; // optional
		
		if (vkCreateGraphicsPipelines(this->logical_device, VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &this->graphic_pipeline) != VK_SUCCESS) {
			throw std::runtime_error("fail to create graphics pipeline");
		}

		vkDestroyShaderModule(this->logical_device, frag_shader_module, nullptr);
		vkDestroyShaderModule(this->logical_device, vert_shader_module, nullptr);
	}

	void CleanupPipelines() {
		vkDestroyPipeline(this->logical_device, this->graphic_pipeline, nullptr);
		vkDestroyPipelineLayout(this->logical_device, this->pipeline_layout, nullptr);
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

	void CreateVertexAndIndexBuffers() {
		VkDeviceSize buffer_size = sizeof(Vertex) * cube.size();
		VkDeviceSize index_buffer_size = sizeof(cube_indices[0]) * cube_indices.size();
		//create temporary staging buffers then copy from host
		vk::VulkanCompositeBuffer staging_buffer;
		staging_buffer.CreateBuffer(this->logical_device, this->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE, 
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		staging_buffer.CopyFromHostData(const_cast<Vertex*>(cube.data()), buffer_size, 0);


		vk::VulkanCompositeBuffer staging_index_buffer;
		staging_index_buffer.CreateBuffer(this->logical_device, this->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_SHARING_MODE_EXCLUSIVE,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		staging_index_buffer.CopyFromHostData(const_cast<uint16_t*>(cube_indices.data()), index_buffer_size, 0);

		// create vertex buffers
		this->vertex_buffer.CreateBuffer(this->logical_device, this->physical_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		// create index buffer
		this->index_buffer.CreateBuffer(this->logical_device, this->physical_device, index_buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
			VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
		
		// create fences
		VkFence transfer_finished_fences[2];
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[0]); // create fence in unsignaled state
		vk::init::CreateFence(this->logical_device, 0, &transfer_finished_fences[1]); // create fence in unsignaled state
		//transfer
		this->vertex_buffer.TransferFromAnotherBuffer(staging_buffer, this->command_pool, 0, 0, buffer_size, this->queues[0], transfer_finished_fences[0]);
		this->index_buffer.TransferFromAnotherBuffer(staging_index_buffer, this->command_pool, 0, 0, index_buffer_size, this->queues[0], transfer_finished_fences[1]);
		// wait for transfer to finish
		vkWaitForFences(this->logical_device, 2, transfer_finished_fences, VK_TRUE, UINT64_MAX);
		
		//cleanup fences and staging buffers
		vkDestroyFence(this->logical_device, transfer_finished_fences[0], nullptr);
		vkDestroyFence(this->logical_device, transfer_finished_fences[1], nullptr);

		staging_buffer.DestroyBuffer();
		staging_index_buffer.DestroyBuffer();
	}


	void CreateDescriptorSetLayout(){
		std::vector<VkDescriptorSetLayoutBinding> layout_bindings = {
			vk::init::CreateDescriptorSetLayoutBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT),
			vk::init::CreateDescriptorSetLayoutBinding(2, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT) //4 is number of lights
		};

		vk::init::CreateDescriptorSetLayout(this->logical_device, layout_bindings, &this->descriptor_set_layout);
	}

	void CreateUniformBuffers() {
		VkDeviceSize light_uniform_buffer_size = sizeof(cg::PointLight);
		this->light_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->light_uniform_buffers.size(); i++) {
			this->light_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, light_uniform_buffer_size,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		VkDeviceSize camera_uniform_buffer_size = sizeof(PerCamera);
		this->per_camera_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->per_camera_uniform_buffers.size(); i++) {
			this->per_camera_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, camera_uniform_buffer_size,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		}
		VkDeviceSize model_uniform_buffer_size = this->per_object_data.total_size;
		this->per_obj_uniform_buffers.resize(this->vulkan_swap_chain.image_count);
		for (uint32_t i = 0; i < this->per_obj_uniform_buffers.size(); i++) {
			this->per_obj_uniform_buffers[i].CreateBuffer(this->logical_device, this->physical_device, model_uniform_buffer_size,
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
			// in real world, dont do host coherent, but do a manual flush of the updated buffer memory part for better performance. ie:  vkFlushMappedMemoryRanges.
		}
	}

	void CleanupUniformBuffers() {
		for (uint32_t i = 0; i < this->per_obj_uniform_buffers.size(); i++) {
			per_obj_uniform_buffers[i].DestroyBuffer();
		}
		for (uint32_t i = 0; i < this->per_camera_uniform_buffers.size(); i++) {
			per_camera_uniform_buffers[i].DestroyBuffer();
		}
		for (uint32_t i = 0; i < this->light_uniform_buffers.size(); i++) {
			light_uniform_buffers[i].DestroyBuffer();
		}
	}

	void UpdateUniformBufferData(uint32_t current_image) {
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		float elapsed = std::chrono::duration<float, std::chrono::seconds::period>(current_time - start_time).count();
		PerCamera mvp;
		mvp.view = glm::lookAt(glm::vec3(8.0, 20.0f, 16.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		mvp.proj = glm::perspective(glm::radians(45.0f), this->vulkan_swap_chain.swap_extent.width / (float)this->vulkan_swap_chain.swap_extent.height, 0.1f, 1000.0f);
		mvp.proj[1][1] *= -1;

		glm::mat4 rotating = glm::rotate(glm::mat4(1.0f), elapsed * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));

		cg::PointLight light;
		light.position = glm::vec3(mvp.view * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		light.ambient = glm::vec3(0.3f, 0.3f, 0.3f);
		light.diffuse = glm::vec3(245.0f, 245.0f, 245.0f);
		light.specular = glm::vec3(5.0f, 5.0f, 5.0f);
		light.constant = 1.0f;
		light.linear = 0.09f;
		light.quadratic = 0.032f;
	
		// uses to draw both light sources and cubes
		unsigned char* obj_ptr = this->per_object_data.data;
		for (uint32_t i = 0; i < boxes_data.size(); i++) {
			PerObject per_obj = boxes_data[i];
			per_obj.model_matrix = per_obj.model_matrix;
			*reinterpret_cast<PerObject*>(obj_ptr) = per_obj;
			obj_ptr += this->per_object_data.stride;
		}
		this->light_uniform_buffers[current_image].CopyFromHostData(&light, sizeof(cg::PointLight), 0);
		this->per_camera_uniform_buffers[current_image].CopyFromHostData(&mvp, sizeof(PerCamera), 0);
		this->per_obj_uniform_buffers[current_image].CopyFromHostData(this->per_object_data.data, this->per_object_data.total_size, 0);;
	}

	void CreateDescriptorPool() {
		std::vector<VkDescriptorPoolSize> poolsizes = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, this->vulkan_swap_chain.image_count * 2},
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, this->vulkan_swap_chain.image_count}
		};
		vk::init::CreateDescriptorPool(this->logical_device, poolsizes, this->vulkan_swap_chain.image_count * 3, &this->descriptor_pool);
	}

	void CreateDescriptorSets() {
		this->descriptor_sets.resize(this->vulkan_swap_chain.image_count);
		std::vector<VkDescriptorSetLayout> layouts(this->vulkan_swap_chain.image_count, this->descriptor_set_layout);
		vk::init::AllocateDescriptorSets(this->logical_device, this->descriptor_pool, layouts, this->descriptor_sets);

		for (uint32_t i = 0; i < this->descriptor_sets.size(); i++) {

			std::vector<VkWriteDescriptorSet> descriptor_writes;

			VkDescriptorBufferInfo binding0_info = vk::init::CreateDescriptorBufferInfo(this->per_camera_uniform_buffers[i].buffer, 0, sizeof(PerCamera));
			descriptor_writes.push_back(vk::init::CreateWriteDescriptorSet(this->descriptor_sets[i], 0, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &binding0_info, nullptr));

			VkDescriptorBufferInfo binding1_info = vk::init::CreateDescriptorBufferInfo(this->per_obj_uniform_buffers[i].buffer, 0, sizeof(PerObject)); //https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDescriptorBufferInfo.html
			descriptor_writes.push_back(vk::init::CreateWriteDescriptorSet(this->descriptor_sets[i], 1, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, &binding1_info, nullptr));

			VkDescriptorBufferInfo binding2_info = vk::init::CreateDescriptorBufferInfo(this->light_uniform_buffers[i].buffer, 0, sizeof(cg::PointLight)); //https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDescriptorBufferInfo.html
			descriptor_writes.push_back(vk::init::CreateWriteDescriptorSet(this->descriptor_sets[i], 2, 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, &binding2_info, nullptr));
			vkUpdateDescriptorSets(this->logical_device, static_cast<uint32_t>(descriptor_writes.size()), descriptor_writes.data(), 0, nullptr);

		}
	}

	void CreateUboDataArrays() {
		uint32_t min_ubuffer_alignment = vk::GetMinUniformBufferAlignment(this->physical_device);
		this->per_object_data.stride = vk::util::CalculateObjectSize(sizeof(PerObject), min_ubuffer_alignment);
		this->per_object_data.total_size = this->per_object_data.stride * boxes_data.size();
		this->per_object_data.data = reinterpret_cast<unsigned char*>(operator new(this->per_object_data.total_size));
	}

	void CleanupUboDataArrays() {
		delete[] this->per_object_data.data;
		this->per_object_data.total_size = 0;
		this->per_object_data.stride = 0;
	}
};

MAIN_METHOD(TriangleDemo)