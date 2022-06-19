#include "Texture.hpp"

#include "../Descriptors/Descriptors.hpp"


void Texture::createTexture2D(Context* new_context,
	uint32_t width, uint32_t height, DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state)
{
	context = new_context;
	resource = nullptr;

	heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = flags;

	states = state;

	checkDX12(context->dev->CreateCommittedResource(
		&heap_props,
		heap_flags,
		&desc,
		states,
		nullptr,
		IID_PPV_ARGS(&resource)));
}

void Texture::createRenderTarget(Context* new_context,
	uint32_t width, uint32_t height, DXGI_FORMAT format)
{
	context = new_context;
	resource = nullptr;

	heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	desc.Width = width;
	desc.Height = height;
	desc.DepthOrArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = format;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

	states = D3D12_RESOURCE_STATE_RENDER_TARGET;

	checkDX12(context->dev->CreateCommittedResource(
		&heap_props,
		heap_flags,
		&desc,
		states,
		nullptr,
		IID_PPV_ARGS(&resource)));
}

void Texture::createSwapchainRenderTarget(Context* new_context, ID3D12Resource* swapchain_backbuffer)
{
	context = new_context;
	resource = swapchain_backbuffer;

	heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
	heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

	desc = swapchain_backbuffer->GetDesc();

	states = D3D12_RESOURCE_STATE_PRESENT;
}

SRV_DescriptorHandle Texture::createShaderResourceView(uint32_t index, CBV_SRV_UAV_DescriptorHeap& heap)
{
	D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
	srv_desc.Format = desc.Format;
	srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srv_desc.Texture2D.MostDetailedMip = 0;
	srv_desc.Texture2D.MipLevels = 1;
	srv_desc.Texture2D.PlaneSlice = 0;
	srv_desc.Texture2D.ResourceMinLODClamp = 0;
	srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

	SRV_DescriptorHandle srv;
	srv.heap = &heap;
	srv.idx = index;
	srv.cpu_handle = heap.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, index).cpu_handle;
	srv.gpu_handle = heap.at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, index).gpu_handle;
	
	context->dev->CreateShaderResourceView(get(), &srv_desc, srv.cpu_handle);

	return srv;
}

RTV_DescriptorHandle Texture::createRenderTargetView(uint32_t index, RTV_DescriptorHeap& rtv_heap)
{
	D3D12_RENDER_TARGET_VIEW_DESC rtv_desc = {};
	rtv_desc.Format = desc.Format;
	rtv_desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	rtv_desc.Texture2D.MipSlice = 0;
	rtv_desc.Texture2D.PlaneSlice = 0;

	RTV_DescriptorHandle rtv;
	rtv.heap = &rtv_heap;
	rtv.idx = index;
	rtv.cpu_handle = rtv_heap.at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtv.idx).cpu_handle;
	rtv.gpu_handle = rtv_heap.at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, rtv.idx).gpu_handle;

	context->dev->CreateRenderTargetView(get(), &rtv_desc, rtv.cpu_handle);

	return rtv;
}

bool Texture::copyToBuffer(Buffer& dest_buff)
{
	// GPU likes to round up textures so it may copy
	// with extra padding added to the row size
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT dest_footprint;
	size_t row_size;
	size_t download_size;
	{
		context->dev->GetCopyableFootprints(
			&desc, 0, 1, 0,
			&dest_footprint, nullptr, &row_size, &download_size
		);
	}

	bool dest_resized = dest_buff.resizeDiscard(download_size);

	// Copy to destination buffer
	{
		RecordAndWaitCommandList r;
		if (context->is_cmd_list_recording == false) {
			r = context->recordAndWaitCommandList();
		}

		auto prev_state = states;
		transitionTo(D3D12_RESOURCE_STATE_COPY_SOURCE);

		D3D12_TEXTURE_COPY_LOCATION dest = {};
		dest.pResource = dest_buff.get();
		dest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dest.PlacedFootprint = dest_footprint;

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = resource.Get();
		src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		context->cmd_list->CopyTextureRegion(
			&dest,
			0, 0, 0,
			&src,
			nullptr
		);

		transitionTo(prev_state);
	}

	return dest_resized;
}

void Texture::copyToCPUMemory(byte* r_mem)
{
	// GPU likes to round up textures so it may copy
	// with extra padding added to the row size
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT dest_footprint;
	size_t row_size;
	size_t download_size;
	{
		context->dev->GetCopyableFootprints(
			&desc, 0, 1, 0,
			&dest_footprint, nullptr, &row_size, &download_size
		);
	}

	resizeDownloadBuffer(download_size);

	// Copy to donwload buffer
	context->beginCommandList();
	{
		auto prev_state = states;
		transitionTo(D3D12_RESOURCE_STATE_COPY_SOURCE);

		D3D12_TEXTURE_COPY_LOCATION dest = {};
		dest.pResource = context->download_buff.Get();
		dest.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		dest.PlacedFootprint = dest_footprint;

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = resource.Get();
		src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

		context->cmd_list->CopyTextureRegion(
			&dest,
			0, 0, 0,
			&src,
			nullptr
		);

		transitionTo(prev_state);
	}
	context->endAndWaitForCommandList();

	// Unload from download buffer
	{
		size_t row_pitch = dest_footprint.Footprint.RowPitch;

		D3D12_RANGE read_all = {};
		read_all.Begin = 0;
		read_all.End = download_size;

		void* gpu_mem;
		checkDX12(context->download_buff->Map(0, &read_all, &gpu_mem));
		{
			for (uint32_t row = 0; row < desc.Height; row++) {

				std::memcpy(
					r_mem + row * row_size,
					((uint8_t*)gpu_mem) + row * row_pitch,
					row_pitch
				);
			}
		}

		D3D12_RANGE no_write = {};
		no_write.Begin = 1;
		no_write.End = 0;
		context->download_buff->Unmap(0, &no_write);
	}
}

uint32_t Texture::getPixelSize()
{
	switch (desc.Format) {
	case DXGI_FORMAT_B8G8R8A8_UNORM: return 4;
	default: __debugbreak();
	}

	return 0;
}

size_t Texture::getRowSize()
{
	size_t row_size;
	context->dev->GetCopyableFootprints(
		&desc, 0, 1, 0,
		nullptr, nullptr, &row_size, nullptr
	);

	return row_size;
}

size_t Texture::getRowPitch()
{
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
	{
		context->dev->GetCopyableFootprints(
			&desc, 0, 1, 0,
			&footprint, nullptr, nullptr, nullptr
		);
	}

	return footprint.Footprint.RowPitch;
}
