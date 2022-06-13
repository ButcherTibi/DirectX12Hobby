#include "Context.hpp"

// PIX Debugger
#include <pix3.h>

#include "../ButchersToolbox/Windows/WindowsSpecific.hpp"


void Context::initPix()
{
	if (PIXLoadLatestWinPixGpuCapturerLibrary() == nullptr) {
		is_pix_debugger_enabled = false;
		win32::printToOutput(win32::getLastError());
		__debugbreak();
	}
	else {
		is_pix_debugger_enabled = true;
	}
}

void Context::beginPixCapture(std::wstring filename)
{
	if (is_pix_debugger_enabled) {

		filename.append(L".wpix");

		PIXCaptureParameters params = {};
		params.GpuCaptureParameters.FileName = filename.c_str();

		if (PIXBeginCapture(PIX_CAPTURE_GPU, &params) != S_OK) {
			__debugbreak();
		}
		else {
			pix_capture_started = true;
		}
	}
}

void Context::endPixCapture()
{
	if (is_pix_debugger_enabled) {

		if (pix_capture_started == false) {
			__debugbreak();
		}

		if (PIXEndCapture(false) != S_OK) {
			__debugbreak();
		}
		else {
			pix_capture_started = false;
		}
	}
}
