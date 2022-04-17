#include "DX12.hpp"


void Renderer::createTexture(uint32_t width, uint32_t height, DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state, Texture& r_texture)
{
	D3D12_HEAP_PROPERTIES heap_props = {};
	heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_RESOURCE_DESC res_desc = {};
	res_desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	res_desc.Width = width;
	res_desc.Height = height;
	res_desc.DepthOrArraySize = 1;
	res_desc.MipLevels = 1;
	res_desc.Format = format;
	res_desc.SampleDesc.Count = 1;
	res_desc.SampleDesc.Quality = 0;
	res_desc.Flags = flags;

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = format;
	clear_value.Color[0] = 0.f;
	clear_value.Color[1] = 0.f;
	clear_value.Color[2] = 0.f;
	clear_value.Color[3] = 0.f;
	clear_value.DepthStencil.Depth = 0.f;
	clear_value.DepthStencil.Stencil = 0u;

	checkDX12(dev->CreateCommittedResource(
		&heap_props,
		D3D12_HEAP_FLAG_NONE,
		&res_desc,
		state,
		&clear_value,
		IID_PPV_ARGS(&r_texture.texture)));
}

void Renderer::createRenderTarget(uint32_t width, uint32_t height, DXGI_FORMAT format, Texture& r_texture)
{
	createTexture(width, height, format,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET,
		r_texture);
}
