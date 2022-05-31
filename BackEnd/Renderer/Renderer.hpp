#pragma once

#include <SimpleX12/SimpleX12.hpp>
#include <Shaders/GPU_ShaderTypes.hpp>
#include <CommonTypes.hpp>

#include <App/App.hpp>


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

	CBV_SRV_UAV_DescriptorHeap cbv_srv_uav_heap;
	RTV_DescriptorHeap rtv_heap;

	Drawcall drawcall;

	// Vertex Position Update
	ComputeShader vert_pos_update_shader;
	DispatchCall vert_pos_update_call;

	// Index Update
	ComputeShader index_update_shader;
	DispatchCall index_update_call;

	// Instance Update
	ComputeShader instance_update_shader;
	DispatchCall instance_update_call;

	Texture final_rt;
	RTV_DescriptorHandle final_rtv;

	uint32_t render_width = 0;
	uint32_t render_height = 0;

private:
	void generateGPU_Data();

public:
	void init();

	void waitForRendering();

	bool tryDownloadRender(u32 dest_width, u32 dest_height, byte* r_pixels);

	void render(RenderWorkload& workload);
};

extern Renderer renderer;
