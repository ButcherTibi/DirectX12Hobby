#include "App.hpp"


App app;

void App::init()
{
	state_update_lock.lock();

	renderer.init();

	// Viewport
	{
		viewport.width = 800;
		viewport.height = 600;

		auto& lighting = viewport.lighting;
		lighting.shading_normal = GPU_ShadingNormal::POLY;

		lighting.lights[0].normal = toNormal(45, 45);
		lighting.lights[0].color = { 1, 1, 1 };
		lighting.lights[0].intensity = 1.f;

		lighting.lights[1].normal = toNormal(-45, 45);
		lighting.lights[1].color = { 1, 1, 1 };
		lighting.lights[1].intensity = 1.f;

		lighting.lights[2].normal = toNormal(45, -45);
		lighting.lights[2].color = { 1, 1, 1 };
		lighting.lights[2].intensity = 1.f;

		lighting.lights[3].normal = toNormal(-45, -45);
		lighting.lights[3].color = { 1, 1, 1 };
		lighting.lights[3].intensity = 1.f;

		lighting.lights[4].intensity = 0.f;
		lighting.lights[5].intensity = 0.f;
		lighting.lights[6].intensity = 0.f;
		lighting.lights[7].intensity = 0.f;

		lighting.ambient_intensity = 0.03f;

		auto& camera = viewport.camera;
		camera.focal_point = { 0.f, 0.f, 0.f };
		camera.field_of_view = 15.f;
		camera.z_near = 0.1f;
		camera.z_far = 100'000.f;
		camera.pos = { 0, 0, 10 };
		camera.quat_inv = { 1, 0, 0, 0 };
		camera.forward = { 0, 0, -1 };

		camera.orbit_sensitivity = 0.1f;
		camera.pan_sensitivity = 0.001f;
		camera.dolly_sensitivity = 0.001f;
	}

	state_update_lock.unlock();

	createTriangleInstance();
}

void App::phase_1_runCPU()
{
	std::lock_guard lock{ state_update_lock };

	// CPU stuff here
}

void App::phase_2_waitForRendering()
{
	renderer.waitForRendering();
}

bool App::phase_2X_tryDownloadRender(u32 width, u32 height, byte* r_pixels)
{
	return renderer.tryDownloadRender(width, height, r_pixels);;
}

void App::phase_3_render()
{
	std::lock_guard lock{ state_update_lock };
	renderer.render();
}
