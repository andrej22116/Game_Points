#include "stdafx.h"
#include "datastructures.h"
#include "Render.h"
#include "GameLogick.h"

#ifdef _DEBUG
#   define log(msg, header) \
        MessageBox(0, msg, header, MB_OK | MB_ICONINFORMATION);
#else
#   define log(msg, header)
#endif


const char WINDOW_CLASS_NAME[] = "Win";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM registerClasses(HINSTANCE);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hWnd;
	MSG msg;

	if (!registerClasses(hInstance)) {
		MessageBox(0, "Ошибка регистрации классов окон!", "Ошибка!", MB_OK | MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(WINDOW_CLASS_NAME,
		"Parent Window",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		800,
		600,
		NULL,
		NULL,
		hInstance,
		NULL);
	if (!hWnd) {
		MessageBox(0, "Не удалось создать окно!", "Ошибка!", MB_OK | MB_ICONERROR);
		return -2;
	}

	ShowWindow(hWnd, iCmdShow);
	UpdateWindow(hWnd);

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: {} break;
	case WM_PAINT: {} break;
	case WM_DESTROY: {
		PostQuitMessage(0);
	} break;

	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

ATOM registerClasses(HINSTANCE hInstance)
{
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hInstance;
	wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)GetStockObject(0);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = WINDOW_CLASS_NAME;
	wndclass.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	return RegisterClassEx(&wndclass);
}