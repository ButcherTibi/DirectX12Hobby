#pragma once

#include <cstdint>


extern "C" {
	
	/// <summary>
	/// Call to load Pix Debugger and enable GPU captures
	/// </summary>
	/// <returns></returns>
	__declspec(dllexport)
	void _stdcall initPixDebugger();

	__declspec(dllexport)
	void _stdcall init();

	__declspec(dllexport)
	void _stdcall render();
} 
