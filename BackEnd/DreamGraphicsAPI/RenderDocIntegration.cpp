// Header
#include "RenderDocIntegration.hpp"

// Standard
#include <format>
using namespace std::string_view_literals;

// Toolbox
#include "ButchersToolbox/Windows/WindowsSpecific.hpp"


RenderDocIntegration render_doc;


std::wstring RenderDocIntegration::init()
{
	this->api = nullptr;

	HMODULE module = GetModuleHandleA("renderdoc.dll");
	if (module == nullptr) {
		return L"renderdoc.dll could not be located";
	}

	pRENDERDOC_GetAPI getApi = (pRENDERDOC_GetAPI)GetProcAddress(module, "RENDERDOC_GetAPI");
	if (getApi == nullptr) {
		return L"Could not find GetAPI procedure in the RenderDoc DLL";
	}

	if (getApi(eRENDERDOC_API_Version_1_5_0, (void**)&api) == false) {
		return L"Could not get RenderDoc API";
	}

	int32_t major;
	int32_t minor;
	int32_t patch;
	api->GetAPIVersion(&major, &minor, &patch);

	return std::format(L"RenderDoc Integration Active API Version {0}.{1}.{2} \n", major, minor, patch);
}

void RenderDocIntegration::startCapture()
{
	if (api == nullptr) {
		return;
	}

	api->StartFrameCapture(nullptr, nullptr);

	win32::printToOutput(L"RenderDoc frame capture started \n");
}

void RenderDocIntegration::endCapture()
{
	if (api == nullptr) {
		return;
	}

	if (api->EndFrameCapture(nullptr, nullptr) == false) {
		throw;
	}

	win32::printToOutput(L"RenderDoc frame capture ended \n");
}

void RenderDocIntegration::triggerCapture()
{
	if (api == nullptr) {
		return;
	}

	api->TriggerCapture();

	win32::printToOutput(L"RenderDoc trigger capture \n");
}

bool RenderDocIntegration::isFrameCapturing()
{
	if (api == nullptr) {
		return false;
	}

	uint32_t is_frame_capturing = api->IsFrameCapturing();
	
	win32::printToOutput(
		std::format(L"RenderDoc is frame capturing = {} \n", is_frame_capturing)
	);
	
	return is_frame_capturing;
}
