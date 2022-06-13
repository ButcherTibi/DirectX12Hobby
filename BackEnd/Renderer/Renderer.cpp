#include "Renderer.hpp"

#include <Sculpt Mesh/SculptMesh.hpp>


Renderer renderer;

void Renderer::init(bool enable_pix_debugger)
{
	if (enable_pix_debugger) {
		Context::initPix();
	}

	context.init();

	// Descriptor Heaps
	{
		cbv_srv_uav_heap.create(&context);
		rtv_heap.create(&context);
	}

	std::wstring root = L"G:/MyWork/DirectX12Hobby/BackEnd/Shaders/";

	// Common
	{
		fullscreen_vs.createFromSourceCodeFile(&context, root + L"FullScreenVS.hlsl");
	}

	// Shaders
	{
		vertex_shader.createFromSourceCodeFile(&context, root + L"vertex.hlsl");
		pixel_shader.createFromSourceCodeFile(&context, root + L"pixel.hlsl");
	}

	// Frame Uniform Buffer
	{
		frame_cbuff.create(&context);
		frame_cbuff.setName(L"Frame");
		
		// Viewport

		// Camera
		frame_cbuff.addFloat4();
		frame_cbuff.addFloat4();
		frame_cbuff.addFloat4();
		frame_cbuff.addMatrix();
		frame_cbuff.addFloat();
		frame_cbuff.addFloat();
	}

	// Drawcall
	{
		drawcall.create(&context);
		drawcall.setConstantBufferParam(0);
		drawcall.setBufferParam(0);
		drawcall.setBufferParam(1);
		drawcall.setVertexShader(&vertex_shader);
		drawcall.setPixelShader(&pixel_shader);
		drawcall.setRenderTargetFormats(DXGI_FORMAT_B8G8R8A8_UNORM);
		drawcall.rebuild();
	}

	// Vertex Position Update
	{
		vert_pos_update_shader.createFromSourceCodeFile(&context, root + L"compute.hlsl");

		auto& call = vert_pos_update_call;
		call.create(&context);
		call.setBufferParam(0);
		call.setUnorderedAccessResourceParam(0);
		call.setComputeShader(&vert_pos_update_shader);
		call.rebuild();
	}

	// Index Update
	{
		index_update_shader.createFromSourceCodeFile(&context, root + L"IndexUpdate.hlsl");

		auto& call = index_update_call;
		call.create(&context);
		call.setBufferParam(0);
		call.setUnorderedAccessResourceParam(0);
		call.setComputeShader(&index_update_shader);
		call.rebuild();
	}

	// Instance Update
	{
		instance_update_shader.createFromSourceCodeFile(&context, root + L"InstanceUpdate.hlsl");

		auto& call = instance_update_call;
		call.create(&context);
		call.setBufferParam(0);
		call.setUnorderedAccessResourceParam(0);
		call.setComputeShader(&instance_update_shader);
		call.rebuild();
	}

	// Background
	{
		background_ps.createFromSourceCodeFile(&context, root + L"Background/BackgroundPS.hlsl");

		background_call.create(&context);
		background_call.setVertexShader(&fullscreen_vs);
		background_call.setConstantBufferParam(0);
		background_call.setTextureParam(0);
		background_call.setPixelShader(&background_ps);
		background_call.setRenderTargetFormats(DXGI_FORMAT_B8G8R8A8_UNORM);
		background_call.rebuild();
	}

	// Swapchain
	{
		swapchain.create(&context, app.window.hwnd);
	}
}

void Renderer::waitForRendering()
{
	context.waitForCommandList();
}

bool Renderer::tryDownloadRender(u32 dest_width, u32 dest_height, byte* r_pixels)
{
	if (compose_rt.desc.Width != dest_width || compose_rt.desc.Height != dest_height) {
		return false;
	}

	compose_rt.download(r_pixels);
	return true;
}

void Renderer::render()
{
	// Is even possible to render
	if (app.window.win_messages.is_minimized ||
		app.viewport.width < 8 || app.viewport.height < 8) {
		return;
	}

	if (app.debug.capture_frame) {
		Context::beginPixCapture();
	}

	// Change Resolution
	{
		auto viewport_width = app.viewport.width;
		auto viewport_height = app.viewport.height;

		if (render_width != viewport_width || render_height != viewport_height) {

			// Background Input Texture
			backgroud_input_tex.createTexture(&context, viewport_width, viewport_height,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D12_RESOURCE_FLAG_NONE,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
			backgroud_input_srv = cbv_srv_uav_heap.createTexture2D_SRV(0, backgroud_input_tex);

			// Compose texture
			compose_rt.createRenderTarget(&context, viewport_width, viewport_height, DXGI_FORMAT_B8G8R8A8_UNORM);
			compose_rtv = compose_rt.createRenderTargetView(0, rtv_heap);

			// Swapchain
			swapchain.resize(viewport_width, viewport_height);
			swapchain_rtvs[0] = swapchain.backbuffers[0].createRenderTargetView(1, rtv_heap);
			swapchain_rtvs[1] = swapchain.backbuffers[1].createRenderTargetView(2, rtv_heap);

			render_width = viewport_width;
			render_height = viewport_height;
		}
	}

	// Choose swapchain image
	swapchain_rtv = swapchain_rtvs[swapchain.getBackbufferIndex()];

	generateGPU_Data();

	// Hot Reloading
	drawcall.rebuild();
	background_call.rebuild();

	// Command List
	context.beginCommandList();
	{
		swapchain.transitionToRenderTarget();

		{
			auto f = cbv_srv_uav_heap.get();
			context.cmd_list->SetDescriptorHeaps(1, &f);
		}

		drawcall.CMD_clearRenderTarget(compose_rtv);

		for (auto& mesh : app.meshes) {

			drawcall.CMD_bind();
			drawcall.CMD_setIndexBuffer(mesh.gpu_indexes);
			drawcall.CMD_setConstantBufferParam(0, frame_cbuff);
			drawcall.CMD_setBufferParam(0, mesh.gpu_verts);
			drawcall.CMD_setBufferParam(1, mesh.gpu_instances);
			drawcall.CMD_setViewportSize(render_width, render_height);		
			drawcall.CMD_setRenderTargets({ compose_rtv });
			drawcall.CMD_drawIndexed();
		}

		compose_rt.copy(backgroud_input_tex);	

		background_call.CMD_bind();
		background_call.CMD_setConstantBufferParam(0, frame_cbuff);
		background_call.CMD_setTextureParam(0, backgroud_input_srv);
		background_call.CMD_setViewportSize(render_width, render_height);
		background_call.CMD_setRenderTargets({ swapchain_rtv });
		background_call.CMD_draw();

		swapchain.transitionToPresentation();
	}
	context.endCommandList();

	swapchain.present();

	if (app.debug.capture_frame) {

		waitForRendering();
		Context::endPixCapture();

		app.debug.capture_frame = false;
	}
}
