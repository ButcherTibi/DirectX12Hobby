#pragma once

#include <DirectX12/Resource/Resource.hpp>
#include <DirectX12/Texture/Texture.hpp>


struct DescriptorHandle {
	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;

	DescriptorHandle() = default;
	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE, D3D12_GPU_DESCRIPTOR_HANDLE);
};

struct SRV_DescriptorHandle : public DescriptorHandle {
	SRV_DescriptorHandle() = default;
	SRV_DescriptorHandle(DescriptorHandle);
};

struct RTV_DescriptorHandle : public DescriptorHandle { };


class DescriptorHeap {
protected:
	ComPtr<ID3D12DescriptorHeap> heap;

protected:
	DescriptorHandle at(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index);

public:
	void init(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size = 128, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	ID3D12DescriptorHeap* get();
};


class CBV_SRV_UAV_DescriptorHeap : public DescriptorHeap {
public:
	void init(uint32_t size = 128, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	template<typename T>
	SRV_DescriptorHandle createShaderResourceView(uint32_t index, StorageBuffer<T>& sbuff)
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.NumElements = sbuff.count();
		desc.Buffer.StructureByteStride = sizeof(T);
		desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

		SRV_DescriptorHandle r_handle = at(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, index);
		Context::dev->CreateShaderResourceView(
			sbuff.get(), &desc, r_handle.cpu_handle);

		return r_handle;
	}
};


class RTV_DescriptorHeap : public DescriptorHeap {
public:
	void init(uint32_t size = 128);

	RTV_DescriptorHandle createRenderTargetView(uint32_t index, Texture& texture);
};