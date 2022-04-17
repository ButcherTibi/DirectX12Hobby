#include "DX12.hpp"


CPU_DescriptorHandle::CPU_DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE new_handle)
{
	this->handle = new_handle;
}

void DescriptorHeap::init(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	D3D12_DESCRIPTOR_HEAP_DESC info = {};
	info.NumDescriptors = size;
	info.Type = type;
	info.Flags = flags;

	checkDX12(renderer.dev->CreateDescriptorHeap(&info, IID_PPV_ARGS(heap.GetAddressOf())));

	used_count = 0;
}

CPU_DescriptorHandle DescriptorHeap::at(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index)
{
	uint32_t size = renderer.dev->GetDescriptorHandleIncrementSize(type);

	D3D12_CPU_DESCRIPTOR_HANDLE result = heap->GetCPUDescriptorHandleForHeapStart();
	result.ptr += size * index;

	return result;
}

RTV_DescriptorHandle RTV_DescriptorHeap::addRenderTargetView(Texture& tex)
{
	auto tex_desc = tex.texture->GetDesc();

	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = tex_desc.Format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	desc.Texture2D.PlaneSlice = 0;

	auto cpu_handle = at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, used_count);
	renderer.dev->CreateRenderTargetView(tex.texture.Get(), &desc, cpu_handle.handle);

	used_count++;

	RTV_DescriptorHandle rtv_handle;
	rtv_handle.handle = cpu_handle.handle;
	return rtv_handle;
}
