#pragma once

// Standard
#include <chrono>
namespace chrono = std::chrono;
#include <mutex>
#include <array>

#include <CommonTypes.hpp>
#include <Renderer/Renderer.hpp>


struct State {
	chrono::milliseconds min_frame_duration;
	u32 render_width;
	u32 render_height;

	std::vector<byte> backbuffer;
	u32 backbuffer_width;
	u32 backbuffer_height;

	bool capture_frame = false;
};


class App {
	std::mutex state_swap_lock;
	State prev_state;
	State state;
	State next_state;

private:
	void tick();
	void loop();

public:
	void init();

	void captureFrame();

	bool tryCopyLastRender(u32 width, u32 height, byte* r_pixels);
};
extern App app;
