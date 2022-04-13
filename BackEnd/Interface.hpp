#pragma once

#include <cstdint>


extern "C" {
	
	__declspec(dllexport)
	void _stdcall initPixDebugger();

	__declspec(dllexport)
	void _stdcall initRender();
} 
