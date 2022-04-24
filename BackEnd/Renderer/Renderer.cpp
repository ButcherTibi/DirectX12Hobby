#include "Renderer.hpp"


void Renderer::init()
{
	Context::init();

	// Shaders
	{
		vertex_shader.createFromSourceCodeFile("G:/My work/DirectX12Hobby/BackEnd/Shaders/vertex.hlsl");
		pixel_shader.createFromSourceCodeFile("G:/My work/DirectX12Hobby/BackEnd/Shaders/pixel.hlsl");
	}

	// Descriptor Heaps
	{
		cbv_srv_uav_heap.init();
		rtv_heap.init();
	}

	// Vertices
	{
		verts_sbuff.init();

		std::array<GPU_Vertex, 3> gpu_verts;
		gpu_verts[0].pos = { 0.0f, 0.25f, 0.0f };
		gpu_verts[1].pos = { 0.25f, -0.25f, 0.0f };
		gpu_verts[2].pos = { -0.25f, -0.25f, 0.0f };

		verts_sbuff.load(gpu_verts.data(), gpu_verts.size() * sizeof(GPU_Vertex));
	}

	// Final Texture
	{
		final_rt.createRenderTarget(render_width, render_height, DXGI_FORMAT_R8G8B8A8_UNORM);

		final_rtv = rtv_heap.createRenderTargetView(0, final_rt);
	}

	// Drawcall
	{
		drawcall.init();

		drawcall.setShaderResourceViewParam(0);

		drawcall.setVertexShader(&vertex_shader);
		drawcall.setPixelShader(&pixel_shader);

		drawcall.build();
	}

	Context::beginPixCapture();
	render();
	Context::endPixCapture();
}

void Renderer::render()
{
	// Command List
	Context::beginCommandList();
	{
		drawcall.CMD_bind(Context::cmd_list.Get());
		drawcall.CMD_setShaderResourceView(0, &verts_sbuff);
		drawcall.CMD_setViewportSize(render_width, render_height);
		drawcall.CMD_clearRenderTarget(final_rtv);
		drawcall.CMD_setRenderTargets({ final_rtv });
		drawcall.CMD_draw(3);
	}
	Context::endAndWaitForCommandList();
}
