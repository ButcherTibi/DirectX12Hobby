#include "Renderer.hpp"


Renderer renderer;

void Renderer::init()
{
	context.create();

	// Shaders
	{
		vertex_shader.createFromSourceCodeFile(&context, L"G:/My work/DirectX12Hobby/BackEnd/Shaders/vertex.hlsl");
		pixel_shader.createFromSourceCodeFile(&context, L"G:/My work/DirectX12Hobby/BackEnd/Shaders/pixel.hlsl");
		compute_shader.createFromSourceCodeFile(&context, L"G:/My work/DirectX12Hobby/BackEnd/Shaders/compute.hlsl");
	}

	// Descriptor Heaps
	{
		cbv_srv_uav_heap.create(&context);
		rtv_heap.create(&context);
	}

	// Vertices
	{
		verts.create(&context, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS);

		std::vector<GPU_Vertex> gpu_verts(3);
		gpu_verts[0].pos = { 0.0f, 0.25f, 0.0f };
		gpu_verts[1].pos = { 0.25f, -0.25f, 0.0f };
		gpu_verts[2].pos = { -0.25f, -0.25f, 0.0f };

		verts.upload(std::span{gpu_verts});
	}

	// Indexes
	{
		indexes.create(&context);

		std::vector<uint32_t> gpu_indexes = { 0, 1, 2 };
		indexes.upload(std::span{gpu_indexes});
	}

	// Position Updates
	{
		pos_updates.create(&context);

		std::vector<GPU_VertexPositionUpdateGroup> updates(1);
		updates[0].vertex_id[0] = 0;
		updates[0].new_pos[0] = {0.0f, 0.50f, 0.0f};

		for (uint32_t i = 1; i < 64; i++) {
			updates[0].vertex_id[i] = 0xFFFF'FFFF;
		}

		pos_updates.upload(std::span{updates});
	}

	// Drawcall
	{
		drawcall.create(&context);
		drawcall.setShaderResourceViewParam(0);
		drawcall.setVertexShader(&vertex_shader);
		drawcall.setPixelShader(&pixel_shader);
		drawcall.setRenderTargetFormats(DXGI_FORMAT_B8G8R8A8_UNORM);
	}

	// Dispatch
	{
		dispatch.create(&context);
		dispatch.setShaderResourceViewParam(0);
		dispatch.setUnorderedAccessViewParam(0);
		dispatch.setComputeShader(&compute_shader);
		dispatch.rebuild();
	}
}

void Renderer::render(uint32_t new_width, uint32_t new_height, uint8_t* r_pixels)
{
	// Resize assets
	if (render_width != new_width || render_height != new_height) {

		// Final texture
		final_rt.createRenderTarget(&context, new_width, new_height, DXGI_FORMAT_B8G8R8A8_UNORM);
		final_rtv = rtv_heap.createRenderTargetView(0, final_rt);

		render_width = new_width;
		render_height = new_height;
	}

	drawcall.rebuild();

	Context::beginPixCapture();

	// Command List
	context.beginCommandList();
	{
		drawcall.CMD_bind();
		drawcall.CMD_setIndexBuffer(indexes);
		drawcall.CMD_setShaderResourceView(0, &verts);
		drawcall.CMD_setViewportSize(render_width, render_height);
		drawcall.CMD_clearRenderTarget(final_rtv);
		drawcall.CMD_setRenderTargets({ final_rtv });
		drawcall.CMD_drawIndexed();

		//dispatch.CMD_bind();
		//dispatch.CMD_setShaderResourceView(0, &pos_updates);
		//dispatch.CMD_setUnorderedAccessView(0, &verts);
		//dispatch.CMD_dispatch();

		//drawcall.CMD_bind();
		//drawcall.CMD_setIndexBuffer(indexes);
		//drawcall.CMD_setShaderResourceView(0, &verts);
		//drawcall.CMD_setViewportSize(render_width, render_height);
		//drawcall.CMD_clearRenderTarget(final_rtv);
		//drawcall.CMD_setRenderTargets({ final_rtv });
		//drawcall.CMD_drawIndexed();
	}
	context.endAndWaitForCommandList();

	final_rt.download(r_pixels);

	Context::endPixCapture();

	/*for (uint32_t row = 0; row < render_height; row++) {
		for (uint32_t col = 0; col < render_width; col++) {
			r_pixels[row * (render_width * 4) + (col * 4) + 0] = 0xFF;
			r_pixels[row * (render_width * 4) + (col * 4) + 3] = 0xFF;
		}
	}*/
}
