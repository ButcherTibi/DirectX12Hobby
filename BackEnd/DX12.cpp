#include "DX12.hpp"

#include <algorithm>

#include "App.hpp"


Renderer renderer;


void checkDX12(HRESULT result)
{
	if (result != S_OK) {
		__debugbreak();
	}
}

void Renderer::beginCommandList()
{
	checkDX12(cmd_alloc->Reset());
	checkDX12(cmd_list->Reset(cmd_alloc.Get(), nullptr));
}

void Renderer::endCommandList()
{
	checkDX12(cmd_list->Close());

	std::array<ID3D12CommandList*, 1> command_lists = {
		cmd_list.Get()
	};
	cmd_queue->ExecuteCommandLists((uint32_t)command_lists.size(), command_lists.data());
}

void Renderer::init()
{
	beginPixCapture();
	
	// Debug Controller
	ComPtr<ID3D12Debug> debug_controller;
	{
		if (D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf())) != S_OK) {
			__debugbreak();
		}

		debug_controller->EnableDebugLayer();
	}

	// Factory
	ComPtr<IDXGIFactory6> factory;
	{
		ComPtr<IDXGIFactory> factory_1;
		if (CreateDXGIFactory(IID_PPV_ARGS(factory_1.GetAddressOf())) != S_OK) {
			__debugbreak();
		}

		if (factory_1->QueryInterface(factory.GetAddressOf()) != S_OK) {
			__debugbreak();
		}
	}

	// Adapter
	{
		IDXGIAdapter* found_adapter;

		while (factory->EnumAdapterByGpuPreference(
			(uint32_t)adapters.size(), DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&found_adapter)) == S_OK) {

			auto& adapter = adapters.emplace_back();
			adapter.index = (uint32_t)adapters.size() - 1;
			adapter.adapter = found_adapter;

			found_adapter->GetDesc(&adapter.desc);
		}

		std::sort(adapters.begin(), adapters.end(), [](Adapter& left, Adapter& right) -> bool {
			return left.desc.DedicatedVideoMemory < right.desc.DedicatedVideoMemory;
		});
	}
	auto& adapter = adapters.front().adapter;

	// Device
	if (D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&dev)) != S_OK) {
		__debugbreak();
	}

	// Info Queue
	//ComPtr<ID3D12InfoQueue> info_queue;
	//{
	//	checkDX12(debug_controller->QueryInterface(IID_PPV_ARGS(&info_queue)));

	//	std::array<D3D12_MESSAGE_ID, 1> black_list = {
	//		// custom clear values are aparently wrong ?
	//		D3D12_MESSAGE_ID_CLEARRENDERTARGETVIEW_MISMATCHINGCLEARVALUE
	//	};

	//	D3D12_INFO_QUEUE_FILTER filter = {};
	//	filter.DenyList.NumIDs = (uint32_t)black_list.size();
	//	filter.DenyList.pIDList = black_list.data();

	//	checkDX12(info_queue->AddStorageFilterEntries(&filter));
	//}

	// Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		checkDX12(dev->CreateCommandQueue(&desc, IID_PPV_ARGS(cmd_queue.GetAddressOf())));
	}

	// Command Allocator
	checkDX12(dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(cmd_alloc.GetAddressOf()))
	);

	// Command List
	checkDX12(dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT, cmd_alloc.Get(), nullptr,
		IID_PPV_ARGS(cmd_list.GetAddressOf())
	));

	cmd_list->Close();

	// Descriptor Heaps
	{
		cbv_srv_uav_heap.init(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);
		rtv_heap.init(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	}

	// Final Texture
	{
		createRenderTarget(render_width, render_height, DXGI_FORMAT_R8G8B8A8_UNORM, final_rt);
		final_rtv = rtv_heap.addRenderTargetView(final_rt);
	}

	// Create Root Signature
	{
		std::array<D3D12_ROOT_PARAMETER, 1> params;
		{
			params[0] = {};
			params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
			params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
		}

		D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
		root_signature_desc.NumParameters = 0;
		root_signature_desc.pParameters = nullptr;
		root_signature_desc.NumStaticSamplers = 0;
		root_signature_desc.pStaticSamplers = nullptr;
		root_signature_desc.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;

		ComPtr<ID3DBlob> blob;
		ComPtr<ID3DBlob> err_blob;

		checkDX12(D3D12SerializeRootSignature(
			&root_signature_desc,
			D3D_ROOT_SIGNATURE_VERSION_1_0,
			blob.GetAddressOf(),
			err_blob.GetAddressOf()
		));

		checkDX12(dev->CreateRootSignature(
			0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(root_sign.GetAddressOf())
		));
	}

	// Shader
	{
		shader.createFromSourceCode(L"G:/My work/DirectX12Hobby/BackEnd/shader.hlsl");
	}

	// Pipeline State Objects
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = root_sign.Get();
		
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
			dev->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(pipeline.GetAddressOf()))
		);
	}

	// Fence
	{
		checkDX12(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
	}

	render();

	endPixCapture();
}

void Renderer::render()
{
	auto& mesh = app.mesh;

	if (mesh.modified_verts.size() > mesh.verts.size()) {


	}

	// Command List
	checkDX12(cmd_alloc->Reset());
	checkDX12(cmd_list->Reset(cmd_alloc.Get(), nullptr));
	{
		cmd_list->SetPipelineState(pipeline.Get());
		cmd_list->SetGraphicsRootSignature(root_sign.Get());

		D3D12_VIEWPORT viewport = {};
		viewport.Width = (float)render_width;
		viewport.Height = (float)render_height;
		viewport.MinDepth = 0;
		viewport.MaxDepth = 1;

		cmd_list->RSSetViewports(1, &viewport);

		D3D12_RECT scissor = {};
		scissor.right = render_width;
		scissor.bottom = render_height;

		cmd_list->RSSetScissorRects(1, &scissor);

		// Texture
		{
			D3D12_RESOURCE_BARRIER barrier = {};
			barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			// barrier.Transition.pResource = texture.Get();
			barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON;
			barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;

			// cmd_list->ResourceBarrier(1, &barrier);
		}

		std::array<D3D12_CPU_DESCRIPTOR_HANDLE, 1> tr = {
			final_rtv.handle
		};

		cmd_list->OMSetRenderTargets(
			(uint32_t)tr.size(), tr.data(),
			false,
			nullptr
		);

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		cmd_list->ClearRenderTargetView(final_rtv.handle, clearColor, 0, nullptr);

		cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		// cmd_list->SetGraphicsRootShaderResourceView();
		cmd_list->DrawInstanced(3, 1, 0, 0);
	}
	checkDX12(cmd_list->Close());

	std::array<ID3D12CommandList*, 1> command_lists = {
		cmd_list.Get()
	};
	cmd_queue->ExecuteCommandLists((uint32_t)command_lists.size(), command_lists.data());
}

void Renderer::destroy()
{
	dev = nullptr;
}
