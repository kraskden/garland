#include <boost/asio.hpp>

using namespace std;
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>

#include "bubble.h"
#include "defines.h"
#include "connection.h"

using namespace Gdiplus;
#pragma comment (lib,"Gdiplus.lib")


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR, INT iCmdShow)
{
	HWND                hWnd;
	MSG                 msg;
	WNDCLASS            wndClass;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR           gdiplusToken;

	// Initialize GDI+.
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = hInstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = TEXT("LightBubble");

	RegisterClass(&wndClass);

	hWnd = CreateWindow(
		TEXT("LightBubble"),   // window class name
		TEXT("Light Bubble"),  // window caption
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT,            // initial x position
		CW_USEDEFAULT,            // initial y position
		windowWidth,            // initial x size
		windowHeight,            // initial y size
		NULL,                     // parent window handle
		NULL,                     // window menu handle
		hInstance,                // program instance handle
		NULL);                    // creation parameters

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	GdiplusShutdown(gdiplusToken);
	return msg.wParam;
}  // WinMain

WindowBubble* bubble;
boost::asio::io_service service;
boost::shared_ptr<Connection> connection;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
	WPARAM wParam, LPARAM lParam)
{
	ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8001);
	switch (message)
	{
	case WM_CREATE:
		bubble = new WindowBubble(hWnd);
		SetTimer(hWnd, 1, 50, NULL);
		connection = Connection::create(service, bubble);
		connection->start(ep);
	case WM_PAINT:
		bubble->repaint();
		return 0;
	case WM_DESTROY:
		delete bubble;
		service.stop();
		PostQuitMessage(0);
		return 0;
	case WM_TIMER:
		service.poll();
		return 0;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
} // WndProc