#include "VulkanGraphicPipeline.h"
#include "VulkanHelper.h"
#include <array>

namespace vk {
	namespace {
		std::vector<char> ReadFile(const char* filename) {
			std::ifstream file(filename, std::ios::ate | std::ios::binary);
			if (!file.is_open()) {
				throw std::runtime_error("failed to open file");
			}
			size_t filesize = (size_t)file.tellg();
			std::vector<char>  buffer(filesize);
			file.seekg(0);
			file.read(buffer.data(), filesize);
			file.close();
			return buffer;
		}
	}
	VkShaderModule CreateShaderModule(VkDevice logical_device, const char* filename) {
		std::vector<char> code = ReadFile(filename);
		VkShaderModuleCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		create_info.codeSize = code.size();
		create_info.pCode = reinterpret_cast<const uint32_t*>(code.data());
		VkShaderModule shader_module;
		if (vkCreateShaderModule(logical_device, &create_info, nullptr, &shader_module) != VK_SUCCESS) {
			throw std::runtime_error("fail to create shader module");
		}
		return shader_module;
	}

	VkPipelineShaderStageCreateInfo CreateShaderStageCreateInfo(VkShaderModule shader_module, VkShaderStageFlagBits stage) {
		VkPipelineShaderStageCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		create_info.stage = stage;
		create_info.module = shader_module;
		create_info.pName = "main";
		return create_info;
	}

	VkPipelineVertexInputStateCreateInfo CreateVertexInputStateCreateInfo(std::vector<VkVertexInputBindingDescription> & input_binding_descs, 
		std::vector<VkVertexInputAttributeDescription> & input_attribute_descs) {
		VkPipelineVertexInputStateCreateInfo vertex_input_info = {};
		vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertex_input_info.vertexBindingDescriptionCount = static_cast<uint32_t>(input_binding_descs.size());
		vertex_input_info.pVertexBindingDescriptions = input_binding_descs.data(); 
		vertex_input_info.vertexAttributeDescriptionCount = static_cast<uint32_t>(input_attribute_descs.size());
		vertex_input_info.pVertexAttributeDescriptions = input_attribute_descs.data(); 
		return vertex_input_info;
	}

	VkPipelineInputAssemblyStateCreateInfo CreateInputAssemblyStateCreateInfo(bool primitive_restart_enabled, VkPrimitiveTopology topology) {
		VkPipelineInputAssemblyStateCreateInfo input_assembly = {};
		input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		input_assembly.topology = topology;
		input_assembly.primitiveRestartEnable = vk::util::GetVkBoolean(primitive_restart_enabled);
		return input_assembly;
	}

	VkPipelineViewportStateCreateInfo CreateViewportStateCreateInfo(std::vector<VkViewport>& viewports, std::vector<VkRect2D>& scissors) {
		VkPipelineViewportStateCreateInfo viewport_state = {};
		viewport_state.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewport_state.viewportCount = static_cast<uint32_t>(viewports.size());
		viewport_state.pViewports = viewports.data();
		viewport_state.scissorCount = static_cast<uint32_t>(scissors.size());
		viewport_state.pScissors = scissors.data();
		return viewport_state;
	}

	VkPipelineRasterizationStateCreateInfo CreateRasterizationStateCreateInfo(bool depth_clamp_enabled, bool rasterizar_discard_enabled,
		VkPolygonMode polygon_mode, float line_width, VkCullModeFlags cull_mode, VkFrontFace front_face, bool depth_bias_enabled,
		float depth_bias_const_factor, float depth_bias_clamp, float depth_bias_slope_factor) {
		
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = vk::util::GetVkBoolean(depth_clamp_enabled);
		rasterizer.rasterizerDiscardEnable = vk::util::GetVkBoolean(rasterizar_discard_enabled);
		rasterizer.polygonMode = polygon_mode;
		rasterizer.lineWidth = line_width;
		rasterizer.cullMode = cull_mode;
		rasterizer.frontFace = front_face;
		rasterizer.depthBiasEnable = depth_bias_enabled;
		rasterizer.depthBiasConstantFactor = depth_bias_const_factor; // optional
		rasterizer.depthBiasClamp = depth_bias_clamp; //optional
		rasterizer.depthBiasSlopeFactor = depth_bias_slope_factor; // optional
		return rasterizer;
	}

	VkPipelineMultisampleStateCreateInfo CreateMultisampleStateCreateInfo(bool sample_shading_enabled, VkSampleCountFlagBits rasterization_samples,
		float min_sample_shading, VkSampleMask* sample_mask, bool alpha_to_coverage_enabled, bool alpha_to_one_enabled) {

		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = vk::util::GetVkBoolean(sample_shading_enabled);
		multisampling.rasterizationSamples = rasterization_samples;
		multisampling.minSampleShading = min_sample_shading; // Optional
		multisampling.pSampleMask = sample_mask; // Optional
		multisampling.alphaToCoverageEnable = vk::util::GetVkBoolean(alpha_to_coverage_enabled); // Optional
		multisampling.alphaToOneEnable = vk::util::GetVkBoolean(alpha_to_one_enabled); // Optional
		return multisampling;
	}

