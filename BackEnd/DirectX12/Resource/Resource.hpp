#pragma once

// Mine
#include <DirectX12/Context/Context.hpp>


void copy(ID3D12Resource* dest, ID3D12Resource* source);


class Resource {
protected:
	ComPtr<ID3D12Resource> resource = nullptr;

	D3D12_HEAP_PROPERTIES heap_props = {};
	D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE;

	inline static ComPtr<ID3D12Resource> upload_buff = nullptr;

public:
	D3D12_RESOURCE_DESC desc = {};
	D3D12_RESOURCE_STATES states;

public:
	/// <summary>
	/// Resize the resource by increasing it's width, previous content is discarded
	/// </summary>
	void resizeDiscard(size_t new_size);

	/// <summary>
	/// Resize the resource by increasing it's width, previous content is copied over
	/// </summary>
	void resize(size_t new_size);

	/// <summary>
	/// Upload CPU memory to GPU resource, will resize if necessary
	/// </summary>
	/// <param name="mem">Pointer to CPU memory to copy over</param>
	/// <param name="size">How many bytes to copy</param>
	/// <remarks>Will use staging buffer if heap is default</remarks>
	void upload(void* mem, size_t size);

	ID3D12Resource* get();
	D3D12_GPU_VIRTUAL_ADDRESS gpu_adress();
};


class IndexBuffer : public Resource {
public:
	void init();

	void upload(std::vector<uint32_t>& indexes);

	uint32_t count();

	uint32_t mem_size();
};


template<typename GPU_T = float>
class StorageBuffer : public Resource {
public:
	void init(D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		desc.Width = 0;
		desc.Height = 1;
		desc.DepthOrArraySize = 1;
		desc.MipLevels = 1;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		desc.Flags = flags;

		states = D3D12_RESOURCE_STATE_COMMON;
	}

	void upload(std::vector<GPU_T>& content)
	{
		Resource::upload(content.data(), content.size() * sizeof(GPU_T));
	}

	uint32_t count()
	{
		return (uint32_t)(desc.Width / sizeof(GPU_T));
	}
};
