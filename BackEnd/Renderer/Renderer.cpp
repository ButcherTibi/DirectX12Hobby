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
		world_pos_readback.create(&context, D3D12_HEAP_TYPE_READBACK);
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
		drawcall.setRenderTargetFormats(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_B8G8R8A8_UNORM);
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

		bckgrd_call.create(&context);
		bckgrd_call.setVertexShader(&fullscreen_vs);
		bckgrd_call.setConstantBufferParam(0);
		bckgrd_call.setTextureParam(0);
		bckgrd_call.setPixelShader(&background_ps);
		bckgrd_call.setRenderTargetFormats(DXGI_FORMAT_B8G8R8A8_UNORM);
		bckgrd_call.rebuild();
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

			// Background Input
			backgroud_input_tex.createTexture2D(&context,
				viewport_width, viewport_height, DXGI_FORMAT_B8G8R8A8_UNORM,
				D3D12_RESOURCE_FLAG_NONE,
				D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
			);
			backgroud_input_srv = backgroud_input_tex.createShaderResourceView(
				(u32)ShaderResourceViews::background_input, cbv_srv_uav_heap);

			// Compose
			compose_rt.createRenderTarget(&context, viewport_width, viewport_height, DXGI_FORMAT_B8G8R8A8_UNORM);
			compose_rtv = compose_rt.createRenderTargetView((u32)RenderTargets::compose, rtv_heap);

			// World Position
			{
				world_pos_rt.createRenderTarget(&context,
					viewport_width, viewport_height, DXGI_FORMAT_R8G8B8A8_UNORM
				);
				world_pos_rtv = world_pos_rt.createRenderTargetView((u32)RenderTargets::world_pos, rtv_heap);
			}

			// Swapchain
			swapchain.resize(viewport_width, viewport_height);
			swapchain_rtvs[0] = swapchain.backbuffers[0].createRenderTargetView(
				(u32)RenderTargets::swapchain_0, rtv_heap);
			swapchain_rtvs[1] = swapchain.backbuffers[1].createRenderTargetView(
				(u32)RenderTargets::swapchain_1, rtv_heap);

			render_width = viewport_width;
			render_height = viewport_height;
		}
	}

	// Choose swapchain image
	swapchain_rtv = swapchain_rtvs[swapchain.getBackbufferIndex()];

	generateGPU_Data();

	// Hot Reloading
#if _DEBUG
	drawcall.rebuild();
	bckgrd_call.rebuild();
#endif

	// Command List
	{
		auto cmds = context.recordCommandList();

		swapchain.transitionToRenderTarget();
		cmds.setDescriptorHeaps(cbv_srv_uav_heap);
		cmds.clearRenderTarget(world_pos_rtv, FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX);
		cmds.clearRenderTarget(compose_rtv);

		for (auto& mesh : app.meshes) {

			drawcall.CMD_bind();
			drawcall.CMD_setIndexBuffer(mesh.gpu_indexes);
			drawcall.CMD_setConstantBufferParam(0, frame_cbuff);
			drawcall.CMD_setBufferParam(0, mesh.gpu_verts);
			drawcall.CMD_setBufferParam(1, mesh.gpu_instances);
			drawcall.CMD_setViewportSize(render_width, render_height);		
			drawcall.CMD_setRenderTargets({ world_pos_rtv, compose_rtv });
			drawcall.CMD_drawIndexed();
		}

		compose_rt.copyToBuffer(world_pos_readback);
		compose_rt.copyResource(backgroud_input_tex);	

		bckgrd_call.CMD_bind();
		bckgrd_call.CMD_setConstantBufferParam(0, frame_cbuff);
		bckgrd_call.CMD_setTextureParam(0, backgroud_input_srv);
		bckgrd_call.CMD_setViewportSize(render_width, render_height);
		bckgrd_call.CMD_setRenderTargets({ swapchain_rtv });
		bckgrd_call.CMD_draw();

		swapchain.transitionToPresentation();
	}

	swapchain.present();

	if (app.debug.capture_frame) {

		waitForRendering();
		Context::endPixCapture();

		app.debug.capture_frame = false;
	}
}
