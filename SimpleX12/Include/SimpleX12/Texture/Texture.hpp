#pragma once

#include "../Buffer/Buffer.hpp"

class CBV_SRV_UAV_DescriptorHeap;
class RTV_DescriptorHeap;
struct RTV_DescriptorHandle;
struct SRV_DescriptorHandle;

class Swapchain;


class Texture : public Resource {
public:
	void createTexture2D(Context* context,
		uint32_t width, uint32_t height, DXGI_FORMAT format,
		D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state);

	void createRenderTarget(Context* context,
		uint32_t width, uint32_t height, DXGI_FORMAT format);

	void createSwapchainRenderTarget(Context* context, ID3D12Resource* swapchain_backbuffer);

	SRV_DescriptorHandle createShaderResourceView(uint32_t index, CBV_SRV_UAV_DescriptorHeap& heap);

	RTV_DescriptorHandle createRenderTargetView(uint32_t index, RTV_DescriptorHeap& rtv_heap);

	/// <summary>
	/// Copy texture to buffer.
	/// Stored texture memory in buffer may not be continous, you must factor in source texture pitch.
	/// </summary>
	/// <returns>True if the destination buffer was resized</returns>
	bool copyToBuffer(Buffer& dest);

	/// <summary>
	/// Download GPU texture memory into CPU memory.
	/// </summary>
	/// <remarks>Will strip any padding when copying over</remarks>
	void copyToCPUMemory(byte* r_mem);

	/// <summary>
	/// Gets the size of each pixel in bytes.
	/// </summary>
	uint32_t getPixelSize();

	/// <summary>
	/// Gets the size of the rows, excludes the padding at each row end.
	/// </summary>
	size_t getRowSize();

	/// <summary>
	/// Texture rows may not be stored in memory continous, especially if they are not power of 2.
	/// They may have padding added to the end. Use this if you need to index in the texture.
	/// </summary>
	/// <returns>Size of row in bytes + padding.</returns>
	size_t getRowPitch();
};
