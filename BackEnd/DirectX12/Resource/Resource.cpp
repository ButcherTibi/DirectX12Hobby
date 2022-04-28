#include "Resource.hpp"


void Resource::resizeDiscard(size_t new_size)
{
	if (resource == nullptr) {

		desc.Width = new_size;

		checkDX12(context->dev->CreateCommittedResource(
			&heap_props,
			heap_flags,
			&desc,
			states,
			nullptr,
			IID_PPV_ARGS(resource.GetAddressOf()))
		);
	}
	else if (new_size > desc.Width) {

		resource = nullptr;

		desc.Width = new_size;

		checkDX12(context->dev->CreateCommittedResource(
			&heap_props,
			heap_flags,
			&desc,
			states,
			nullptr,
			IID_PPV_ARGS(resource.GetAddressOf()))
		);
	}
}

void Resource::resize(size_t new_size)
{
	// fresh
	if (resource == nullptr) {

		desc.Width = new_size;

		checkDX12(context->dev->CreateCommittedResource(
			&heap_props,
			heap_flags,
			&desc,
			states,
			nullptr,
			IID_PPV_ARGS(resource.GetAddressOf()))
		);
	}
	// copy existing data
	else if (new_size > desc.Width) {

		ComPtr<ID3D12Resource> new_resource;
		{
			D3D12_RESOURCE_DESC new_res_desc = desc;
			new_res_desc.Width = new_size;

			checkDX12(context->dev->CreateCommittedResource(
				&heap_props,
				heap_flags,
				&new_res_desc,
				states,
				nullptr,
				IID_PPV_ARGS(new_resource.GetAddressOf()))
			);
		}

		context->copy(new_resource.Get(), resource.Get());

		resource = nullptr;
		resource = new_resource;

		desc.Width = new_size;
	}
}

void createUploadBuffer(Context* context, ComPtr<ID3D12Resource>& r_staging_buff, size_t new_size)
{
	D3D12_HEAP_PROPERTIES heap_props = {};
	heap_props.Type = D3D12_HEAP_TYPE_UPLOAD;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_HEAP_FLAGS flags = D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

	D3D12_RESOURCE_DESC desc = {};
	desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	desc.Width = new_size;
	desc.Height = 1;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_UNKNOWN;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES states = D3D12_RESOURCE_STATE_GENERIC_READ | D3D12_RESOURCE_STATE_COPY_SOURCE;

	checkDX12(context->dev->CreateCommittedResource(
		&heap_props,
		flags,
		&desc,
		states,
		nullptr,
		IID_PPV_ARGS(r_staging_buff.GetAddressOf()))
	);
}

void loadIntoMappable(ID3D12Resource* mappable_res, void* mem, size_t size)
{
	if (mappable_res->GetDesc().Width < size) {
		__debugbreak();
	}

	D3D12_RANGE no_read = {};
	no_read.Begin = 0;
	no_read.End = 0;

	void* gpu_mem_ptr;
	checkDX12(mappable_res->Map(0, &no_read, &gpu_mem_ptr));
	{
		std::memcpy(gpu_mem_ptr, mem, size);
	}

	D3D12_RANGE write_all = {};
	write_all.Begin = 0;
	write_all.End = size;
	mappable_res->Unmap(0, &write_all);
}

void Resource::upload(void* mem, size_t size)
{
	resizeDiscard(size);

	switch (heap_props.Type) {
	case D3D12_HEAP_TYPE_UPLOAD: {

		loadIntoMappable(resource.Get(), mem, size);
		break;
	}
	case D3D12_HEAP_TYPE_DEFAULT: {
		
		if (context->upload_buff == nullptr) {
			createUploadBuffer(context, context->upload_buff, size);
		}
		else if (context->upload_buff->GetDesc().Width < size) {
			context->upload_buff = nullptr;
			createUploadBuffer(context, context->upload_buff, size);
		}

		loadIntoMappable(context->upload_buff.Get(), mem, size);
		context->copy(resource.Get(), context->upload_buff.Get());
		break;
	}
	default: __debugbreak();
	}
}

size_t Resource::download(void* r_mem)
{
	size_t download_size = mem_size();

	if (context->download_buff == nullptr) {
		createDownloadBuffer(download_size);
	}
	else if (context->download_buff->GetDesc().Width < download_size) {
		context->download_buff = nullptr;
		createDownloadBuffer(download_size);
	}

	context->copy(context->download_buff.Get(), resource.Get());

	D3D12_RANGE read_all = {};
	read_all.Begin = 0;
	read_all.End = download_size;

	void* gpu_mem_ptr;
	checkDX12(resource->Map(0, &read_all, &gpu_mem_ptr));
	{
		std::memcpy(r_mem, gpu_mem_ptr, download_size);
	}

	D3D12_RANGE no_write = {};
	no_write.Begin = 1;
	no_write.End = 0;
	resource->Unmap(0, &no_write);

	return download_size;
}

void Resource::createDownloadBuffer(size_t size)
{
	D3D12_HEAP_PROPERTIES d_heap_props = {};
	d_heap_props.Type = D3D12_HEAP_TYPE_READBACK;
	d_heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d_heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	D3D12_HEAP_FLAGS d_flags = D3D12_HEAP_FLAG_CREATE_NOT_ZEROED;

	D3D12_RESOURCE_DESC d_desc = {};
	d_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	d_desc.Width = size;
	d_desc.Height = 1;
	d_desc.DepthOrArraySize = 1;
	d_desc.MipLevels = 1;
	d_desc.Format = DXGI_FORMAT_UNKNOWN;
	d_desc.SampleDesc.Count = 1;
	d_desc.SampleDesc.Quality = 0;
	d_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	d_desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES d_states = D3D12_RESOURCE_STATE_COPY_DEST;

	checkDX12(context->dev->CreateCommittedResource(
		&d_heap_props,
		d_flags,
		&d_desc,
		d_states,
		nullptr,
		IID_PPV_ARGS(context->download_buff.GetAddressOf()))
	);
}

ID3D12Resource* Resource::get()
{
	return resource.Get();
}

D3D12_GPU_VIRTUAL_ADDRESS Resource::gpu_adress()
{
	return resource->GetGPUVirtualAddress();
}

size_t Resource::mem_size()
{
	size_t total_size;

	context->dev->GetCopyableFootprints(&desc, 0, 0, 0,
		nullptr, nullptr, nullptr, &total_size);

	return total_size;
}

void IndexBuffer::create(Context* new_context)
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
	desc.Flags = D3D12_RESOURCE_FLAG_NONE;

	states = D3D12_RESOURCE_STATE_COMMON;
}

uint32_t IndexBuffer::count()
{
	return (uint32_t)(desc.Width / sizeof(uint32_t));
}

uint32_t IndexBuffer::mem_size()
{
	return (uint32_t)(desc.Width);
}
