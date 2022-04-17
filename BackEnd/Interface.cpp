#include "Interface.hpp"

// Mine
#include "App.hpp"
#include "DX12.hpp"


void _stdcall initPixDebugger()
{
	renderer.initPix();
}

void __stdcall init()
{
	// app.init();
	try {
		app.init();
		renderer.init();
	}
	catch(...) {
		__debugbreak();
	}
}

void _stdcall render()
{

}
