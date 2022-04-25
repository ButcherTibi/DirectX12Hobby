#pragma once

// Standard
#include <string>
#include <vector>
#include <array>

// Windows
#include <wrl/client.h>
using Microsoft::WRL::ComPtr;

// DXGI
#include <dxgi1_6.h>
#include <dxgidebug.h>

// DirectX 12
#include <d3d12.h>
#include <D3Dcompiler.h>

// PIX Debugger
#include <pix3.h>

// Mine
#include <ButchersToolbox/Windows/WindowsSpecific.hpp>


void checkDX12(HRESULT result);

struct Adapter {
	uint32_t index;

	ComPtr<IDXGIAdapter> adapter;
	DXGI_ADAPTER_DESC desc;
};

class Context {
public:
	// PIX
	inline static bool is_pix_debugger_enabled = false;
	inline static bool pix_capture_started = false;

	// Renderer
	inline static std::vector<Adapter> adapters;
	inline static ComPtr<ID3D12Device> dev;
	inline static ComPtr<ID3D12CommandQueue> cmd_queue;
	inline static ComPtr<ID3D12CommandAllocator> cmd_alloc;
	inline static ComPtr<ID3D12GraphicsCommandList> cmd_list;
	inline static ComPtr<ID3D12Fence> cmd_fence;
	// inline static win32::Handle complete_event;

public:
	// PIX Debugger
	static void initPix();
	static void beginPixCapture(std::wstring filename = L"ProgramaticPixCapture");
	static void endPixCapture();

	static void init();

	static void beginCommandList();
	static void endAndWaitForCommandList();
};
