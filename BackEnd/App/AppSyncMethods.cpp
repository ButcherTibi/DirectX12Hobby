#include "App.hpp"

// Standard
#include <thread>


App app;

void App::init()
{
	// Default state
	{
		
	}

	prev_state = state;
	next_state = state;

	renderer.init();
}

void App::phase_1_runCPU()
{
	// Swap the state
	{
		std::scoped_lock guard(state_swap_lock);
		prev_state = state;
		state = next_state;
	}

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
	RenderWorkload workload;
	workload.width = state.render_width;
	workload.height = state.render_height;
	workload.capture_frame = state.capture_frame;
	renderer.render(workload);

	// End frame capture
	next_state.capture_frame = false;
}

void App::captureFrame()
{
	std::scoped_lock guard(state_swap_lock);
	next_state.capture_frame = true;
}
