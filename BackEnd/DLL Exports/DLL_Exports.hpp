#pragma once

// Standard
#include <cstdint>

// Mine
#include <App/App.hpp>


extern "C" {
	
	/// <summary>
	/// Initialize the backend
	/// </summary>
	__declspec(dllexport)
		void _stdcall init(bool enable_pix_debugger)
	{
		app.init(enable_pix_debugger);
	}


	/*__declspec(dllexport)
	bool _stdcall phase_2X_tryDownloadRender(uint32_t width, uint32_t height, uint8_t* r_pixels)
	{
		return app.phase_2X_tryDownloadRender(width, height, r_pixels);
	}*/


	/* These methods do not require timing */

	__declspec(dllexport)
	void _stdcall captureFrame()
	{
		app.captureFrame();
	}
} 
