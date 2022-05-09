#pragma once

// Standard
#include <chrono>
namespace chrono = std::chrono;
#include <mutex>
#include <array>

#include <CommonTypes.hpp>
#include <Renderer/Renderer.hpp>


struct State {
	u32 render_width = 250;
	u32 render_height = 250;

	bool capture_frame = false;
};


class App {
	std::mutex state_swap_lock;
	State prev_state;
	State state;
	State next_state;

private:
	//void tick();
	//void loop();

public:
	void init();


	/* Aplication loop is made from these methods which have a timing constraint */

	void phase_1_runCPU();
	void phase_2_waitForRendering();
	bool phase_2X_tryDownloadRender(u32 width, u32 height, byte* r_pixels);
	void phase_3_render();


	/* These methods do not require timing */

	void captureFrame();
};
extern App app;
