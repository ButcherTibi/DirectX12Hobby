#include "./IndexBuffer.hpp"


void IndexBuffer::create(Context* new_context, D3D12_HEAP_TYPE heap_type)
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
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	states = D3D12_RESOURCE_STATE_COMMON;
}

void IndexBuffer::resize(uint32_t new_count)
{
	Resource::resize(new_count * sizeof(uint32_t));
}

void IndexBuffer::update(uint32_t index, uint32_t element)
{
	Resource::update(index, &element, sizeof(uint32_t));
}

uint32_t IndexBuffer::count()
{
	return (uint32_t)(desc.Width / sizeof(uint32_t));
}
