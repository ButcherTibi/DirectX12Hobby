#include "Drawcall.hpp"

#include <format>


void Drawcall::create(Context* new_context)
{
	context = new_context;

	// Pipeline
	{
		pipe_desc = {};

		// Input Layout
		pipe_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipe_desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		// Rasterizer State
		{
			auto& rasterizer = pipe_desc.RasterizerState;
			rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
			rasterizer.CullMode = D3D12_CULL_MODE_NONE;
			rasterizer.FrontCounterClockwise = false;
			rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
			rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
			rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
			rasterizer.DepthClipEnable = false;
			rasterizer.MultisampleEnable = false;
			rasterizer.AntialiasedLineEnable = false;
			rasterizer.ForcedSampleCount = 0;
			rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
		}

		// Depth Stencil
		{
			auto& depth = pipe_desc.DepthStencilState;
			depth.DepthEnable = false;
			depth.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			depth.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			depth.StencilEnable = false;
		}

		// Sample
		{
			pipe_desc.SampleMask = UINT_MAX;
			pipe_desc.SampleDesc.Quality = 0;
			pipe_desc.SampleDesc.Count = 1;
		}

		// Blend State
		{
			auto& blend = pipe_desc.BlendState;

			blend.AlphaToCoverageEnable = false;
			blend.IndependentBlendEnable = false;

			D3D12_RENDER_TARGET_BLEND_DESC no_blend = {};
			no_blend.BlendEnable = false;
			no_blend.LogicOpEnable = false;
			no_blend.SrcBlend = D3D12_BLEND_ONE;
			no_blend.DestBlend = D3D12_BLEND_ZERO;
			no_blend.BlendOp = D3D12_BLEND_OP_ADD;
			no_blend.SrcBlendAlpha = D3D12_BLEND_ONE;
			no_blend.DestBlendAlpha = D3D12_BLEND_ZERO;
			no_blend.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			no_blend.LogicOp = D3D12_LOGIC_OP_NOOP;
			no_blend.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			blend.RenderTarget[0] = no_blend;
		}

		pipe_desc.NumRenderTargets = 1;
		pipe_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		pipe_desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		pipe_desc.NodeMask = 0;
		pipe_desc.CachedPSO.pCachedBlob = nullptr;
		pipe_desc.CachedPSO.CachedBlobSizeInBytes = 0;
		pipe_desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	}

	// Viewport
	{
		viewport = {};
		viewport.Width = (float)0xFFFF'FFFF;
		viewport.Height = (float)0xFFFF'FFFF;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;
	}

	// Scissor
	{
		scissor = {};
		scissor.right = 0xFFFF'FFFF;
		scissor.bottom = 0xFFFF'FFFF;
	}
}

void Drawcall::setVertexShader(VertexShader* new_vertex_shader)
{
	this->vertex_shader = new_vertex_shader;
}

void Drawcall::setPixelShader(PixelShader* new_pixel_shader)
{
	this->pixel_shader = new_pixel_shader;
}

void Drawcall::build()
{
	// Generate Root Signature
	{
		std::string hlsl_root_signature = CallBase::buildRootSiganture();

		vertex_shader->compile(hlsl_root_signature);
		pixel_shader->compile(hlsl_root_signature);
	}

	// Pipeline
	{
		pipe_desc.pRootSignature = root_signature.Get();
		pipe_desc.VS.BytecodeLength = vertex_shader->cso->GetBufferSize();
		pipe_desc.VS.pShaderBytecode = vertex_shader->cso->GetBufferPointer();
		pipe_desc.PS.BytecodeLength = pixel_shader->cso->GetBufferSize();
		pipe_desc.PS.pShaderBytecode = pixel_shader->cso->GetBufferPointer();

		checkDX12(
			context->dev->CreateGraphicsPipelineState(&pipe_desc, IID_PPV_ARGS(pipeline.GetAddressOf()))
		);
	}
}

void Drawcall::CMD_bind()
{
	context->cmd_list->SetGraphicsRootSignature(root_signature.Get());
	context->cmd_list->SetPipelineState(pipeline.Get());
}

void Drawcall::CMD_setShaderResourceView(uint32_t shader_register, Resource* resource)
{
	for (uint32_t i = 0; i < params.size(); i++) {
		auto& param = params[i];

		if (param.ParameterType == D3D12_ROOT_PARAMETER_TYPE_SRV &&
			param.Descriptor.ShaderRegister == shader_register)
		{
			context->cmd_list->SetGraphicsRootShaderResourceView(i, resource->gpu_adress());
			return;
		}
	}

	__debugbreak();
}

void Drawcall::CMD_setIndexBuffer(IndexBuffer& index_buffer)
{
	D3D12_INDEX_BUFFER_VIEW view = {};
	view.BufferLocation = index_buffer.gpu_adress();
	view.SizeInBytes = index_buffer.mem_size();
	view.Format = DXGI_FORMAT_R32_UINT;

	context->cmd_list->IASetIndexBuffer(&view);

	this->index_buff_count = index_buffer.count();
}

void Drawcall::CMD_setViewportSize(float width, float height)
{
	viewport.Width = width;
	viewport.Height = height;
}

void Drawcall::CMD_setViewportSize(uint32_t width, uint32_t height)
{
	CMD_setViewportSize((float)width, (float)height);
}

void Drawcall::CMD_setRenderTargets(std::vector<DescriptorHandle> render_targets)
{
	std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 8> handles;

	for (uint32_t i = 0; i < render_targets.size(); i++) {
		handles[i] = render_targets[i].cpu_handle;
	}

	context->cmd_list->OMSetRenderTargets((uint32_t)render_targets.size(), handles.data(), false, nullptr);
}

void Drawcall::CMD_clearRenderTarget(DescriptorHandle render_target, float red, float green, float blue, float alpha)
{
	const float clearColor[] = { red, green, blue, alpha };
	context->cmd_list->ClearRenderTargetView(render_target.cpu_handle, clearColor, 0, nullptr);
}

void Drawcall::CMD_draw(uint32_t vertex_count, uint32_t instance_count)
{
	// Input Assembly
	context->cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Viewport
	context->cmd_list->RSSetViewports(1, &viewport);

	// Scissors
	{
		if (scissor.right == 0xFFFF'FFFF) {
			scissor.right = (uint32_t)viewport.Width;
			scissor.bottom = (uint32_t)viewport.Height;
		}

		context->cmd_list->RSSetScissorRects(1, &scissor);
	}

	context->cmd_list->DrawInstanced(vertex_count, instance_count, 0, 0);
}

void Drawcall::CMD_drawIndexed(uint32_t instance_count)
{
	// Input Assembly
	context->cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Viewport
	context->cmd_list->RSSetViewports(1, &viewport);

	// Scissors
	{
		if (scissor.right == 0xFFFF'FFFF) {
			scissor.right = (uint32_t)viewport.Width;
			scissor.bottom = (uint32_t)viewport.Height;
		}

		context->cmd_list->RSSetScissorRects(1, &scissor);
	}

	if (index_buff_count != 0xFFFF'FFFF) {
		context->cmd_list->DrawIndexedInstanced(index_buff_count, instance_count, 0, 0, 0);
	}
	else {
		__debugbreak();
	}
}