	VkPipelineColorBlendStateCreateInfo CreateColorBlendStateCreateInfo(bool logic_op_enabled, std::vector<VkPipelineColorBlendAttachmentState>& attachments,
		VkLogicOp logic_op, float const0, float const1, float const2, float const3) {
		VkPipelineColorBlendStateCreateInfo blend_state = {};
		blend_state.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blend_state.logicOpEnable = vk::util::GetVkBoolean(logic_op_enabled);
		blend_state.attachmentCount = static_cast<uint32_t>(attachments.size());
		blend_state.pAttachments = attachments.data();
		blend_state.logicOp = logic_op;
		blend_state.blendConstants[0] = const0;
		blend_state.blendConstants[1] = const1;
		blend_state.blendConstants[2] = const2;
		blend_state.blendConstants[3] = const3;
		return blend_state;
	}

	VkPipelineDepthStencilStateCreateInfo CreateDepthStencilStateCreateInfo(bool depth_test_enabled, bool depth_write_enabled, VkCompareOp compare_op, 
		bool depth_bounds_test_enabled, bool stencil_test_enabled, float min_depth_bounds, float max_depth_bounds, VkStencilOpState front, VkStencilOpState back) {
		VkPipelineDepthStencilStateCreateInfo depth_stencil = {};
		depth_stencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depth_stencil.depthTestEnable = vk::util::GetVkBoolean(depth_test_enabled);
		depth_stencil.depthWriteEnable = vk::util::GetVkBoolean(depth_write_enabled);
		depth_stencil.depthCompareOp = compare_op;
		depth_stencil.depthBoundsTestEnable = vk::util::GetVkBoolean(depth_bounds_test_enabled);
		depth_stencil.minDepthBounds = min_depth_bounds; // Optional
		depth_stencil.maxDepthBounds = max_depth_bounds; // Optional
		depth_stencil.stencilTestEnable = vk::util::GetVkBoolean(stencil_test_enabled);
		depth_stencil.front = front; // Optional
		depth_stencil.back = back; // Option
		return depth_stencil;
	}

	VkPipelineDynamicStateCreateInfo CreateDynamicStateCreateInfo(std::vector<VkDynamicState>& dynamic_states) {
		VkPipelineDynamicStateCreateInfo create_info = {};
		create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		create_info.dynamicStateCount = static_cast<uint32_t>(dynamic_states.size());
		create_info.pDynamicStates = dynamic_states.data();
		return create_info;
	}

	VkPipelineColorBlendAttachmentState CreateColorBlendAttachmentState(VkColorComponentFlags color_write_mask, bool blend_enable,
		VkBlendFactor src_color_blend_factor, VkBlendFactor dst_color_blend_factor, VkBlendOp color_blend_op,
		VkBlendFactor src_alpha_blend_factor, VkBlendFactor dst_alpha_blend_factor, VkBlendOp alpha_blend_op) {
		VkPipelineColorBlendAttachmentState color_blend_attachment = {};
		color_blend_attachment.colorWriteMask = color_write_mask;
		color_blend_attachment.blendEnable = vk::util::GetVkBoolean(blend_enable);
		color_blend_attachment.srcColorBlendFactor = src_color_blend_factor; // Optional
		color_blend_attachment.dstColorBlendFactor = dst_color_blend_factor; // Optional
		color_blend_attachment.colorBlendOp = color_blend_op; // Optional
		color_blend_attachment.srcAlphaBlendFactor = src_alpha_blend_factor; // Optional
		color_blend_attachment.dstAlphaBlendFactor = dst_alpha_blend_factor; // Optional
		color_blend_attachment.alphaBlendOp = alpha_blend_op; // Optional
		return color_blend_attachment;
	}

	void CreatePipelineLayout(VkDevice logical_device, std::vector<VkDescriptorSetLayout> & set_layouts, std::vector<VkPushConstantRange> & constant_ranges, 
		VkPipelineLayout * pipeline_layout) {
		VkPipelineLayoutCreateInfo pipeline_layout_info = {};
		pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_info.setLayoutCount = static_cast<uint32_t>(set_layouts.size()); 
		pipeline_layout_info.pSetLayouts = set_layouts.data(); 
		pipeline_layout_info.pushConstantRangeCount = static_cast<uint32_t>(constant_ranges.size()); 
		pipeline_layout_info.pPushConstantRanges = constant_ranges.data(); 
		if (vkCreatePipelineLayout(logical_device, &pipeline_layout_info, nullptr, pipeline_layout) != VK_SUCCESS) {
			throw std::runtime_error("fail to create pipeline layout");
		}
	}

}