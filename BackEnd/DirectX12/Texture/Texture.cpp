#include "Texture.hpp"


void Texture::createTexture(Context* new_context, uint32_t width, uint32_t height, DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
	std::array<float, 4>& new_clear_color, float new_clear_depth, uint8_t new_clear_stencil)
{
	context = new_context;

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
	clear_value.Color[0] = new_clear_color[0];
	clear_value.Color[1] = new_clear_color[1];
	clear_value.Color[2] = new_clear_color[2];
	clear_value.Color[3] = new_clear_color[3];
	clear_value.DepthStencil.Depth = new_clear_depth;
	clear_value.DepthStencil.Stencil = new_clear_stencil;

	checkDX12(context->dev->CreateCommittedResource(
		&heap_props,
		heap_flags,
		&desc,
		states,
		&clear_value,
		IID_PPV_ARGS(&resource)));
}

void Texture::createRenderTarget(Context* new_context,
	uint32_t width, uint32_t height, DXGI_FORMAT format,
	std::array<float, 4> new_clear_color, float new_clear_depth, uint8_t new_clear_stencil)
{
	createTexture(new_context, width, height, format,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET, D3D12_RESOURCE_STATE_RENDER_TARGET,
		new_clear_color, new_clear_depth, new_clear_stencil);
}
