#pragma once

#include <DirectX12/Resource/Resource.hpp>


class Texture : public Resource {
public:
	void createTexture(uint32_t width, uint32_t height, DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state);

	void createRenderTarget(uint32_t width, uint32_t height, DXGI_FORMAT format);
};
