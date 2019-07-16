#include "splash_screen.h"

#include <cstdint>
#include <thread>
#include <Windows.h>

HWND hWnd;
HBITMAP hBmp;

LRESULT CALLBACK SplashWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hMemoryDC;
	uint32_t progress_width = 0;

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

		// Remove pen
		SelectObject((HDC)wParam, GetStockObject(NULL_PEN));

		// Print the bmp to the window
		BitBlt((HDC)wParam, 0, 0, SPLASH_WIDTH, SPLASH_HEIGHT, hMemoryDC, 0, 0, SRCCOPY);

		// Delete the memory
		DeleteObject(hMemoryDC);

		// Calc progress width
		progress_width = ((float)gtamp::launcher::splash_screen::get_progress() / 100) * SPLASH_WIDTH;

		// Draw progress bg
		SelectObject((HDC)wParam, CreateSolidBrush(RGB(PROGRESS_BAR_BG_RED, PROGRESS_BAR_BG_GREEN, PROGRESS_BAR_BG_BLUE)));
		Rectangle((HDC)wParam, 0, SPLASH_HEIGHT, SPLASH_WIDTH + 1, SPLASH_HEIGHT + PROGRESS_BAR_HEIGHT + 1);

		// Draw progress fg
		SelectObject((HDC)wParam, CreateSolidBrush(RGB(PROGRESS_BAR_FG_RED, PROGRESS_BAR_FG_GREEN, PROGRESS_BAR_FG_BLUE)));
		Rectangle((HDC)wParam, 0, SPLASH_HEIGHT, progress_width + 1, SPLASH_HEIGHT + PROGRESS_BAR_HEIGHT + 1);

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
	hWnd = CreateWindow(TEXT("GTAMPSplashScreen"), NULL, WS_CAPTION | WS_SYSMENU | BS_BITMAP | WS_EX_TOPMOST, CW_USEDEFAULT, CW_USEDEFAULT, SPLASH_SCREEN_WIDTH, SPLASH_SCREEN_HEIGHT, 0, 0, GetModuleHandle(NULL), NULL);

	// Hide from taskbar
	SetWindowLongPtr(hWnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW);
	
	// Remove borders
	SetWindowLongPtr(hWnd, GWL_STYLE, WS_POPUP | WS_MINIMIZEBOX);

	// Get the pos of the window by the size of the screen
	x = (GetSystemMetrics(SM_CXSCREEN) - SPLASH_SCREEN_WIDTH) / 2;
	y = (GetSystemMetrics(SM_CYSCREEN) - SPLASH_SCREEN_HEIGHT) / 2;

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

void gtamp::launcher::splash_screen::set_progress(int progress)
{
	_progress = progress;

	// Update splash window
	InvalidateRect(hWnd, NULL, true);
}

int gtamp::launcher::splash_screen::get_progress()
{
	return _progress;
}