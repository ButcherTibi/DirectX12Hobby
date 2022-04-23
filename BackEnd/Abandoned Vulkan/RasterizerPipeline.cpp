// Header
#include "DreamGraphicsAPI.hpp"

using namespace dga;


void dga::setToDefault(VkPipelineColorBlendAttachmentState& blend_state)
{
	blend_state.blendEnable = false;
	blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	blend_state.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	blend_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	blend_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	blend_state.alphaBlendOp = VK_BLEND_OP_ADD; // Optional
	blend_state.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
}

void dga::setToAlphaBlending(VkPipelineColorBlendAttachmentState& blend_state)
{
	blend_state.blendEnable = true;
	blend_state.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blend_state.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blend_state.colorBlendOp = VK_BLEND_OP_ADD;
	blend_state.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blend_state.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blend_state.alphaBlendOp = VK_BLEND_OP_ADD;
	blend_state.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
}

void RasterizerPipeline::reset()
{
	// Input Assembly
	vertex_input_info = {};
	vertex_input_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertex_input_info.vertexBindingDescriptionCount = 0;
	vertex_input_info.pVertexBindingDescriptions = nullptr;
	vertex_input_info.vertexAttributeDescriptionCount = 0;
	vertex_input_info.pVertexAttributeDescriptions = nullptr;

	input_assembly_info = {};
	input_assembly_info.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	input_assembly_info.primitiveRestartEnable = false;

	// Vertex Shader
	vertex_stage_info = {};
	vertex_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertex_stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertex_stage_info.module = nullptr;
	vertex_stage_info.pName = "main";

	// Rasterizer
	viewport_info = {};
	viewport_info.x = 0;
	viewport_info.y = 0;
	viewport_info.width = FLT_MAX;
	viewport_info.height = FLT_MAX;
	viewport_info.minDepth = 0;
	viewport_info.maxDepth = 1;

	scissor_info = {};
	scissor_info.offset.x = 0;
	scissor_info.offset.y = 0;
	scissor_info.extent.width = 0xFFFF'FFFF;
	scissor_info.extent.height = 0xFFFF'FFFF;

	viewport_state_info = {};
	viewport_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewport_state_info.viewportCount = 1;
	viewport_state_info.pViewports = &viewport_info;
	viewport_state_info.scissorCount = 1;
	viewport_state_info.pScissors = &scissor_info;

	rasterizer_info = {};
	rasterizer_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer_info.depthClampEnable = false;
	rasterizer_info.rasterizerDiscardEnable = false;
	rasterizer_info.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
	rasterizer_info.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
	rasterizer_info.frontFace = VkFrontFace::VK_FRONT_FACE_CLOCKWISE;
	rasterizer_info.depthBiasEnable = false;
	rasterizer_info.depthBiasConstantFactor = 0;
	rasterizer_info.depthBiasClamp = 0;
	rasterizer_info.depthBiasSlopeFactor = 0;
	rasterizer_info.lineWidth = 1.0;

	multisample_info = {};
	multisample_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisample_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisample_info.sampleShadingEnable = false;
	multisample_info.minSampleShading = 1.0f;
	multisample_info.pSampleMask = nullptr;
	multisample_info.alphaToCoverageEnable = false;
	multisample_info.alphaToOneEnable = false;

	// Pixel Shader
	pixel_stage_info = {};
	pixel_stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	pixel_stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	pixel_stage_info.module = nullptr;
	pixel_stage_info.pName = "main";

	// Blend State
	blend_state_info = {};
	blend_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blend_state_info.logicOpEnable = false;
	blend_state_info.logicOp = VK_LOGIC_OP_COPY;
	blend_state_info.blendConstants[0] = 0.0f;
	blend_state_info.blendConstants[1] = 0.0f;
	blend_state_info.blendConstants[2] = 0.0f;
	blend_state_info.blendConstants[3] = 0.0f;

	// RenderPass
	subpass_desc = {};
	subpass_desc.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

	// Layout
	pipe_layout_info = {};
	pipe_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipe_layout_info.setLayoutCount = 0;
	pipe_layout_info.pSetLayouts = nullptr;
	pipe_layout_info.pushConstantRangeCount = 0;
	pipe_layout_info.pPushConstantRanges = nullptr;

	// Vulkan Objects
	atachments.clear();
	vertex_shader_input_count = 0;
	pixel_shader_input_count = 0;
	render_target_index_count = 0;

	renderpass = nullptr;
	framebuffer = nullptr;
	pipe_layout = nullptr;
	pipeline = nullptr;
}

void RasterizerPipeline::setPrimitiveTopology(VkPrimitiveTopology topology)
{
	input_assembly_info.topology = topology;
}

