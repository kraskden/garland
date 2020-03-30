#include "bubble.h"

WindowBubble::WindowBubble(HWND hWnd) : hWnd(hWnd)
{
	RECT clientRect;
	GetClientRect(hWnd, &clientRect);
	width = clientRect.right;
	height = clientRect.bottom;
	transparentColor.SetValue(GetSysColor(COLOR_WINDOW));
	currentColor = transparentColor;
}

void WindowBubble::turnOn(const Gdiplus::Color& color)
{
	isActive = true;
	currentColor = color;
	InvalidateRect(hWnd, NULL, true);
}

void WindowBubble::turnOff()
{
	isActive = false;
	currentColor = transparentColor;
	InvalidateRect(hWnd, NULL, true);
}

void WindowBubble::repaint()
{
	PAINTSTRUCT ps;
	HDC hdc;
	hdc = BeginPaint(hWnd, &ps);
	paintOnHdc(hdc);
	EndPaint(hWnd, &ps);
}

void WindowBubble::paintOnHdc(HDC hdc)
{
	Graphics graphics(hdc);
	SolidBrush brush(currentColor);
	graphics.FillEllipse(&brush, 0, 0, width, height);
}
