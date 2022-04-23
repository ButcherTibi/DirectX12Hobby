#include "Renderer.hpp"


void Renderer::init()
{
	Context::init();

	// Descriptor Heaps
	{
		cbv_srv_uav_heap.init();
		rtv_heap.init();
	}

	// Vertices
	{
		verts_sbuff.init();

		std::array<GPU_Vertex, 3> gpu_verts;
		gpu_verts[0].pos = { 0.0f, 0.25f, 0.0f };
		gpu_verts[1].pos = { 0.25f, -0.25f, 0.0f };
		gpu_verts[2].pos = { -0.25f, -0.25f, 0.0f };

		verts_sbuff.load(gpu_verts.data(), gpu_verts.size() * sizeof(GPU_Vertex));

		verts_srv = cbv_srv_uav_heap.createShaderResourceView(0, verts_sbuff);
	}

	// Final Texture
	{
		final_rt.createRenderTarget(render_width, render_height, DXGI_FORMAT_R8G8B8A8_UNORM);

		final_rtv = rtv_heap.createRenderTargetView(0, final_rt);
	}

	// Create Root Signature
	{
		root_sign.setShaderResourceViewParam(0, 0);
		root_sign.build();
	}

	// Shader
	{
		shader.createFromSourceCode(L"G:/My work/DirectX12Hobby/BackEnd/Shaders/shader.hlsl");
	}

	// Pipeline State Objects
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = root_sign.get();
		
		// Input Layout
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

		desc.VS.BytecodeLength = shader.vertex_shader_cso->GetBufferSize();
		desc.VS.pShaderBytecode = shader.vertex_shader_cso->GetBufferPointer();
		desc.PS.BytecodeLength = shader.pixel_shader_cso->GetBufferSize();
		desc.PS.pShaderBytecode = shader.pixel_shader_cso->GetBufferPointer();

		// Rasterizer State
		{
			desc.RasterizerState = {};
			auto& rasterizer = desc.RasterizerState;
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
			desc.DepthStencilState = {};
			auto& depth = desc.DepthStencilState;
			depth.DepthEnable = false;
			depth.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
			depth.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			depth.StencilEnable = false;

			desc.DSVFormat = DXGI_FORMAT_UNKNOWN;
		}

		// Sample
		{
			desc.SampleMask = UINT_MAX;
			desc.SampleDesc.Quality = 0;
			desc.SampleDesc.Count = 1;
		}

		// Blend State
		{
			desc.BlendState = {};
			auto& blend = desc.BlendState;

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

		// Render Targets
		{
			desc.NumRenderTargets = 1;
			desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		}
		
		desc.NodeMask = 0;
		desc.CachedPSO.pCachedBlob = nullptr;
		desc.CachedPSO.CachedBlobSizeInBytes = 0;
		desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

		checkDX12(
			Context::dev->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipeline.GetAddressOf()))
		);
	}

	// Fence
	{
		checkDX12(Context::dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
	}

	Context::beginPixCapture();
	render();
	Context::endPixCapture();
}

void Renderer::render()
{
	// Command List
	Context::beginCommandList();
	{
		Context::cmd_list->SetGraphicsRootSignature(root_sign.get());
		Context::cmd_list->SetPipelineState(pipeline.Get());

		Context::cmd_list->SetGraphicsRootShaderResourceView(0, verts_sbuff.get()->GetGPUVirtualAddress());

		// Viewport
		{
			D3D12_VIEWPORT viewport = {};
			viewport.Width = (float)render_width;
			viewport.Height = (float)render_height;
			viewport.MinDepth = 0;
			viewport.MaxDepth = 1;

			Context::cmd_list->RSSetViewports(1, &viewport);
		}

		// Scissor
		{
			D3D12_RECT scissor = {};
			scissor.right = render_width;
			scissor.bottom = render_height;

			Context::cmd_list->RSSetScissorRects(1, &scissor);
		}

		// Render Targets
		{
			std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> rts = {
				final_rtv.cpu_handle
			};

			Context::cmd_list->OMSetRenderTargets(
				(uint32_t)rts.size(), rts.data(),
				false,
				nullptr
			);
		}

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		Context::cmd_list->ClearRenderTargetView(final_rtv.cpu_handle, clearColor, 0, nullptr);

		Context::cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		Context::cmd_list->DrawInstanced(3, 1, 0, 0);
	}
	Context::endAndWaitForCommandList();
}
