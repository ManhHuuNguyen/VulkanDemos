#pragma once
#include <vector>
#include <string>
#include <fstream>
#include "vulkan/vulkan.h"

namespace vk {
	namespace {
		std::vector<char> ReadFile(const char* filename);
	}

	VkShaderModule CreateShaderModule(VkDevice logical_device, const char* filename);

	VkPipelineShaderStageCreateInfo CreateShaderStageCreateInfo(VkShaderModule shader_module, VkShaderStageFlagBits stage);

	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription>& input_binding_descs,
		std::vector<VkVertexInputAttributeDescription>& input_attribute_descs);

	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateCreateInfo(bool primitive_restart_enabled, VkPrimitiveTopology topology);

	VkPipelineViewportStateCreateInfo CreateViewportStateCreateInfo(std::vector<VkViewport> & viewports, std::vector<VkRect2D> & scissors);

	VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(bool depth_clamp_enabled, bool rasterizar_discard_enabled,
		VkPolygonMode polygon_mode, float line_width, VkCullModeFlags cull_mode, VkFrontFace front_face, bool depth_bias_enabled,
		float depth_bias_const_factor = 0.0f, float depth_bias_clamp = 0.0f, float depth_bias_slope_factor = 0.0f);

	VkPipelineMultisampleStateCreateInfo CreateMultisampleStateCreateInfo(bool sample_shading_enabled, VkSampleCountFlagBits rasterization_samples,
		float min_sample_shading = 1.0f, VkSampleMask * sample_mask = nullptr, bool alpha_to_coverage_enabled = false, bool alpha_to_one_enabled = false);
	
	VkPipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(bool logic_op_enabled, std::vector<VkPipelineColorBlendAttachmentState>& attachments,
		VkLogicOp logic_op = VK_LOGIC_OP_COPY, float const0 = 0.0f, float const1 = 0.0f, float const2 = 0.0f, float const3 = 0.0f);

	VkPipelineDynamicStateCreateInfo CreateDynamicStateCreateInfo(std::vector<VkDynamicState>& dynamic_states);

	VkPipelineDepthStencilStateCreateInfo CreateDepthStencilStateCreateInfo(bool depth_test_enabled, bool depth_write_enabled, VkCompareOp compare_op,
		bool depth_bounds_test_enabled, bool stencil_test_enabled, 
		float min_depth_bounds = 0.0f, float max_depth_bounds = 1.0f, VkStencilOpState front = {}, VkStencilOpState back = {});
	
	VkPipelineColorBlendAttachmentState CreateColorBlendAttachmentState(VkColorComponentFlags color_write_mask, bool blend_enable,
		VkBlendFactor src_color_blend_factor = VK_BLEND_FACTOR_ONE, VkBlendFactor dst_color_blend_factor = VK_BLEND_FACTOR_ZERO, VkBlendOp color_blend_op = VK_BLEND_OP_ADD,
		VkBlendFactor src_alpha_blend_factor = VK_BLEND_FACTOR_ONE, VkBlendFactor dst_alpha_blend_factor = VK_BLEND_FACTOR_ZERO, VkBlendOp alpha_blend_op = VK_BLEND_OP_ADD);
	
	void CreatePipelineLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayout>& set_layouts, std::vector<VkPushConstantRange>& constant_ranges, VkPipelineLayout * layout);


}