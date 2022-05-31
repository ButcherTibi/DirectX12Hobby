#include "Renderer.hpp"

#include <Sculpt Mesh/SculptMesh.hpp>


Renderer renderer;

void Renderer::init()
{
	context.create();

	std::wstring root = L"G:/My work/DirectX12Hobby/BackEnd/Shaders/";

	// Shaders
	{
		vertex_shader.createFromSourceCodeFile(&context, L"G:/My work/DirectX12Hobby/BackEnd/Shaders/vertex.hlsl");
		pixel_shader.createFromSourceCodeFile(&context, L"G:/My work/DirectX12Hobby/BackEnd/Shaders/pixel.hlsl");
	}

	// Descriptor Heaps
	{
		cbv_srv_uav_heap.create(&context);
		rtv_heap.create(&context);
	}

	// Drawcall
	{
		drawcall.create(&context);
		drawcall.setShaderResourceViewParam(0);
		drawcall.setVertexShader(&vertex_shader);
		drawcall.setPixelShader(&pixel_shader);
		drawcall.setRenderTargetFormats(DXGI_FORMAT_B8G8R8A8_UNORM);
	}

	// Vertex Position Update
	{
		vert_pos_update_shader.createFromSourceCodeFile(&context, L"G:/My work/DirectX12Hobby/BackEnd/Shaders/compute.hlsl");

		auto& call = vert_pos_update_call;
		call.create(&context);
		call.setShaderResourceViewParam(0);
		call.setUnorderedAccessViewParam(0);
		call.setComputeShader(&vert_pos_update_shader);
		call.rebuild();
	}

	// Index Update
	{
		index_update_shader.createFromSourceCodeFile(&context, root + L"IndexUpdate.hlsl");

		auto& call = index_update_call;
		call.create(&context);
		call.setShaderResourceViewParam(0);
		call.setUnorderedAccessViewParam(0);
		call.setComputeShader(&index_update_shader);
		call.rebuild();
	}

	// Instance Update
	{
		instance_update_shader.createFromSourceCodeFile(&context, root + L"InstanceUpdate.hlsl");

		auto& call = instance_update_call;
		call.create(&context);
		call.setShaderResourceViewParam(0);
		call.setUnorderedAccessViewParam(0);
		call.setComputeShader(&instance_update_shader);
		call.rebuild();
	}
}

void Renderer::waitForRendering()
{
	context.waitForCommandList();
}

bool Renderer::tryDownloadRender(u32 dest_width, u32 dest_height, byte* r_pixels)
{
	if (final_rt.desc.Width != dest_width || final_rt.desc.Height != dest_height) {
		return false;
	}

	final_rt.download(r_pixels);
	return true;
}

void Renderer::render(RenderWorkload& w)
{
	if (w.capture_frame) {
		Context::beginPixCapture();
	}

	// Change Resolution
	if (render_width != w.width || render_height != w.height) {

		// Final texture
		final_rt.createRenderTarget(&context, w.width, w.height, DXGI_FORMAT_B8G8R8A8_UNORM);
		final_rtv = rtv_heap.createRenderTargetView(0, final_rt);

		render_width = w.width;
		render_height = w.height;
	}

	generateGPU_Data();

	// Build the objects
	drawcall.rebuild();

	// Command List
	context.beginCommandList();
	{
		for (auto& mesh : app.meshes) {

			drawcall.CMD_bind();
			drawcall.CMD_setIndexBuffer(mesh.gpu_indexes);
			drawcall.CMD_setShaderResourceView(0, &mesh.gpu_verts);
			drawcall.CMD_setViewportSize(render_width, render_height);
			drawcall.CMD_clearRenderTarget(final_rtv);
			drawcall.CMD_setRenderTargets({ final_rtv });
			drawcall.CMD_drawIndexed();
		}
	}
	context.endCommandList();

	if (w.capture_frame) {
		waitForRendering();
		Context::endPixCapture();
	}
}
