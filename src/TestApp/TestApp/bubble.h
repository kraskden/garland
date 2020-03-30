#pragma once

#include <algorithm>
using namespace std;
#include <Windows.h>
#include <Unknwn.h>
#include <objidl.h>

#include <gdiplus.h>

using namespace Gdiplus;

class WindowBubble
{
	HWND hWnd;
	Color currentColor;
	Color lightColor;
	Color transparentColor;
	int width, height;

	bool isActive = false;

public:
	WindowBubble(HWND hWnd);
	
	void turnOn(const Color& color);
	void turnOff();
	void repaint();

	HWND getHWnd() { return hWnd; }

private:
	void paintOnHdc(HDC hdc);
	
};

