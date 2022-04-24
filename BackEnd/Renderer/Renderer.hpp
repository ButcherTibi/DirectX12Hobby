#pragma once

#include <DirectX12/Shader/Shader.hpp>
#include <DirectX12/Descriptors/Descriptors.hpp>
#include <DirectX12/Drawcall/Drawcall.hpp>
#include <Shaders/GPU_ShaderTypes.hpp>

#include "App.hpp"

#define _static static inline


class Renderer {
public:
	_static VertexShader vertex_shader;
	_static PixelShader pixel_shader;

	_static CBV_SRV_UAV_DescriptorHeap cbv_srv_uav_heap;
	_static RTV_DescriptorHeap rtv_heap;

	_static Drawcall drawcall;

	_static StorageBuffer<GPU_Vertex> verts_sbuff;

	_static Texture final_rt;
	_static RTV_DescriptorHandle final_rtv;

	_static uint32_t render_width = 1024;
	_static uint32_t render_height = 720;

public:
	static void init();
	static void render();
};
