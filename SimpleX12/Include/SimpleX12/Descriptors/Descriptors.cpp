#include "Descriptors.hpp"

#include "../Texture/Texture.hpp"


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
	DescriptorHandle result;
	uint32_t size = context->dev->GetDescriptorHandleIncrementSize(type);

	result.cpu_handle = heap->GetCPUDescriptorHandleForHeapStart();
	result.cpu_handle.ptr += size * index;

	result.gpu_handle = heap->GetGPUDescriptorHandleForHeapStart();
	result.gpu_handle.ptr += size * index;

	return result;
}

void CBV_SRV_UAV_DescriptorHeap::create(Context* new_context, uint32_t size, D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	DescriptorHeap::create(new_context, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, size, flags);
}

//SRV_DescriptorHandle CBV_SRV_UAV_DescriptorHeap::createTexture2D_SRV(uint32_t index, Texture& texture)
//{
//	D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
//	desc.Format = texture.desc.Format;
//	desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
//	desc.Texture2D.MostDetailedMip = 0;
//	desc.Texture2D.MipLevels = 1;
//	desc.Texture2D.PlaneSlice = 0;
//	desc.Texture2D.ResourceMinLODClamp = 0;
//	desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
//
//	SRV_DescriptorHandle r_handle;
//	r_handle.heap = this;
//	r_handle.cpu_handle = at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, index).cpu_handle;
//	r_handle.gpu_handle = at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, index).gpu_handle;
//	context->dev->CreateShaderResourceView(
//		texture.get(), &desc, r_handle.cpu_handle);
//
//	return r_handle;
//}

void RTV_DescriptorHeap::create(Context* new_context, uint32_t size)
{
	DescriptorHeap::create(new_context, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, size, D3D12_DESCRIPTOR_HEAP_FLAG_NONE);
}
