#include "Renderer.hpp"


void Renderer::init()
{
	context.create();

	// Shaders
	{
		vertex_shader.createFromSourceCodeFile("G:/My work/DirectX12Hobby/BackEnd/Shaders/vertex.hlsl");
		pixel_shader.createFromSourceCodeFile("G:/My work/DirectX12Hobby/BackEnd/Shaders/pixel.hlsl");
		compute_shader.createFromSourceCodeFile("G:/My work/DirectX12Hobby/BackEnd/Shaders/compute.hlsl");
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

	// Final Texture
	{
		final_rt.createRenderTarget(&context, render_width, render_height, DXGI_FORMAT_R8G8B8A8_UNORM);

		final_rtv = rtv_heap.createRenderTargetView(0, final_rt);
	}

	// Readback Texture
	{
		
		// final_rt.download();
	}

	// Drawcall
	{
		drawcall.create(&context);
		drawcall.setShaderResourceViewParam(0);
		drawcall.setVertexShader(&vertex_shader);
		drawcall.setPixelShader(&pixel_shader);
		drawcall.build();
	}

	// Dispatch
	{
		dispatch.create(&context);
		dispatch.setShaderResourceViewParam(0);
		dispatch.setUnorderedAccessViewParam(0);
		dispatch.setComputeShader(&compute_shader);
		dispatch.build();
	}

	Context::beginPixCapture();
	render();
	Context::endPixCapture();
}

void Renderer::render()
{
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

		dispatch.CMD_bind();
		dispatch.CMD_setShaderResourceView(0, &pos_updates);
		dispatch.CMD_setUnorderedAccessView(0, &verts);
		dispatch.CMD_dispatch();

		drawcall.CMD_bind();
		drawcall.CMD_setIndexBuffer(indexes);
		drawcall.CMD_setShaderResourceView(0, &verts);
		drawcall.CMD_setViewportSize(render_width, render_height);
		drawcall.CMD_clearRenderTarget(final_rtv);
		drawcall.CMD_setRenderTargets({ final_rtv });
		drawcall.CMD_drawIndexed();
	}
	context.endAndWaitForCommandList();
}
