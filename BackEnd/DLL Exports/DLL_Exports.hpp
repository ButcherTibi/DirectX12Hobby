#pragma once

#include <cstdint>


extern "C" {
	
	/// <summary>
	/// Call to load Pix Debugger and enable GPU captures
	/// </summary>
	__declspec(dllexport)
	void _stdcall initPixDebugger();

	/// <summary>
	/// Initialize the backend
	/// </summary>
	__declspec(dllexport)
	void _stdcall init();

	__declspec(dllexport)
	void _stdcall captureFrame();

	__declspec(dllexport)
	void _stdcall tryCopyLastRender(uint32_t width, uint32_t height, uint8_t* r_pixels);
} 
