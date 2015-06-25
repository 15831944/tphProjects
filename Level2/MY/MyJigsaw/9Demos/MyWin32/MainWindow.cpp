#include <windows.h>
#include "WindowsMessageName.h"

LRESULT CALLBACK MainWindowProcdure(HWND hWnd, UINT uInt, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{

    init_map_windowsMessage();
	WNDCLASSEX wcex;
	wcex.cbClsExtra = NULL;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.cbWndExtra = NULL;
	wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hInstance = hInstance;
	wcex.lpfnWndProc = (WNDPROC)MainWindowProcdure;
	wcex.lpszClassName = L"MyMainWindow";
	wcex.lpszMenuName = NULL;
	wcex.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;

	if(!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, L"Register Window Class failed!", L"error", MB_OK | MB_ICONERROR);
		return -1;
	}

	HWND hWnd = NULL;
	hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, L"MyMainWindow", L"Main Window", WS_OVERLAPPEDWINDOW, 0, 0, 300, 300, 
							NULL, NULL, hInstance, NULL);
	if(hWnd == NULL)
	{
		MessageBox(NULL, L"Create Window Failed!", L"error", MB_OK | MB_ICONERROR);
		return -1;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	MSG msg;
	while(GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;
}

LRESULT CALLBACK MainWindowProcdure(HWND hWnd, UINT uInt, WPARAM wParam, LPARAM lParam)
{
    wchar_t buf[256] = {0};
    wsprintf(buf, L"Windows message: (0x%08X) %s\n", uInt, map_windowsMessage[uInt]);
    OutputDebugString(buf);
	switch(uInt)
	{
	case WM_PAINT:
		{
			HDC hDC;
			PAINTSTRUCT ps;
			RECT rect;
			GetClientRect(hWnd, &rect);
			hDC = BeginPaint(hWnd, &ps);
			DrawText(hDC, L"Hello World!", -1, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
			EndPaint(hWnd, &ps);
			return 0;
		}
	case WM_QUIT:
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		return DefWindowProc(hWnd, uInt, wParam, lParam);
	}
}
