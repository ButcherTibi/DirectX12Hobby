#include <DirectX12/Context/Context.hpp>

// Standard
#include <algorithm>
#include <format>


void checkDX12(HRESULT result)
{
	if (result != S_OK) {
		__debugbreak();
	}
}

void Context::create()
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
	ComPtr<IDXGIFactory6> factory;
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
}

void Context::beginCommandList()
{
	checkDX12(cmd_alloc->Reset());
	checkDX12(cmd_list->Reset(cmd_alloc.Get(), nullptr));
}

void Context::endAndWaitForCommandList()
{
	checkDX12(cmd_list->Close());

	checkDX12(cmd_fence->Signal(0));

	std::array<ID3D12CommandList*, 1> command_lists = {
		cmd_list.Get()
	};
	cmd_queue->ExecuteCommandLists((uint32_t)command_lists.size(), command_lists.data());

	checkDX12(cmd_queue->Signal(cmd_fence.Get(), 1));

	while (cmd_fence->GetCompletedValue() == 0) {

	}
}

void Context::copy(ID3D12Resource* dest, ID3D12Resource* source)
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
