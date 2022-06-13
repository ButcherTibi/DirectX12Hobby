#pragma once

#include "../Buffer/StorageBuffer.hpp"
#include "../Texture/Texture.hpp"


class DescriptorHeap;

struct DescriptorHandle {
	DescriptorHeap* heap;
	uint32_t idx;

	D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle;
	D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle;
};

struct CBV_DescriptorHandle : public DescriptorHandle {};
struct SRV_DescriptorHandle : public DescriptorHandle {};
struct UAV_DescriptorHandle : public DescriptorHandle {};

struct RTV_DescriptorHandle : public DescriptorHandle {
	D3D12_RENDER_TARGET_VIEW_DESC desc;
};


class DescriptorHeap {
public:
	Context* context = nullptr;
	ComPtr<ID3D12DescriptorHeap> heap;

public:
	void create(Context* context, D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t size = 128, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);

	DescriptorHandle at(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t index);

	ID3D12DescriptorHeap* get();
};


class CBV_SRV_UAV_DescriptorHeap : public DescriptorHeap {
public:
	void create(Context* context, uint32_t size = 128, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE);


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

	SRV_DescriptorHandle createTexture2D_SRV(uint32_t index, Texture& texture);
};


class RTV_DescriptorHeap : public DescriptorHeap {
public:
	void create(Context* context, uint32_t size = 128);
};