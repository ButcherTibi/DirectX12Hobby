#pragma once

#include <SimpleX12/SimpleX12.hpp>
#include <Shaders/GPU_ShaderTypes.hpp>

#include "App.hpp"


class Renderer {
public:
	Context context;

	VertexShader vertex_shader;
	PixelShader pixel_shader;
	ComputeShader compute_shader;

	CBV_SRV_UAV_DescriptorHeap cbv_srv_uav_heap;
	RTV_DescriptorHeap rtv_heap;

	StorageBuffer<GPU_Vertex> verts;
	IndexBuffer indexes;
	Drawcall drawcall;

	StorageBuffer<GPU_VertexPositionUpdateGroup> pos_updates;
	DispatchCall dispatch;

	Texture final_rt;
	RTV_DescriptorHandle final_rtv;

	uint32_t render_width = 0;
	uint32_t render_height = 0;

public:
	void init();

	void render(uint32_t width, uint32_t height, uint8_t* r_pixels);

	// static void destroy();
};

extern Renderer renderer;
