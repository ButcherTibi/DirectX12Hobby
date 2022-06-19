#pragma once

// Standard
#include <string>
#include <vector>

// Windows
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// DXGI
#include <dxgi1_6.h>
#include <dxgidebug.h>

// DirectX 12
#include <d3d12.h>

// HLSL Compiler
#include <dxcapi.h>


class Context;
class DescriptorHeap;
struct RTV_DescriptorHandle;


void checkDX12(HRESULT result);

struct Adapter {
	uint32_t index;

	ComPtr<IDXGIAdapter> adapter;
	DXGI_ADAPTER_DESC desc;
};

class CommandList {
protected:
	Context* context = nullptr;

public:
	void setDescriptorHeaps(DescriptorHeap& heap_0);

	void clearRenderTarget(RTV_DescriptorHandle render_target,
		float red = 0.f, float green = 0.f, float blue = 0.f, float alpha = 0.f);
};

class RecordCommandList : public CommandList {
public:
	RecordCommandList() = default;
	RecordCommandList(Context* context);

	~RecordCommandList();
};

class RecordAndWaitCommandList : public CommandList {
public:
	RecordAndWaitCommandList() = default;
	RecordAndWaitCommandList(Context* context);	

	~RecordAndWaitCommandList();
};

class Context {
public:
	// PIX
	inline static bool is_pix_debugger_enabled = false;
	inline static bool pix_capture_started = false;

	// DirectX 12
	ComPtr<IDXGIFactory6> factory;
	std::vector<Adapter> adapters;
	ComPtr<ID3D12Device> dev = nullptr;
	ComPtr<ID3D12DebugDevice> debug_device;
	ComPtr<ID3D12CommandQueue> cmd_queue;
	ComPtr<ID3D12CommandAllocator> cmd_alloc;
	ComPtr<ID3D12GraphicsCommandList> cmd_list;
	ComPtr<ID3D12Fence> cmd_fence;
	bool is_cmd_list_recording = false;

	/// <summary>
	/// Used by <c>Resource</c> for uploading
	/// </summary>
	ComPtr<ID3D12Resource> upload_buff = nullptr;

	/// <summary>
	/// Used by <c>Resource</c> for downloading
	/// </summary>
	ComPtr<ID3D12Resource> download_buff = nullptr;

	// HLSL
	ComPtr<IDxcUtils> hlsl_utils;
	ComPtr<IDxcIncludeHandler> hlsl_include_handler;
	ComPtr<IDxcCompiler3> hlsl_compiler;

public:
	// PIX Debugger
	static void initPix();
	static void beginPixCapture(std::wstring filename = L"ProgramaticPixCapture");
	static void endPixCapture();


	/* API */

	void init();

	void beginCommandList();
	void endCommandList();
	void waitForCommandList();
	void endAndWaitForCommandList();
	void copyBuffer(ID3D12Resource* dest, ID3D12Resource* source);

	RecordCommandList recordCommandList();
	RecordAndWaitCommandList recordAndWaitCommandList();
	

	/* Debug */
	
	void reportLiveObjects();
};

class ScopedFrameCapture {
public:
	ScopedFrameCapture()
	{
		Context::beginPixCapture();
	}

	~ScopedFrameCapture() {
		Context::endPixCapture();
	}
};
