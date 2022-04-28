#pragma once

// Standard
#include <span>

// Mine
#include <DirectX12/Context/Context.hpp>


class Resource {
protected:
	Context* context = nullptr;
	ComPtr<ID3D12Resource> resource = nullptr;

	D3D12_HEAP_PROPERTIES heap_props = {};
	D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE;

public:
	D3D12_RESOURCE_DESC desc = {};
	D3D12_RESOURCE_STATES states;

private:
	void createDownloadBuffer(size_t size);

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
	/// Upload CPU memory to GPU resource, will resize if necessary.
	/// </summary>
	/// <param name="mem">Pointer to CPU memory to copy over</param>
	/// <param name="size">How many bytes to copy</param>
	/// <remarks>Will use upload buffer if heap is default</remarks>
	void upload(void* mem, size_t size);

	template<typename T>
	void upload(std::span<T> mem)
	{
		upload(mem.data(), mem.size_bytes());
	}

	/// <summary>
	/// Download GPU resource into CPU memory.
	/// </summary>
	/// <param name="r_mem">Pointer to CPU memory to copy to</param>
	/// <returns>Download size.</returns>
	size_t download(void* r_mem);

	template<typename T>
	size_t download(std::span<T> mem)
	{
		if (mem.size_bytes() < mem_size()) {
			__debugbreak();  // don't even think about it !
			return;
		}

		return download(mem.data());
	}

	ID3D12Resource* get();
	D3D12_GPU_VIRTUAL_ADDRESS gpu_adress();
	size_t mem_size();
};


class IndexBuffer : public Resource {
public:
	void create(Context* new_context);

	uint32_t count();

	uint32_t mem_size();
};


template<typename GPU_T = float>
class StorageBuffer : public Resource {
public:
	void create(Context* new_context, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		context = new_context;

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

	/*void upload(std::vector<GPU_T>& content)
	{
		Resource::upload(content.data(), content.size() * sizeof(GPU_T));
	}*/

	uint32_t count()
	{
		return (uint32_t)(desc.Width / sizeof(GPU_T));
	}
};
