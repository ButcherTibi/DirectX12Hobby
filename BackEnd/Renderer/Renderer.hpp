#pragma once

#include <SimpleX12/Drawcall/Drawcall.hpp>
#include <SimpleX12/Buffer/ConstantBuffer.hpp>
#include <SimpleX12/Swapchain/Swapchain.hpp>

#include <Shaders/GPU_ShaderTypes.hpp>
#include <CommonTypes.hpp>
#include <App/App.hpp>


class Renderer {
public:
	Context context;

	CBV_SRV_UAV_DescriptorHeap cbv_srv_uav_heap;
	RTV_DescriptorHeap rtv_heap;

	ConstantBuffer frame_cbuff;

	// Common
	VertexShader fullscreen_vs;

	// Solid Mesh
	VertexShader vertex_shader;
	PixelShader pixel_shader;
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

	// Background
	Texture backgroud_input_tex;
	SRV_DescriptorHandle backgroud_input_srv;
	PixelShader background_ps;
	Drawcall background_call;

	Texture compose_rt;
	RTV_DescriptorHandle compose_rtv;

	Swapchain swapchain;
	std::array<RTV_DescriptorHandle, 2> swapchain_rtvs;
	RTV_DescriptorHandle swapchain_rtv;

	uint32_t render_width = 0;
	uint32_t render_height = 0;

private:
	void generateGPU_Data();

public:
	void init(bool enable_pix_debugger);

	void waitForRendering();

	bool tryDownloadRender(u32 dest_width, u32 dest_height, byte* r_pixels);

	void render();
};

extern Renderer renderer;
