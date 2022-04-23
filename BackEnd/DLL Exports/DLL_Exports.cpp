#include "DLL_Exports.hpp"

// Mine
#include "App.hpp"
#include <Renderer/Renderer.hpp>


void _stdcall initPixDebugger()
{
	Context::initPix();
}

void __stdcall init()
{
	// app.init();
	try {
		Renderer::init();
	}
	catch(...) {
		__debugbreak();
	}
}

void _stdcall render()
{

}
