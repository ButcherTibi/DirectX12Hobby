#include "App.hpp"

// Standard
#include <thread>


App app;

void App::init()
{
	// Default state
	{
		state.min_frame_duration = chrono::milliseconds(16);
		state.render_width = 250;
		state.render_height = 250;
	}

	prev_state = state;
	next_state = state;

	renderer.init();

	std::jthread main_thread = std::jthread(&App::loop, this);
	main_thread.detach();
}

void App::tick()
{
	// win32::printToOutput(L"ticking \n");

	// Just swap the pointers around like in a swapchain
	{
		std::scoped_lock guard(state_swap_lock);
		prev_state = state;
		state = next_state;
	}

	// CPU time
	{

	}

	// GPU time
	{
		renderer.waitForRendering();

		renderer.downloadRender(state.backbuffer_width, state.backbuffer_height, state.backbuffer);

		RenderWorkload workload;
		workload.width = state.render_width;
		workload.height = state.render_height;
		workload.capture_frame = state.capture_frame;
		renderer.render(workload);

		// End frame capture
		next_state.capture_frame = false;
	}
}

void App::loop()
{
	while (true) {

		auto tick_start = chrono::steady_clock::now();
		tick();
		auto tick_duration = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - tick_start);

		// execution finished early so sleep
		if (tick_duration < state.min_frame_duration) {
			std::this_thread::sleep_for(state.min_frame_duration - tick_duration);
		}
	}
}

void App::captureFrame()
{
	std::scoped_lock guard(state_swap_lock);
	next_state.capture_frame = true;
}

bool App::tryCopyLastRender(u32 width, u32 height, byte* r_pixels)
{
	std::scoped_lock guard(state_swap_lock);
	
	if (prev_state.backbuffer_width != width || prev_state.backbuffer_height != height) {
		next_state.render_width = width;
		next_state.render_height = height;
		return false;
	}

	/*for (uint32_t row = 0; row < width; row++) {
		for (uint32_t col = 0; col < height; col++) {
			r_pixels[row * (width * 4) + (col * 4) + 0] = 0xFF;
			r_pixels[row * (width * 4) + (col * 4) + 3] = 0xFF;
		}
	}*/

	if (width * height * 4 != prev_state.backbuffer.size()) {
		__debugbreak();
	}

	memcpy(r_pixels, prev_state.backbuffer.data(), prev_state.backbuffer.size());
	return true;
}
