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
	renderer.init();
}

void _stdcall render(uint32_t width, uint32_t height, uint8_t* r_pixels)
{
	renderer.render(width, height, r_pixels);
}
