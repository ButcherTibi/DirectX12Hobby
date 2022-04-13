// Header
#include "Interface.hpp"

#include "DX12.hpp"


void _stdcall initPixDebugger()
{
	hello_world.initPix();
}

void __stdcall initRender()
{
	// app.init();
	try {
		hello_world.init();
	}
	catch(...) {
		__debugbreak();
	}
}
