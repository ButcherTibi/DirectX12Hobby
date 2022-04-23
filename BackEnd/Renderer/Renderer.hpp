#pragma once

#include <DirectX12/Shader/Shader.hpp>
#include <DirectX12/Descriptors/Descriptors.hpp>
#include <DirectX12/RootSignature/RootSignature.hpp>
#include <Shaders/GPU_ShaderTypes.hpp>

#include "App.hpp"

#define _static static inline


class Renderer {
public:
	_static CBV_SRV_UAV_DescriptorHeap cbv_srv_uav_heap;
	_static RTV_DescriptorHeap rtv_heap;

	_static StorageBuffer<GPU_Vertex> verts_sbuff;
	_static SRV_DescriptorHandle verts_srv;

	_static Texture final_rt;
	_static RTV_DescriptorHandle final_rtv;

	_static RootSignature root_sign;
	_static ComPtr<ID3D12PipelineState> pipeline;

	_static Shader shader;

	_static ComPtr<ID3D12Fence> fence;

	_static uint32_t render_width = 1024;
	_static uint32_t render_height = 720;


public:
	static void init();
	static void render();
};
