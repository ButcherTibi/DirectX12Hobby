#pragma once

// Standard
#include <cstdint>

// Mine
#include <App/App.hpp>


extern "C" {
	
	/// <summary>
	/// Call to load Pix Debugger and enable GPU captures
	/// </summary>
	__declspec(dllexport)
	void _stdcall initPixDebugger()
	{
		Context::initPix();
	}

	/// <summary>
	/// Initialize the backend
	/// </summary>
	__declspec(dllexport)
		void _stdcall init()
	{
		app.init();
	}


	/* Aplication loop is made from these methods which have a timing constraint */

	__declspec(dllexport)
	void _stdcall phase_1_runCPU()
	{
		app.phase_1_runCPU();
	}

	__declspec(dllexport)
	void _stdcall phase_2_waitForRendering()
	{
		app.phase_2_waitForRendering();
	}

	__declspec(dllexport)
	bool _stdcall phase_2X_tryDownloadRender(uint32_t width, uint32_t height, uint8_t* r_pixels)
	{
		return app.phase_2X_tryDownloadRender(width, height, r_pixels);
	}

	__declspec(dllexport)
	void _stdcall phase_3_render()
	{
		app.phase_3_render();
	}


	/* These methods do not require timing */

	__declspec(dllexport)
	void _stdcall captureFrame()
	{
		app.captureFrame();
	}
} 
