#include "./App.hpp"


void App::captureFrame()
{
	std::lock_guard guard(state_update_lock);
	debug.capture_frame = true;
}
