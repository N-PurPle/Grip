

#include <tchar.h>
#include <Windows.h>
#include <algorithm>
#include <thread>
#include <Grip.hpp>
#include <sstream>
#include "GameSample.h"


#define UNUSED(x)


int g_iX = 0;
int g_iY = 0;
int g_iWidth = 1280;
int g_iHeight = 720;
bool g_bUseOpenGL = false;
bool g_bIsFullScreen = false;
HICON g_hIcon = nullptr;
HICON g_hIconSmall = nullptr;
HCURSOR g_hCursor = nullptr;
HMENU g_hMenu = nullptr;
HWND g_hWnd = nullptr;
TCHAR g_pTitle[] = _T("Default");


#define WINDOW_CLASS_NAME _T("Default")


HWND Initialize(HINSTANCE hInstance, int nCmdShow, WNDPROC lpWndProc)
{
	DWORD windowStyle = 0;
	DWORD windowStyleEx = 0;
	LONG adjustedWidth = static_cast<LONG>(g_iWidth);
	LONG adjustedHeight = static_cast<LONG>(g_iHeight);

	if (g_bUseOpenGL)
	{
		windowStyle |= WS_CLIPCHILDREN;
		windowStyle |= WS_CLIPSIBLINGS;
	}

	if (g_bIsFullScreen)
	{
		windowStyleEx |= WS_EX_TOPMOST;
		windowStyle |= WS_POPUP;
		g_iX = 0;
		g_iY = 0;
	}
	else
	{
		DWORD const fixedWindowStyle(WS_OVERLAPPED | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
		windowStyle |= fixedWindowStyle;

		RECT windowRect = { 0, 0, static_cast<LONG>(g_iWidth), static_cast<LONG>(g_iHeight) };
		::AdjustWindowRect(&windowRect, windowStyle, FALSE);

		adjustedWidth = windowRect.right - windowRect.left;
		adjustedHeight = windowRect.bottom - windowRect.top;
	}

	UINT windowClassStyle = (CS_HREDRAW | CS_VREDRAW);

	if (g_bUseOpenGL)
	{
		windowClassStyle |= CS_OWNDC;
	}

	if (g_hIcon == nullptr)
	{
		g_hIcon = reinterpret_cast<HICON>(::LoadImage(
			hInstance,
			_T("GRIPICON"),
			IMAGE_ICON,
			256,
			256,
			LR_DEFAULTSIZE | LR_SHARED));
	}

	if (g_hIconSmall == nullptr)
	{
		g_hIconSmall = reinterpret_cast<HICON>(::LoadImage(
			hInstance,
			_T("GRIPICON"),
			IMAGE_ICON,
			16,
			16,
			LR_DEFAULTSIZE | LR_SHARED));
	}

	if (g_hCursor == nullptr)
	{
		g_hCursor = reinterpret_cast<HCURSOR>(::LoadImage(
			hInstance,
			_T(""),
			IMAGE_CURSOR,
			0,
			0,
			LR_DEFAULTSIZE | LR_SHARED));
	}

	WNDCLASSEX wcex = { 0 };
	if (::GetClassInfoEx(hInstance, WINDOW_CLASS_NAME, &wcex) == 0)
	{
		wcex = WNDCLASSEX{
			sizeof(WNDCLASSEX),
			windowClassStyle,
			lpWndProc,
			0, 0,
			hInstance,
			g_hIcon,
			g_hCursor,
			static_cast<HBRUSH>(::GetStockObject(WHITE_BRUSH)),
			nullptr, // menu name
			WINDOW_CLASS_NAME,
			g_hIconSmall
		};
		if (0 == ::RegisterClassEx(&wcex))
		{
			return nullptr;
		}
	}

	// create window
	g_hWnd = ::CreateWindowEx(
		windowStyleEx,
		wcex.lpszClassName,
		g_pTitle,
		windowStyle,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		adjustedWidth,
		adjustedHeight,
		nullptr,
		g_hMenu,
		hInstance,
		nullptr);

	if (g_hWnd == nullptr)
	{
		return nullptr;
	}

	{
		if (nCmdShow == SW_MAXIMIZE)
		{
			::ShowWindow(g_hWnd, SW_RESTORE);
		}
		else
		{
			::ShowWindow(g_hWnd, nCmdShow);
		}
	}

	{
		POINT point = { 0, 0 };
		if (0 != ::ClientToScreen(g_hWnd, &point))
		{
			g_iX = static_cast<int>(point.x);
			g_iY = static_cast<int>(point.y);
		}
	}

	::UpdateWindow(g_hWnd);             //表示を初期化
	::SetFocus(g_hWnd);                 //フォーカスを設定

	// 常に最前面に設定
	//::SetWindowPos(g_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	//::SetWindowLongPtr(g_hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	return g_hWnd;
}



LRESULT APIENTRY WindowProcedure(
	HWND hWnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
)
{
	switch (uMsg)
	{
	case WM_CREATE:
		break;
	}
	return ::DefWindowProc(hWnd, uMsg, wParam, lParam);
}



int APIENTRY _tWinMain(
	HINSTANCE hInstance,
	HINSTANCE UNUSED(hPrevInstance),
	LPTSTR    UNUSED(lpCmdLine),
	int       nCmdShow
)
{
	HWND hWnd = Initialize(hInstance, nCmdShow, WindowProcedure);
	if (hWnd == nullptr) {
		return 0;
	}

	MSG msg = { 0 };
	GameSample sample;
	Grip::Framework framework(&sample, ::GetModuleHandle(nullptr), g_hWnd);
	while (true)
	{
		while (::PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE) != 0)
		{
			if (::GetMessage(&msg, hWnd, 0, 0) == 0) break;
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}

		if (::IsWindow(hWnd) == 0) { break; }

		if (framework.Update()) {
			framework.Draw();
		}

		if (framework.IsExit()) { break; }
	}

	return 0;
}




