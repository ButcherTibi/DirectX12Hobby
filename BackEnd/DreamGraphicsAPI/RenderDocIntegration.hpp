#pragma once

// Standard
#include <string>

// RenderDoc
#include "./renderdoc_app.h"


class RenderDocIntegration {
public:
	RENDERDOC_API_1_5_0* api;

public:
	std::wstring init();

	void startCapture();
	void endCapture();
	void triggerCapture();

	bool isFrameCapturing();
};

extern RenderDocIntegration render_doc;
