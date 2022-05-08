#pragma once

#include <SimpleX12/SimpleX12.hpp>
#include <Shaders/GPU_ShaderTypes.hpp>
#include <CommonTypes.hpp>

#include "App.hpp"


struct RenderWorkload {
	u32 width;
	u32 height;

	bool capture_frame;
};

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

	void waitForRendering();

	void downloadRender(u32& r_width, u32& r_height, std::vector<byte>& r_pixels);

	void render(RenderWorkload& workload);
};

extern Renderer renderer;