void RasterizerPipeline::setVertexShader(VertexShader* vertex_shader)
{
	if (vertex_stage_info.module != nullptr) {
		throw;
	}

	vertex_stage_info.module = vertex_shader->shader;
	stage_infos.push_back(vertex_stage_info);
}

void RasterizerPipeline::setViewportPosition(float x, float y)
{
	viewport_info.x = x;
	viewport_info.y = y;
}

void RasterizerPipeline::setViewportSize(float width, float height)
{
	viewport_info.width = width;
	viewport_info.height = height;
}

void RasterizerPipeline::setViewportDepth(float min_depth, float max_depth)
{
	viewport_info.minDepth = min_depth;
	viewport_info.maxDepth = max_depth;
}

void RasterizerPipeline::setScissorOffset(int32_t x, int32_t y)
{
	scissor_info.offset.x = x;
	scissor_info.offset.y = y;
}

void RasterizerPipeline::setScissorExtend(uint32_t width, uint32_t height)
{
	scissor_info.extent.width = width;
	scissor_info.extent.height = height;
}

void RasterizerPipeline::setPolygonMode(VkPolygonMode mode)
{
	rasterizer_info.polygonMode = mode;
}

void RasterizerPipeline::setCullMode(VkCullModeFlags flags)
{
	rasterizer_info.cullMode = flags;
}

void RasterizerPipeline::setFrontFace(VkFrontFace front_face)
{
	rasterizer_info.frontFace = front_face;
}

void RasterizerPipeline::setMultisampleCount(VkSampleCountFlagBits flags)
{
	multisample_info.rasterizationSamples = flags;
}

void RasterizerPipeline::setPixelShader(PixelShader* pixel_shader)
{
	if (pixel_stage_info.module != nullptr) {
		throw;
	}

	pixel_stage_info.module = pixel_shader->shader;
	stage_infos.push_back(pixel_stage_info);
}

void RasterizerPipeline::addRenderTarget(TextureView* texture_view, const VkPipelineColorBlendAttachmentState& blend_state)
{
	Attachment& attachment = atachments.emplace_back();
	attachment.tex_view = texture_view;

	VkPipelineColorBlendAttachmentState& blend_attachment_info = blend_attachments_infos.emplace_back();
	blend_attachment_info.blendEnable         = blend_state.blendEnable;
	blend_attachment_info.srcColorBlendFactor = blend_state.srcColorBlendFactor;
	blend_attachment_info.dstColorBlendFactor = blend_state.dstColorBlendFactor;
	blend_attachment_info.colorBlendOp        = blend_state.colorBlendOp;
	blend_attachment_info.srcAlphaBlendFactor = blend_state.srcAlphaBlendFactor;
	blend_attachment_info.dstAlphaBlendFactor = blend_state.dstAlphaBlendFactor;
	blend_attachment_info.alphaBlendOp        = blend_state.alphaBlendOp;
	blend_attachment_info.colorWriteMask      = blend_state.colorWriteMask;

	VkAttachmentDescription& description = attach_descps.emplace_back();
	description.flags = 0;
	description.format = texture_view->format;
	description.samples = VK_SAMPLE_COUNT_1_BIT;
	description.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	description.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	description.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	description.stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
	description.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	description.finalLayout = VK_IMAGE_LAYOUT_GENERAL;

	VkAttachmentReference& reference = color_attach_refs.emplace_back();
	reference.attachment = (uint32_t)attach_descps.size() - 1;
	reference.layout = VK_IMAGE_LAYOUT_GENERAL;

	views.push_back(texture_view->view);

	attachment.render_target_index = render_target_index_count;
	render_target_index_count += 1;
}

void RasterizerPipeline::addRenderTarget(TextureView* texture_view)
{
	VkPipelineColorBlendAttachmentState blend_state;
	setToDefault(blend_state);
	addRenderTarget(texture_view, blend_state);
}

