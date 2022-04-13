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
#include <DirectXMath.h>

// PIX Debugger
#include <pix3.h>

// Mine


void checkDX12(HRESULT result);


struct Adapter {
	uint32_t index;

	ComPtr<IDXGIAdapter> adapter;
	DXGI_ADAPTER_DESC desc;
};


enum class ShaderStages : uint16_t {
	VERTEX = 1 << 0,
	PIXEL = 1 << 1
};

class Shader {
public:
	ComPtr<ID3DBlob> vertex_shader_cso = nullptr;
	ComPtr<ID3DBlob> vertex_shader_errors;
	ComPtr<ID3DBlob> pixel_shader_cso;
	ComPtr<ID3DBlob> pixel_shader_errors;

public:
	void createFromSourceCode(std::wstring file_path,
		uint16_t shader_stages = (uint16_t)ShaderStages::VERTEX | (uint16_t)ShaderStages::PIXEL);
};

class Texture {
public:
	ComPtr<ID3D12Resource> texture = nullptr;

public:
	void createRTV(D3D12_CPU_DESCRIPTOR_HANDLE dest_descriptor);
};


class HelloTriangle {
public:
	// PIX
	inline static bool is_pix_debugger_enabled = false;
	bool pix_capture_started = false;

	// Renderer
	inline static std::vector<Adapter> adapters;
	inline static ComPtr<ID3D12Device> dev;
	inline static ComPtr<ID3D12CommandQueue> cmd_queue;
	inline static ComPtr<ID3D12CommandAllocator> cmd_alloc;

	inline static ComPtr<ID3D12DescriptorHeap> rtv_heap;
	ComPtr<ID3D12RootSignature> root_sign;
	ComPtr<ID3D12PipelineState> pipeline;

	Shader shader;
	Texture final_rtv;
	
	// ComPtr<ID3D12Resource> texture;
	ComPtr<ID3D12Resource> vbuff;
	D3D12_VERTEX_BUFFER_VIEW vbuff_view;

	ComPtr<ID3D12GraphicsCommandList> cmd_list;

	ComPtr<ID3D12Fence> fence;

	inline static uint32_t render_width;
	inline static uint32_t render_height;


public:
	// PIX Debugger
	void initPix();
	void beginPixCapture(std::wstring filename = L"ProgramaticPixCapture");
	void endPixCapture();

	// Resources
	void createTexture(uint32_t width, uint32_t height, DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state, Texture& r_texture);
	void createRenderTarget(uint32_t width, uint32_t height, DXGI_FORMAT format, Texture& r_texture);

	// Render
	void init();
	void render();

	void destroy();
};
extern HelloTriangle hello_world;
