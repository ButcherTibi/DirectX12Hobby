#include "DX12.hpp"

#include <format>

#include "ButchersToolbox/Windows/WindowsSpecific.hpp"


//void HelloTriangle::_createStubWindow()
//{
//	// Initialize the window class.
//	auto window_class_name = L"DXSampleClass";
//
//	WNDCLASSEX window_class = {};
//	window_class.cbSize = sizeof(WNDCLASSEX);
//	window_class.style = CS_HREDRAW | CS_VREDRAW;
//	window_class.lpfnWndProc = _stubWindowProc;
//	window_class.hInstance = GetModuleHandle(NULL);;
//	window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
//	window_class.lpszClassName = L"DXSampleClass";
//
//	if (RegisterClassEx(&window_class) == 0) {
//		__debugbreak();
//	}
//
//	// Create the window and store a handle to it.
//	this->window_handle = CreateWindowEx(
//		0,
//		window_class_name,
//		L"This is what you do when you can't capture headless",
//		WS_OVERLAPPEDWINDOW,
//		CW_USEDEFAULT, CW_USEDEFAULT,  // position
//		1024, 720,
//		nullptr,
//		nullptr,
//		hinstance,
//		nullptr);
//
//	ShowWindow(window_handle, SW_NORMAL);
//}
//
//void HelloTriangle::updateStubWindow()
//{
//	win32::printToOutput(L"started \n");
//
//	called = 0;
//
//    // process window messages
//    MSG msg = {};
//    if (PeekMessage(&msg, window_handle, 0, 0, PM_REMOVE)) {
//
//        TranslateMessage(&msg);
//        DispatchMessage(&msg);
//
//		/*if (msg.message == WM_PAINT) {
//			break;
//		}*/
//    }
//
//	win32::printToOutput(L"ended \n");
//}
//
//LRESULT CALLBACK HelloTriangle::_stubWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//    switch (message) {
//    case WM_PAINT: {
//		hello_world.called++;
//		win32::printToOutput(std::format(L"window handle = {0} \n", (int64_t)hWnd));
//        return 0;
//    }
//    case WM_DESTROY: {
//        PostQuitMessage(0);
//        return 0;
//    }    
//    }
//
//	return DefWindowProc(hWnd, message, wParam, lParam);
//}
