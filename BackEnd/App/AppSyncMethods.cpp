#include "App.hpp"

#include <thread>


App app;

void App::init(bool enable_pix_debugger)
{
	thread = std::thread(&App::main, &app, enable_pix_debugger);
}

void App::main(bool enable_pix_debugger)
{
	// Raw Device Input for Mouse
	{
		RAWINPUTDEVICE raw_input_dev;
		raw_input_dev.usUsagePage = 0x01;
		raw_input_dev.usUsage = 0x02; // HID_USAGE_GENERIC_MOUSE
		raw_input_dev.dwFlags = 0;  // NOTE: RIDEV_NOLEGACY where legacy means regular input like WM_KEYDOWN or WM_LBUTTONDOWN
		raw_input_dev.hwndTarget = 0;

		if (!RegisterRawInputDevices(&raw_input_dev, 1, sizeof(RAWINPUTDEVICE))) {
			__debugbreak();
		};
	}

	// Window
	{
		window.width = 800;
		window.height = 600;
		window.init();
	}

	// Timing
	{
		frame_start_time = std::chrono::steady_clock::now();
		min_frame_duration_ms = 16;
	}

	// Lighting
	{
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
	}

	// Camera
	{
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

	renderer.init(enable_pix_debugger);

	createTriangleInstance();

	while (true) {

		// Calculate Delta Factor
		{
			SteadyTime now = std::chrono::steady_clock::now();

			delta_time = (float)toMs(now - frame_start_time) / std::chrono::milliseconds(16).count();
			frame_start_time = now;
		}

		// win32::printToOutput(std::format(L"delta_time = {} \n", delta_time));

		// Reset Input
		{
			input.unicode_list.clear();

			input.mouse_pos_history.clear();

			input.mouse_delta_x = 0;
			input.mouse_delta_y = 0;
			input.mouse_wheel_delta = 0;

			for (uint16_t virtual_key = 0; virtual_key < input.key_list.size(); virtual_key++) {

				KeyState& key = input.key_list[virtual_key];
				key.down_transition = false;
				key.up_transition = false;
			}
		}

		// Read Input
		MSG msg{};
		while (PeekMessage(&msg, window.hwnd, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Calculate time for keys
		for (uint16_t virtual_key = 0; virtual_key < input.key_list.size(); virtual_key++) {

			KeyState& key = input.key_list[virtual_key];

			if (key.is_down) {
				key.end_time = frame_start_time;
			}
			else {
				key.end_time = key.start_time;
			}
		}

		// Update
		{
			std::scoped_lock l(state_update_lock);

			CPU_update();
			renderer.waitForRendering();
			renderer.render();
		}

		// Frame Rate Limit
		{
			SteadyTime frame_used_time = std::chrono::steady_clock::now();
			SteadyTime target_end_time = frame_start_time + std::chrono::milliseconds(min_frame_duration_ms);

			// finished up early
			if (frame_used_time < target_end_time) {
				std::this_thread::sleep_for(target_end_time - frame_used_time);
			}
		}
	}
}

void App::CPU_update()
{
	// Camera Rotate
	if (input.key_list[VirtualKeys::RIGHT_MOUSE_BUTTON].down_transition) {

		// glm::vec3 f = { 0, 0, 0 };
		// camera.setCameraFocalPoint(f);
	}
	else if (input.key_list[VirtualKeys::RIGHT_MOUSE_BUTTON].is_down) {

		int32_t delta_x = input.mouse_delta_x;
		int32_t delta_y = input.mouse_delta_y;

		float scaling = camera.orbit_sensitivity * delta_time;
		camera.arcballOrbitCamera((float)delta_x * scaling, (float)delta_y * scaling);

	}
	else if (input.key_list[VirtualKeys::RIGHT_MOUSE_BUTTON].up_transition) {

	}

	// Camera Panning
	if (input.key_list[VirtualKeys::MIDDLE_MOUSE_BUTTON].down_transition) {
		window.setMouseVisibility(false);
		window.trapMousePosition(input.mouse_x, input.mouse_y);
	}
	else if (input.key_list[VirtualKeys::MIDDLE_MOUSE_BUTTON].is_down) {
		int32_t delta_x = input.mouse_delta_x;
		int32_t delta_y = input.mouse_delta_y;

		float scaling = camera.pan_sensitivity * delta_time;
		camera.panCamera((float)-delta_x * scaling, (float)-delta_y * scaling);
	}
	else if (input.key_list[VirtualKeys::MIDDLE_MOUSE_BUTTON].up_transition) {
		window.untrapMousePosition();
		window.setMouseVisibility(true);
	}

	// Camera Dolly
	/*{
		glm::vec3 pixel_world_pos;
		renderer.getPixelWorldPosition(input.mouse_x, input.mouse_y, pixel_world_pos);

		if (pixel_world_pos.x != FLT_MAX) {
			application.setCameraFocus(pixel_world_pos);
		}

		camera.dollyCamera(window->input.mouse_wheel_delta * application.camera_dolly_sensitivity);
	}*/
}
