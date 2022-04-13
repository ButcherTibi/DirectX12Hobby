#include "DX12.hpp"

#include <algorithm>


HelloTriangle hello_world;


void checkDX12(HRESULT result)
{
	if (result != S_OK) {
		__debugbreak();
	}
}

void HelloTriangle::init()
{
	this->render_width = 1024;
	this->render_height = 720;

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

	// Create Image
	{
		createRenderTarget(render_width, render_height, DXGI_FORMAT_R8G8B8A8_UNORM, final_rtv);
	}

	// Descriptor Heap
	{
		// Describe and create a render target view (RTV) descriptor heap.
		D3D12_DESCRIPTOR_HEAP_DESC info = {};
		info.NumDescriptors = 1;
		info.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
		info.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

		checkDX12(dev->CreateDescriptorHeap(&info, IID_PPV_ARGS(rtv_heap.GetAddressOf())));
	}

	// Render Target View
	{
		final_rtv.createRTV(rtv_heap->GetCPUDescriptorHandleForHeapStart());
	}

	// Create Root Signature
	{
		D3D12_ROOT_SIGNATURE_DESC root_signature_desc = {};
		root_signature_desc.NumParameters = 0;
		root_signature_desc.pParameters = nullptr;
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

		checkDX12(dev->CreateRootSignature(
			0, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(root_sign.GetAddressOf())
		));
	}

	// Shader
	{
		shader.createFromSourceCode(L"G:/My work/DreamGraphicsAPI/BackEnd/shader.hlsl");
	}

	// Pipeline State Objects
	{
		D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		};

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = root_sign.Get();
		
		// Input Layout
		desc.InputLayout = { inputElementDescs, 2 };
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

	// Command List
	{
		checkDX12(
			dev->CreateCommandList(0,
				D3D12_COMMAND_LIST_TYPE_DIRECT, cmd_alloc.Get(), pipeline.Get(),
				IID_PPV_ARGS(cmd_list.GetAddressOf())
			)
		);

		cmd_list->Close();
	}

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT4 color;
	};

	Vertex verts[] =
	{
		{ { 0.0f, 0.25f * 1, 0.0f }, { 1.0f, 0.0f, 0.0f, 1.0f } },
		{ { 0.25f, -0.25f * 1, 0.0f }, { 0.0f, 1.0f, 0.0f, 1.0f } },
		{ { -0.25f, -0.25f * 1, 0.0f }, { 0.0f, 0.0f, 1.0f, 1.0f } }
	};

	uint32_t vertex_buff_size = (uint32_t)sizeof(verts);

	// Vertex Buffer
	{
		D3D12_HEAP_PROPERTIES heap_props = {};
		heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		D3D12_RESOURCE_DESC res_desc = {};
		res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		res_desc.Width = vertex_buff_size;
		res_desc.Height = 1;
		res_desc.DepthOrArraySize = 1;
		res_desc.MipLevels = 1;
		res_desc.Format = DXGI_FORMAT_UNKNOWN;
		res_desc.SampleDesc.Count = 1;
		res_desc.SampleDesc.Quality = 0;
		res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

		checkDX12(dev->CreateCommittedResource(
			&heap_props,
			D3D12_HEAP_FLAG_NONE,
			&res_desc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(vbuff.GetAddressOf())));
	}

	// Load data into vertex buffer
	{
		D3D12_RANGE no_read = {};
		no_read.Begin = 0;
		no_read.End = 0;

		void* gpu_vbuff_ptr;

		checkDX12(vbuff->Map(0, &no_read, &gpu_vbuff_ptr));
		{
			std::memcpy(gpu_vbuff_ptr, verts, vertex_buff_size);
		}

		D3D12_RANGE write_all = {};
		write_all.Begin = 0;
		write_all.End = vertex_buff_size;
		vbuff->Unmap(0, &write_all);
	}

	// Vertex Buffer View
	{
		vbuff_view = {};
		vbuff_view.BufferLocation = vbuff->GetGPUVirtualAddress();
		vbuff_view.StrideInBytes = sizeof(Vertex);
		vbuff_view.SizeInBytes = vertex_buff_size;
	}

	// Fence
	{
		checkDX12(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf())));
	}

	checkDX12(cmd_alloc->Reset());
	checkDX12(cmd_list->Reset(cmd_alloc.Get(), pipeline.Get()));
	{
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
			rtv_heap->GetCPUDescriptorHandleForHeapStart()
		};

		cmd_list->OMSetRenderTargets(
			(uint32_t)tr.size(), tr.data(),
			false,
			nullptr
		);

		// Record commands.
		const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
		cmd_list->ClearRenderTargetView(rtv_heap->GetCPUDescriptorHandleForHeapStart(), clearColor, 0, nullptr);

		cmd_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		cmd_list->IASetVertexBuffers(0, 1, &vbuff_view);
		cmd_list->DrawInstanced(3, 1, 0, 0);
	}
	checkDX12(cmd_list->Close());

	std::array<ID3D12CommandList*, 1> command_lists = {
		cmd_list.Get()
	};

	cmd_queue->ExecuteCommandLists((uint32_t)command_lists.size(), command_lists.data());

	endPixCapture();
}

void HelloTriangle::render()
{
	
}

void HelloTriangle::destroy()
{
	dev = nullptr;
}
