#include "stdafx.h"
#include "datastructures.h"
#include "Render.h"
#include "GameLogick.h"
#include "resource.h"

#ifdef _DEBUG
#   define log(msg, header) \
        MessageBox(0, msg, header, MB_OK | MB_ICONINFORMATION);
#else
#   define log(msg, header)
#endif

GameSetings g_gameSettings;
GamePtr g_game = nullptr;
RenderContext g_renderContext;



const char WINDOW_CLASS_NAME[] = "Win";

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK DlgProc(HWND, UINT, WPARAM, LPARAM);
ATOM registerClasses(HINSTANCE);


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR szCmdLine, int iCmdShow)
{
	HWND hWnd;
	HWND hDlg;
	MSG msg;

	g_gameSettings.width = 10;
	g_gameSettings.height = 10;

	if (!registerClasses(hInstance)) {
		MessageBox(0, "Ошибка регистрации классов окон!", "Ошибка!", MB_OK | MB_ICONERROR);
		return -1;
	}

	hWnd = CreateWindow(WINDOW_CLASS_NAME,
		"Parent Window",
		WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX | WS_SYSMENU,
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

	hDlg = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), 0, DlgProc);
	if (!hDlg)
	{
		MessageBox(0, "Не удалось создать диалоговое окно!", "Ошибка!", MB_OK | MB_ICONERROR);
		return -3;
	}

	ShowWindow(hDlg, iCmdShow);
	UpdateWindow(hDlg);

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
	case WM_CREATE: {
		createPensAndBrushes();
		g_game = createGame(g_gameSettings.width, g_gameSettings.height);
		g_renderContext = makeRenderContext(hWnd, g_game);
	} break;
	case WM_PAINT: {
		beginPaint(g_renderContext);
		drawWhoMove(g_renderContext, g_game);
		drawScores(g_renderContext, g_game);
		drawHoverPoint(g_renderContext, g_game);
		endPaint(g_renderContext);
	} break;
	case WM_SIZE: {
		updateScreenSize(g_renderContext);
	} break;
	case WM_MOUSEMOVE: {
		updateMouseInfo(g_game, g_renderContext, LOWORD(lParam), HIWORD(lParam));
		if (g_game->isHover) InvalidateRect(hWnd, nullptr, false);
	} break;
	case WM_LBUTTONDOWN: {
		if (!mouseClickProcessing(g_game)) { break; }
		auto color = g_game->whoseMove;

		int x = g_game->mouseHover.first;
		int y = g_game->mouseHover.second;
		repaintPoint(g_renderContext, x, y, color);

		auto points = checkingExistenceOfRing(g_game);
		if (points.size() >= 4)
		{
			modifyExistenceOfRing(points);
			if (points.size() >= 4)
			{
				auto other = findOtherColors(g_game, points);
				if (other.size() > 0)
				{
					paintLines(g_renderContext, points, color);
				}
			}
		}

		if (g_game->freePoints == 0)
		{
			if (g_game->blueScore > g_game->redScore)
			{
				MessageBox(hWnd, "Ура, победил членовек\nиз синей команды!", "Blue team win!", MB_OK | MB_ICONINFORMATION);
			}
			else if (g_game->blueScore < g_game->redScore)
			{
				MessageBox(hWnd, "Ура, победил членовек\nиз красной команды!", "Red team win!", MB_OK | MB_ICONINFORMATION);
			}
			else
			{
				MessageBox(hWnd, "О магад, победила дружба!", "Jesus win!", MB_OK | MB_ICONINFORMATION);
			}	
		}

		g_game->whoseMove = g_game->whoseMove == Color_Red ? Color_Blue : Color_Red;
	} break;
	case WM_ERASEBKGND: {} break;
	case WM_DESTROY: {
		destroyPensAndBrushes();
	} break;

	default: return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

BOOL CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG: {
		SetDlgItemInt(hWnd, EDIT_HEIGHT, g_gameSettings.height, true);
		SetDlgItemInt(hWnd, EDIT_WIDTH_2, g_gameSettings.width, true);
		SendDlgItemMessage(hWnd, RADIO_PVP, BM_SETCHECK, 1, 0);
	} break;
	case WM_COMMAND: {
		switch (LOWORD(wParam))
		{
		case BUTTON_HEIGHT_ADD: {
			if (g_gameSettings.height == 20) { break; }
			g_gameSettings.height++;
			SetDlgItemInt(hWnd, EDIT_HEIGHT, g_gameSettings.height, true);
		} break;
		case BUTTON_HEIGHT_SUB: {
			if (g_gameSettings.height == 10) { break; }
			g_gameSettings.height--;
			SetDlgItemInt(hWnd, EDIT_HEIGHT, g_gameSettings.height, true);
		} break;
		case BUTTON_WIDTH_ADD: {
			if (g_gameSettings.width == 30) { break; }
			g_gameSettings.width++;
			SetDlgItemInt(hWnd, EDIT_WIDTH_2, g_gameSettings.width, true);
		} break;
		case BUTTON_WIDTH_SUB: {
			if (g_gameSettings.width == 10) { break; }
			g_gameSettings.width--;
			SetDlgItemInt(hWnd, EDIT_WIDTH_2, g_gameSettings.width, true);
		} break;
		case IDOK: {
			g_game = createGame(g_gameSettings.width, g_gameSettings.height);
			g_renderContext = makeRenderContext(g_renderContext.hWnd, g_game);
			SetWindowPos(g_renderContext.hWnd, 0, 0, 0,
				g_renderContext.baseBuffer->width, g_renderContext.baseBuffer->height,
				SWP_NOMOVE | SWP_NOZORDER
			);
			UpdateWindow(g_renderContext.hWnd);
		} break;
		case CHECK_SOUNDS: {
			if (SendDlgItemMessage(hWnd, CHECK_SOUNDS, BM_GETCHECK, 0, 0))
			{
				PlaySound(MAKEINTRESOURCE(IDR_WAVE1), 0, SND_RESOURCE | SND_ASYNC | SND_MEMORY | SND_LOOP);
			}
			else
			{
				PlaySound(0, 0, SND_ASYNC);
			}
		} break;
		}
	} break;
	case WM_CLOSE: {
		PostQuitMessage(0);
	} break;
	default: return FALSE;
	}
	return TRUE;
}

ATOM registerClasses(HINSTANCE hInstance)
{
	WNDCLASSEX wndclass;

	wndclass.cbSize = sizeof(wndclass);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_NOCLOSE;
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