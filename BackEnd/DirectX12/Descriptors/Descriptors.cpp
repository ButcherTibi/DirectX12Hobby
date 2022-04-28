#include "Descriptors.hpp"


DescriptorHandle::DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE new_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE new_gpu_handle)
{
	this->cpu_handle = new_cpu_handle;
	this->gpu_handle = new_gpu_handle;
}

SRV_DescriptorHandle::SRV_DescriptorHandle(DescriptorHandle new_handle)
{
	this->cpu_handle = new_handle.cpu_handle;
	this->gpu_handle = new_handle.gpu_handle;
}

void DescriptorHeap::create(Context* new_context, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	context = new_context;

	D3D12_DESCRIPTOR_HEAP_DESC info = {};
	info.NumDescriptors = size;
	info.Type = type;
	info.Flags = flags;

	checkDX12(context->dev->CreateDescriptorHeap(&info, IID_PPV_ARGS(heap.GetAddressOf())));
}

ID3D12DescriptorHeap* DescriptorHeap::get()
{
	return heap.Get();
}

DescriptorHandle DescriptorHeap::at(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index)
{
	uint32_t size = context->dev->GetDescriptorHandleIncrementSize(type);

	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle = heap->GetCPUDescriptorHandleForHeapStart();
	cpu_handle.ptr += size * index;

	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle = heap->GetGPUDescriptorHandleForHeapStart();
	gpu_handle.ptr += size * index;

	return DescriptorHandle(cpu_handle, gpu_handle);
}

void CBV_SRV_UAV_DescriptorHeap::create(Context* new_context, uint32_t size, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	DescriptorHeap::create(new_context, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, size, flags);
}

void RTV_DescriptorHeap::create(Context* new_context, uint32_t size)
{
	DescriptorHeap::create(new_context, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, size, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
}

RTV_DescriptorHandle RTV_DescriptorHeap::createRenderTargetView(uint32_t index, Texture& tex)
{
	D3D12_RENDER_TARGET_VIEW_DESC desc = {};
	desc.Format = tex.desc.Format;
	desc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
	desc.Texture2D.MipSlice = 0;
	desc.Texture2D.PlaneSlice = 0;

	RTV_DescriptorHandle rtv_handle;
	rtv_handle.cpu_handle = at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, index).cpu_handle;
	rtv_handle.gpu_handle = at(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, index).gpu_handle;
	context->dev->CreateRenderTargetView(tex.get(), &desc, rtv_handle.cpu_handle);

	return rtv_handle;
}
