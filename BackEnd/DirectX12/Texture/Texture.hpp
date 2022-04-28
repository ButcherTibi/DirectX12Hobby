#pragma once

#include <DirectX12/Resource/Resource.hpp>


class Texture : public Resource {
public:
	void createTexture(Context* context, uint32_t width, uint32_t height, DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
		std::array<float, 4>& clear_color, float clear_depth, uint8_t clear_stencil);

	void createRenderTarget(Context* context,
		uint32_t width, uint32_t height, DXGI_FORMAT format,
		std::array<float, 4> clear_color = {0.f, 0.f, 0.f, 0.f},
		float clear_depth = 0.f, uint8_t clear_stencil = 0);
};
