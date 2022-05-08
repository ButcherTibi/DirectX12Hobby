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
	app.init();
}

void _stdcall captureFrame()
{
	app.captureFrame();
}

void _stdcall tryCopyLastRender(uint32_t width, uint32_t height, uint8_t* r_pixels)
{
	app.tryCopyLastRender(width, height, r_pixels);
}
