#include "Context.hpp"

// Standard
#include <algorithm>
#include <format>


void checkDX12(HRESULT result)
{
	if (result != S_OK) {
		__debugbreak();
	}
}

void Context::init()
{
	// Debug Controller
	ComPtr<ID3D12Debug> debug_controller;
	{
		if (D3D12GetDebugInterface(IID_PPV_ARGS(debug_controller.GetAddressOf())) != S_OK) {
			__debugbreak();
		}

		debug_controller->EnableDebugLayer();
	}

	// Factory
	{
		ComPtr<IDXGIFactory2> factory_2;
		if (CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(factory_2.GetAddressOf())) != S_OK) {
			__debugbreak();
		}

		if (factory_2->QueryInterface(factory.GetAddressOf()) != S_OK) {
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
			return left.desc.DedicatedVideoMemory > right.desc.DedicatedVideoMemory;
		});
	}
	auto& adapter = adapters.front().adapter;

	// Device
	{
		if (D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(dev.GetAddressOf())) != S_OK) {
			__debugbreak();
		}
		dev->SetName(L"Device");
		dev->QueryInterface(IID_PPV_ARGS(&debug_device));
	}
	
	// Command Queue
	{
		D3D12_COMMAND_QUEUE_DESC desc = {};
		desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		checkDX12(dev->CreateCommandQueue(&desc, IID_PPV_ARGS(cmd_queue.GetAddressOf())));
	}

	// Command Allocator
	checkDX12(dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmd_alloc))
	);

	// Command List
	{
		checkDX12(dev->CreateCommandList(0,
			D3D12_COMMAND_LIST_TYPE_DIRECT, cmd_alloc.Get(), nullptr,
			IID_PPV_ARGS(&cmd_list)
		));

		cmd_list->Close();
	}

	// Sync
	{
		checkDX12(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&cmd_fence)));
	}

	// HLSL Compiler
	{
		checkDX12(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&hlsl_utils)));

		checkDX12(hlsl_utils->CreateDefaultIncludeHandler(hlsl_include_handler.ReleaseAndGetAddressOf()));

		checkDX12(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&hlsl_compiler)));
	}
}

void Context::beginCommandList()
{
	checkDX12(cmd_alloc->Reset());
	checkDX12(cmd_list->Reset(cmd_alloc.Get(), nullptr));

	is_cmd_list_recording = true;
}

void Context::endCommandList()
{
	checkDX12(cmd_list->Close());

	// reset the fence
	checkDX12(cmd_fence->Signal(0));

	std::array<ID3D12CommandList*, 1> command_lists = {
		cmd_list.Get()
	};
	cmd_queue->ExecuteCommandLists((uint32_t)command_lists.size(), command_lists.data());
}

void Context::waitForCommandList()
{
	// make the command queue set the fence to 1 when done
	checkDX12(cmd_queue->Signal(cmd_fence.Get(), 1));

	// check for curent fence value
	while (cmd_fence->GetCompletedValue() == 0) {

	}

	is_cmd_list_recording = false;
}

void Context::endAndWaitForCommandList()
{
	endCommandList();
	waitForCommandList();
}

void Context::copyBuffer(ID3D12Resource* dest, ID3D12Resource* source)
{
	auto dest_desc = dest->GetDesc();

	beginCommandList();
	{
		cmd_list->CopyBufferRegion(
			dest, 0,
			source, 0,
			dest_desc.Width
		);
	}
	endAndWaitForCommandList();
}

void Context::reportLiveObjects()
{
	checkDX12(debug_device->ReportLiveDeviceObjects(D3D12_RLDO_DETAIL));
}
