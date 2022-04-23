#include <DirectX12/Context/Context.hpp>

// Standard
#include <algorithm>

#include <ButchersToolbox/Windows/WindowsSpecific.hpp>


void checkDX12(HRESULT result)
{
	if (result != S_OK) {
		__debugbreak();
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

	std::array<ID3D12CommandList*, 1> command_lists = {
		cmd_list.Get()
	};
	cmd_queue->ExecuteCommandLists((uint32_t)command_lists.size(), command_lists.data());

	// wait for queue
	ComPtr<ID3D12Fence> fence;
	checkDX12(dev->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));

	cmd_queue->Signal(fence.Get(), 1);

	win32::Handle on_complete = CreateEvent(0, false, false, L"queue_complete");

	if (fence->GetCompletedValue() != 1) {
		fence->SetEventOnCompletion(1, on_complete.handle); // CHECK HRESULT
		WaitForSingleObject(on_complete.handle, 0xFFFF'FFFF);
	}
}

void Context::init()
{
	// beginPixCapture();

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

		checkDX12(dev->CreateCommandQueue(&desc, IID_PPV_ARGS(&cmd_queue)));
	}

	// Command Allocator
	checkDX12(dev->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&cmd_alloc))
	);

	// Command List
	checkDX12(dev->CreateCommandList(0,
		D3D12_COMMAND_LIST_TYPE_DIRECT, cmd_alloc.Get(), nullptr,
		IID_PPV_ARGS(&cmd_list)
	));

	cmd_list->Close();  // not sure if usefull
}