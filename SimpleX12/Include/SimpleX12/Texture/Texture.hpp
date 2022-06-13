#pragma once

#include "../Resource/Resource.hpp"

struct RTV_DescriptorHandle;
class RTV_DescriptorHeap;


class Texture : public Resource {
	std::vector<RTV_DescriptorHandle> rtvs;

public:
	void createTexture(Context* context, uint32_t width, uint32_t height, DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state);

	void createRenderTarget(Context* context,
		uint32_t width, uint32_t height, DXGI_FORMAT format,
		std::array<float, 4> clear_color = {0.f, 0.f, 0.f, 0.f},
		float clear_depth = 0.f, uint8_t clear_stencil = 0);

	void createSwapchainRenderTarget(Context* context, ID3D12Resource* swapchain_backbuffer);

	RTV_DescriptorHandle createRenderTargetView(uint32_t index, RTV_DescriptorHeap& rtv_heap);

	/// <summary>
	/// Download GPU texture memory into CPU memory.
	/// </summary>
	/// <remarks>Will strip any padding when copying over.</remarks>
	void download(byte* r_mem);

	/// <summary>
	/// Gets the size of each pixel in bytes.
	/// </summary>
	uint32_t getPixelSize();

	/// <summary>
	/// Gets the size of the rows, excludes the padding at each row end.
	/// </summary>
	size_t getRowSize();

	/// <summary>
	/// Texture rows may not be stored in memory consecutively, especially if they are not power of 2.
	/// They may have padding added to the end. Use this if you need to index in the texture.
	/// </summary>
	/// <returns>Size of row in bytes + padding.</returns>
	size_t getRowPitch();
};
