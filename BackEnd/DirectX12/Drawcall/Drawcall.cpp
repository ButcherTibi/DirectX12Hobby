#include "Drawcall.hpp"

#include <format>


void Drawcall::build()
{
	assertCmdListUnset();

	// Build Root Signature
	{
		D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
		root_signature_desc.NumParameters = (uint32_t)params.size();
		root_signature_desc.pParameters = params.data();
		root_signature_desc.NumStaticSamplers = 0;
		root_signature_desc.pStaticSamplers = nullptr;
		root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> err_blob;

		checkDX12(D3D12SerializeRootSignature(
			&root_signature_desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,
			blob.GetAddressOf(),
			err_blob.GetAddressOf()
		));

		checkDX12(Context::dev->CreateRootSignature(0,
			blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(root_signature.GetAddressOf())
		));
	}

	// Generate Root Signature
	{
		hlsl_root_signature = "[RootSignature(\"";

		for (uint32_t i = 0; i < params.size(); i++) {

			auto& param = params[i];

			switch (param.ParameterType) {
			case D3D12_ROOT_PARAMETER_TYPE_SRV: {
				hlsl_root_signature += std::format("SRV(t{})", param.Descriptor.ShaderRegister);
				break;
			}
			default: __debugbreak();
			}

			if (i != params.size() - 1) {
				hlsl_root_signature += ", \n";
			}
		}

		hlsl_root_signature += "\")]";

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
			Context::dev->CreateGraphicsPipelineState(&pipe_desc, IID_PPV_ARGS(pipeline.GetAddressOf()))
		);
	}
}

void Drawcall::assertCmdListUnset()
{
	if (cmd_list != nullptr) {
		__debugbreak();
	}
}

void Drawcall::assertCmdListSet()
{
	if (cmd_list == nullptr) {
		__debugbreak();
	}
}

void Drawcall::init()
{
	assertCmdListUnset();

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

void Drawcall::setShaderResourceViewParam(uint32_t shader_register, D3D12_SHADER_VISIBILITY shader_visibility)
{
	assertCmdListUnset();

	auto& new_param = params.emplace_back();
	new_param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	new_param.Descriptor.ShaderRegister = shader_register;
	new_param.Descriptor.RegisterSpace = 0;
	new_param.ShaderVisibility = shader_visibility;
}

void Drawcall::setVertexShader(VertexShader* new_vertex_shader)
{
	assertCmdListUnset();
	this->vertex_shader = new_vertex_shader;
}

void Drawcall::setPixelShader(PixelShader* new_pixel_shader)
{
	assertCmdListUnset();
	this->pixel_shader = new_pixel_shader;
}
