#pragma once
#include "../Resource/Resource.hpp"


template<typename GPU_T = float>
class StorageBuffer : public Buffer {
public:
	void create(Context* new_context, D3D12_HEAP_TYPE heap_type = D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE)
	{
		context = new_context;

		heap_props.Type = heap_type;
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

		if (heap_type == D3D12_HEAP_TYPE_DEFAULT) {
			states = D3D12_RESOURCE_STATE_COMMON;
		}
		else if (heap_type == D3D12_HEAP_TYPE_UPLOAD) {
			states = D3D12_RESOURCE_STATE_GENERIC_READ;
		}
	}

	void resize(uint32_t new_count)
	{
		Resource::resize(new_count * sizeof(GPU_T));
	}

	void update(uint32_t index, GPU_T& element)
	{
		Resource::update(index, &element, sizeof(GPU_T));
	}

	uint32_t count()
	{
		return (uint32_t)(desc.Width / sizeof(GPU_T));
	}
};