#pragma once

// Mine
#include "DX12.hpp"


class Buffer {
protected:

};


template<typename GPU_T = float>
class StorageBuffer {
	ComPtr<ID3D12Resource> buff = nullptr;

	D3D12_HEAP_PROPERTIES heap_props = {};
	D3D12_RESOURCE_DESC res_desc = {};
	uint32_t count = 0;

public:
	void init()
	{
		heap_props.Type = D3D12_HEAP_TYPE_DEFAULT;
		heap_props.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heap_props.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

		res_desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
		res_desc.Width = 0;
		res_desc.Height = 1;
		res_desc.DepthOrArraySize = 1;
		res_desc.MipLevels = 1;
		res_desc.Format = DXGI_FORMAT_UNKNOWN;
		res_desc.SampleDesc.Count = 1;
		res_desc.SampleDesc.Quality = 0;
		res_desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
		res_desc.Flags = D3D12_RESOURCE_FLAG_NONE;
	}

	void resize(uint32_t new_count)
	{
		auto& dev = renderer.dev;

		// fresh
		if (buff == nullptr) {

			res_desc.Width = new_count * sizeof(GPU_T);

			checkDX12(dev->CreateCommittedResource(
				&heap_props,
				D3D12_HEAP_FLAG_NONE,
				&res_desc,
				D3D12_RESOURCE_STATE_INDEX_BUFFER |
				D3D12_RESOURCE_STATE_COPY_DEST | D3D12_RESOURCE_STATE_COPY_SOURCE,
				nullptr,
				IID_PPV_ARGS(buff.GetAddressOf()))
			);
		}
		// copy existing data
		else if (new_count > count) {

			//ComPtr<ID3D12Resource> new_buff;
			//{
			//	D3D12_RESOURCE_DESC new_res_desc = res_desc;
			//	new_res_desc.Width = new_count * sizeof(T);

			//	checkDX12(dev->CreateCommittedResource(
			//		&heap_props,
			//		D3D12_HEAP_FLAG_NONE,
			//		&new_res_desc,
			//		D3D12_RESOURCE_STATE_INDEX_BUFFER |
			//		D3D12_RESOURCE_STATE_COPY_DEST | D3D12_RESOURCE_STATE_COPY_SOURCE,
			//		nullptr,
			//		IID_PPV_ARGS(new_buff.GetAddressOf()))
			//	);
			//}

			//

			//D3D11_BOX src_box = {};
			//src_box.left = 0;
			//src_box.right = init_desc.ByteWidth;
			//src_box.top = 0;
			//src_box.bottom = 1;
			//src_box.front = 0;
			//src_box.back = 1;

			//ctx3->CopySubresourceRegion(new_buff, 0,
			//	0, 0, 0,
			//	buff, 0,
			//	&src_box);

			//// destroy old
			//srv = nullptr;
			//uav = nullptr;
			//buff->Release();

			//// assign/create new
			//buff = new_buff;

			//init_desc.ByteWidth = new_size_bytes;
		}

		count = new_count;
	}
};
