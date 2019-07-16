#include "splash_screen.h"

#include <cstdint>
#include <thread>
#include <Windows.h>

HWND hWnd;
HBITMAP hBmp;

LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hMemoryDC;

	switch (uMsg)
	{
	case WM_CREATE:
		// Load the splash screen image
		hBmp = (HBITMAP)LoadImage(NULL, TEXT(SPLASH_PATH), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

		break;

	case WM_ERASEBKGND:
		// Get the bmp source
		hMemoryDC = CreateCompatibleDC((HDC)wParam);
		SelectObject(hMemoryDC, (HGDIOBJ)hBmp);

		// Print the bmp to the window
		BitBlt((HDC)wParam, 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, hMemoryDC, 0, 0, SRCCOPY);

		// Delete the memory
		DeleteObject(hMemoryDC);

		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

void gtamp::launcher::splash_screen::show()
{
	WNDCLASSEX splash_wc;

	std::thread ui_thread;

	// Init window class
	splash_wc.cbSize = sizeof(WNDCLASSEX);
	splash_wc.style = 0;
	splash_wc.lpfnWndProc = SplashWndProc;
	splash_wc.cbClsExtra = 0;
	splash_wc.cbWndExtra = 0;
	splash_wc.hInstance = GetModuleHandle(NULL);
	splash_wc.hIcon = NULL;
	splash_wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	splash_wc.hbrBackground = NULL;
	splash_wc.lpszMenuName = NULL;
	splash_wc.lpszClassName = TEXT("GTAMPSplashScreen");
	splash_wc.hIconSm = NULL;

	// Register the window class
	RegisterClassEx(&splash_wc);

	// Create the UI thread and detach it
	ui_thread = std::thread(&gtamp::launcher::splash_screen::ui_loop);
	ui_thread.detach();
}

void gtamp::launcher::splash_screen::close()
{
	_destroy = true;
}

void gtamp::launcher::splash_screen::ui_loop()
{
	int32_t x, y;

	MSG msg;

	// Create the splash window
	hWnd = CreateWindow(TEXT("GTAMPSplashScreen"), NULL, WS_POPUPWINDOW | WS_EX_TOPMOST, CW_USEDEFAULT, CW_USEDEFAULT, SPLASH_WIDTH, SPLASH_HEIGHT, 0, 0, GetModuleHandle(NULL), NULL);

	// Hide from taskbar
	SetWindowLong(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);

	// Get the pos of the window by the size of the screen
	x = (GetSystemMetrics(SM_CXSCREEN) - SPLASH_WIDTH) / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - SPLASH_HEIGHT) / 2;

	// Set the pos of the window
	SetWindowPos(hWnd, HWND_TOPMOST, x, y, 0, 0, SWP_NOSIZE);

	// Show the splash screen
	ShowWindow(hWnd, SW_SHOW);

	// Handle UI messages
	while (GetMessage(&msg, NULL, 0, 0) > 0 && !_destroy)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// Destroy the window and the bitmap
	DeleteObject((HGDIOBJ)hBmp);
	DestroyWindow(hWnd);
}