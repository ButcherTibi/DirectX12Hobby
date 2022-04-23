#include "Texture.hpp"


void Texture::createTexture(uint32_t width, uint32_t height, DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state)
{
	heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = flags;
	
	states = state;

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = format;
	clear_value.Color[0] = 0.f;
	clear_value.Color[1] = 0.f;
	clear_value.Color[2] = 0.f;
	clear_value.Color[3] = 0.f;
	clear_value.DepthStencil.Depth = 0.f;
	clear_value.DepthStencil.Stencil = 0u;

	checkDX12(Context::dev->CreateCommittedResource(
		&heap_props,
		heap_flags,
		&desc,
		states,
		&clear_value,
		IID_PPV_ARGS(&resource)));
}

void Texture::createRenderTarget(uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	createTexture(width, height, format,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET);
}
