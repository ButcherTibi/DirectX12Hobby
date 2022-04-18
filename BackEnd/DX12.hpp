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


/* @TODO
 - create load geometry for loading geometry when mesh has more stuff
 - setup readback texture to print to screen
*/

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
	ComPtr<ID3DBlob> vertex_shader_errors = nullptr;
	ComPtr<ID3DBlob> pixel_shader_cso = nullptr;
	ComPtr<ID3DBlob> pixel_shader_errors = nullptr;

public:
	void createFromSourceCode(std::wstring file_path,
		uint16_t shader_stages = (uint16_t)ShaderStages::VERTEX | (uint16_t)ShaderStages::PIXEL);
};


class Texture {
public:
	ComPtr<ID3D12Resource> texture = nullptr;
};


struct CPU_DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE handle;

	CPU_DescriptorHandle() = default;
	CPU_DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE);
};

struct RTV_DescriptorHandle : public CPU_DescriptorHandle { };


class DescriptorHeap {
protected:
	ComPtr<ID3D12DescriptorHeap> heap;
	uint32_t used_count;

protected:
	CPU_DescriptorHandle at(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index = 1);

public:
	void init(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size = 128, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
};


class CBV_SRV_UAV_DescriptorHeap : public DescriptorHeap {
public:

};


class RTV_DescriptorHeap : public DescriptorHeap {
public:
	RTV_DescriptorHandle addRenderTargetView(Texture& texture);
};


class Renderer {
public:
	// PIX
	bool is_pix_debugger_enabled = false;
	bool pix_capture_started = false;

	// Renderer
	std::vector<Adapter> adapters;
	inline static ComPtr<ID3D12Device> dev;
	ComPtr<ID3D12CommandQueue> cmd_queue;
	ComPtr<ID3D12CommandAllocator> cmd_alloc;

	CBV_SRV_UAV_DescriptorHeap cbv_srv_uav_heap;
	RTV_DescriptorHeap rtv_heap;
	ComPtr<ID3D12RootSignature> root_sign;
	ComPtr<ID3D12PipelineState> pipeline;

	Shader shader;

	Texture final_rt;
	RTV_DescriptorHandle final_rtv;
	
	// ComPtr<ID3D12Resource> vbuff;
	// D3D12_VERTEX_BUFFER_VIEW vbuff_view;

	ComPtr<ID3D12GraphicsCommandList> cmd_list;

	ComPtr<ID3D12Fence> fence;

	uint32_t render_width = 1024;
	uint32_t render_height = 720;


public:
	// PIX Debugger
	void initPix();
	void beginPixCapture(std::wstring filename = L"ProgramaticPixCapture");
	void endPixCapture();

	// Resources
	void createTexture(uint32_t width, uint32_t height, DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state, Texture& r_texture);
	void createRenderTarget(uint32_t width, uint32_t height, DXGI_FORMAT format, Texture& r_texture);

	// Command List
	void beginCommandList();
	void endCommandList();

	// Render
	void init();
	void render();

	void destroy();
};
extern Renderer renderer;
