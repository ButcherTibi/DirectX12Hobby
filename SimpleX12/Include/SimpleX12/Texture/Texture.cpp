#include "Texture.hpp"


void Texture::createTexture(Context* new_context, uint32_t width, uint32_t height, DXGI_FORMAT format,
	D3D12_RESOURCE_FLAGS flags, D3D12_RESOURCE_STATES state,
	std::array<float, 4>& new_clear_color, float new_clear_depth, uint8_t new_clear_stencil)
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

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = format;
	clear_value.Color[0] = new_clear_color[0];
	clear_value.Color[1] = new_clear_color[1];
	clear_value.Color[2] = new_clear_color[2];
	clear_value.Color[3] = new_clear_color[3];
	clear_value.DepthStencil.Depth = new_clear_depth;
	clear_value.DepthStencil.Stencil = new_clear_stencil;

	checkDX12(context->dev->CreateCommittedResource(
		&heap_props,
		heap_flags,
		&desc,
		states,
		&clear_value,
		IID_PPV_ARGS(&resource)));
}

void Texture::createRenderTarget(Context* new_context,
	uint32_t width, uint32_t height, DXGI_FORMAT format,
	std::array<float, 4> new_clear_color, float new_clear_depth, uint8_t new_clear_stencil)
{
	createTexture(new_context, width, height, format,
		D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET,
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		new_clear_color, new_clear_depth, new_clear_stencil);
}

void Texture::download(uint8_t* r_mem)
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
