#pragma once

// Standard
#include <span>

// Mine
#include "../Context/Context.hpp"


class Resource {
protected:
	Context* context = nullptr;
	ComPtr<ID3D12Resource> resource = nullptr;

	D3D12_HEAP_PROPERTIES heap_props = {};
	D3D12_HEAP_FLAGS heap_flags = D3D12_HEAP_FLAG_NONE;

	void* mapped_mem = nullptr;

	std::wstring name;

public:
	D3D12_RESOURCE_DESC desc = {};
	D3D12_RESOURCE_STATES states;

private:
	void createDownloadBuffer(size_t size);

protected:
	void resizeDownloadBuffer(size_t size);

public:
	/// <summary>
	/// Resize the resource by increasing it's width, previous content is discarded.
	/// </summary>
	void resizeDiscard(size_t new_size);

	/// <summary>
	/// Resize the resource by increasing it's width, previous content is copied over.
	/// </summary>
	void resize(size_t new_size);

	/// <summary>
	/// Upload CPU memory to GPU resource, will resize if necessary.
	/// </summary>
	/// <param name="mem">Pointer to CPU memory to copy over.</param>
	/// <param name="size">How many bytes to copy.</param>
	/// <remarks>Will use upload buffer if heap is default.</remarks>
	void upload(void* mem, size_t size);

	template<typename GPU_T>
	void upload(std::span<GPU_T> mem)
	{
		upload(mem.data(), mem.size_bytes());
	}

	template<typename GPU_T>
	void upload(std::vector<GPU_T>& elements)
	{
		upload(elements.data(), elements.size() * sizeof(GPU_T));
	}

	virtual void download(uint8_t* r_mem);

	void mapNoRead();

	void update(uint32_t index, void* element, size_t element_size);

	void unmap();

	/// <summary>
	/// Transitions the resource to a new state.
	/// </summary>
	/// <param name="transition_now">By default this method is used when recording command list,
	/// set <c>transition_now</c> to true to execute the command now on the CPU.
	/// </param>
	void transitionTo(D3D12_RESOURCE_STATES new_state);

	void copy(Resource& dest);

	ID3D12Resource* get();

	D3D12_GPU_VIRTUAL_ADDRESS gpu_adress();

	/// <summary>
	/// This returns the total memory used to store the elements + additional padding.
	/// </summary>
	size_t mem_size();

	void setName(std::wstring name);
};


class Buffer : public Resource {};
