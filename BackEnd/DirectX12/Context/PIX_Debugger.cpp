#include <DirectX12/Context/Context.hpp>

// PIX Debugger
#include <pix3.h>

#include <ButchersToolbox/Windows/WindowsSpecific.hpp>


void Context::initPix()
{
	if (PIXLoadLatestWinPixGpuCapturerLibrary() == nullptr) {
		std::wstring last_error = win32::getLastError();
		is_pix_debugger_enabled = false;
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

// typedef HRESULT (__stdcall *PFN_DXGIGetDebugInterface)(REFIID, void**);
// auto hmodule = GetModuleHandle(dxgi_dll_name);
// if (hmodule != nullptr) {
// 
// 	auto func = (PFN_DXGIGetDebugInterface)GetProcAddress(hmodule, "DXGIGetDebugInterface");
// 	checkDX12(func(IID_PPV_ARGS(dxgi_debug.GetAddressOf())));
// }