void RasterizerPipeline::_createVkObjects()
{
	// Defaults
	{
		if (atachments.size()) {
			default_width = atachments[0].tex_view->texture->width;
			default_height = atachments[0].tex_view->texture->height;
		}
		else if (viewport_info.width != FLT_MAX) {
			default_width = (uint32_t)viewport_info.width;
			default_height = (uint32_t)viewport_info.height;
		}
		else if (scissor_info.extent.width != 0xFFFF'FFFF) {
			default_width = scissor_info.extent.width;
			default_height = scissor_info.extent.height;
		}
		else {
			default_width = 0;
			default_height = 0;
		}

		if (viewport_info.width == FLT_MAX) {
			viewport_info.width = (float)default_width;
			viewport_info.height = (float)default_height;
		}

		if (scissor_info.extent.width == 0xFFFF'FFFF) {
			scissor_info.extent.width = default_width;
			scissor_info.extent.height = default_height;
		}
	}

	// Renderpass
	{
		subpass_desc.colorAttachmentCount = (uint32_t)color_attach_refs.size();
		subpass_desc.pColorAttachments = color_attach_refs.data();

		VkRenderPassCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = (uint32_t)attach_descps.size();
		info.pAttachments = attach_descps.data();
		info.subpassCount = 1;
		info.pSubpasses = &subpass_desc;
		info.dependencyCount = 0;
		info.pDependencies = nullptr;

		if (vkCreateRenderPass(device->device, &info, nullptr, &renderpass) != VK_SUCCESS) {
			throw;
		}
	}

	// Framebuffer
	{
		VkFramebufferCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		info.renderPass = renderpass;
		info.attachmentCount = (uint32_t)views.size();
		info.pAttachments = views.data();
		info.width = default_width;
		info.height = default_height;
		info.layers = 1;

		if (vkCreateFramebuffer(device->device, &info, nullptr, &framebuffer) != VK_SUCCESS) {
			throw;
		}
	}

	// Pipeline Layout
	if (vkCreatePipelineLayout(device->device, &pipe_layout_info, nullptr, &pipe_layout) != VK_SUCCESS) {
		throw;
	}

	// Pipeline
	{
		// Blend State
		blend_state_info.attachmentCount = (uint32_t)blend_attachments_infos.size();
		blend_state_info.pAttachments = blend_attachments_infos.data();

		VkGraphicsPipelineCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		info.stageCount = (uint32_t)stage_infos.size();
		info.pStages = stage_infos.data();
		info.pVertexInputState = &vertex_input_info;
		info.pInputAssemblyState = &input_assembly_info;
		info.pViewportState = &viewport_state_info;
		info.pRasterizationState = &rasterizer_info;
		info.pMultisampleState = &multisample_info;
		info.pDepthStencilState = nullptr; // Optional
		info.pColorBlendState = &blend_state_info;
		info.pDynamicState = nullptr; // Optional
		info.layout = pipe_layout;
		info.renderPass = renderpass;
		info.subpass = 0;
		info.basePipelineHandle = nullptr; // Optional
		info.basePipelineIndex = -1; // Optional

		if (vkCreateGraphicsPipelines(device->device, nullptr, 1, &info, nullptr, &pipeline) != VK_SUCCESS) {
			throw;
		}
	}
}

void RasterizerPipeline::_recordCommands(CommandBuffer* cmd_buff)
{
	std::vector<VkClearValue> clear_values;
	for (uint32_t i = 0; i < atachments.size(); i++) {

		auto& clear_value = clear_values.emplace_back();
		clear_value.color.float32[0] = 0;
		clear_value.color.float32[1] = 0;
		clear_value.color.float32[2] = 0;
		clear_value.color.float32[3] = 0;

		clear_value.color.int32[0] = 0;
		clear_value.color.int32[1] = 0;
		clear_value.color.int32[2] = 0;
		clear_value.color.int32[3] = 0;

		clear_value.color.uint32[0] = 0;
		clear_value.color.uint32[1] = 0;
		clear_value.color.uint32[2] = 0;
		clear_value.color.uint32[3] = 0;

		clear_value.depthStencil.depth = 0;
		clear_value.depthStencil.stencil = 0;
	}

	VkRenderPassBeginInfo renderpass_begin_info{};
	renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderpass_begin_info.renderPass = renderpass;
	renderpass_begin_info.framebuffer = framebuffer;
	renderpass_begin_info.renderArea.offset.x = 0;
	renderpass_begin_info.renderArea.offset.y = 0;
	renderpass_begin_info.renderArea.extent.width = default_width;
	renderpass_begin_info.renderArea.extent.height = default_height;
	renderpass_begin_info.clearValueCount = (uint32_t)clear_values.size();
	renderpass_begin_info.pClearValues = clear_values.data();

	vkCmdBeginRenderPass(cmd_buff->cmd_buff, &renderpass_begin_info, VK_SUBPASS_CONTENTS_INLINE);
	{
		vkCmdBindPipeline(cmd_buff->cmd_buff, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		vkCmdDraw(cmd_buff->cmd_buff, 3, 1, 0, 0);
	}
	vkCmdEndRenderPass(cmd_buff->cmd_buff);
}

RasterizerPipeline::~RasterizerPipeline()
{
	if (device != nullptr) {
		vkDestroyPipeline(device->device, pipeline, nullptr);
		vkDestroyPipelineLayout(device->device, pipe_layout, nullptr);
		vkDestroyFramebuffer(device->device, framebuffer, nullptr);
		vkDestroyRenderPass(device->device, renderpass, nullptr);
	}
}
