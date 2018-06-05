#include "stdafx.h"
#include "Render.h"
#include "Constants.h"

struct RenderInstruments {
	bool isCreated = false;
	HPEN hWhitePen;
	HPEN hBlackPen;
	HPEN hRedPen;
	HPEN hBluePen;

	HBRUSH hWhiteBrush;
	HBRUSH hBlackBrush;
	HBRUSH hRedBrush;
	HBRUSH hBlueBrush;
} g_RenderInstruments;


void copyStaticBuffer(StaticBufferPtr& to, StaticBufferPtr& from);
void copyStaticBuffer(HDC hDC_to, StaticBufferPtr& from);
void initialDrawField(RenderContext& renderContext, GamePtr& game);
void drawLine(HDC hDC, int start_x, int start_y, int end_x, int end_y);


void releaseStaticBuffer(StaticBuffer& buffer)
{
	DeleteDC(buffer.hDC);
	DeleteObject(buffer.hBmp);

	buffer.hDC = 0;
	buffer.hBmp = 0;
}

StaticBufferPtr makeStaticBuffer(HDC hBaseDC, GamePtr& game)
{
	auto buffer = StaticBufferPtr(new StaticBuffer(), releaseStaticBuffer);

	buffer->width = g_freeBorderSone * 2 + g_CellSize * (game->width - 1) + (g_PointRadius * 2) * game->width;
	buffer->height = g_freeBorderSone * 2 + g_CellSize * (game->height - 1) + (g_PointRadius * 2) * game->height;

	
	buffer->hDC = CreateCompatibleDC(hBaseDC);
	buffer->hBmp = CreateCompatibleBitmap(buffer->hDC, buffer->width, buffer->height);
	SelectObject(buffer->hDC, buffer->hBmp);

	return buffer;
}





RenderContext makeRenderContext(HWND hWnd, GamePtr& game)
{
	RenderContext renderCtx;

	renderCtx.hWnd = hWnd;
	HDC hBaseDC = GetDC(hWnd);
	renderCtx.hBaseDC = hBaseDC;
	renderCtx.baseBuffer = makeStaticBuffer(hBaseDC, game);
	renderCtx.activeBuffer = makeStaticBuffer(hBaseDC, game);

	initialDrawField(renderCtx, game);
	ReleaseDC(hWnd, hBaseDC);
}


void drawLine(HDC hDC, int start_x, int start_y, int end_x, int end_y)
{
	POINT point;
	MoveToEx(hDC, start_x, start_y, &point);
	LineTo(hDC, end_x, end_y);
	MoveToEx(hDC, point.x, point.y, nullptr);
}

void initialDrawField(RenderContext& renderContext, GamePtr& game)
{
	SelectObject(renderContext.baseBuffer->hDC, g_RenderInstruments.hBlackPen);
	SelectObject(renderContext.baseBuffer->hDC, g_RenderInstruments.hWhiteBrush);

	RECT rect;
	rect.right = renderContext.baseBuffer->width;
	rect.bottom = renderContext.baseBuffer->height;

	FillRect(renderContext.baseBuffer->hDC, &rect, g_RenderInstruments.hWhiteBrush);

	int start_x = g_freeBorderSone;
	int end_x = renderContext.baseBuffer->width - g_freeBorderSone;

	int start_y = g_freeBorderSone;
	int end_y = renderContext.baseBuffer->height - g_freeBorderSone;

	int x = start_x;
	for (int i = 0; i < game->width; i++)
	{
		drawLine(renderContext.baseBuffer->hDC, x, start_y, x, end_y);
		x += g_CellSize;
	}

	int y = start_y;
	for (int i = 0; i < game->height; i++)
	{
		drawLine(renderContext.baseBuffer->hDC, start_x, y, end_x, y);
		y += g_CellSize;
	}

	int pointWidth = g_PointRadius * 2;
	y = start_y - g_PointRadius;
	for (int point_y = 0; point_y < game->height; point_y++)
	{
		x = start_x - g_PointRadius;
		for (int point_x = 0; point_x < game->width; point_x++)
		{
			Ellipse(renderContext.baseBuffer->hDC, x, y, x + pointWidth, y + pointWidth);
			x += g_CellSize;
		}
		y += g_CellSize;
	}
}

void createPensAndBrushes()
{
	if (g_RenderInstruments.isCreated) { return; }
	g_RenderInstruments.isCreated = true;

	g_RenderInstruments.hWhitePen = CreatePen(PS_SOLID, g_BorderLineWidth, RGB(255, 255, 255));
	g_RenderInstruments.hBlackPen = CreatePen(PS_SOLID, g_BaseLineWidth, RGB(0, 0, 0));
	g_RenderInstruments.hRedPen = CreatePen(PS_SOLID, g_BorderLineWidth, g_RedColor);
	g_RenderInstruments.hBluePen = CreatePen(PS_SOLID, g_BorderLineWidth, g_BlueColor);
	
	g_RenderInstruments.hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	g_RenderInstruments.hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	g_RenderInstruments.hRedBrush = CreateHatchBrush(HS_DIAGCROSS, g_RedFillColor);
	g_RenderInstruments.hBlueBrush = CreateSolidBrush(HS_DIAGCROSS, g_BlueFillColor);
}

void destroyPensAndBrushes()
{
	if (!g_RenderInstruments.isCreated) { return; }
	g_RenderInstruments.isCreated = false;

	DeleteObject(g_RenderInstruments.hWhitePen);
	DeleteObject(g_RenderInstruments.hBlackPen);
	DeleteObject(g_RenderInstruments.hRedPen);
	DeleteObject(g_RenderInstruments.hBluePen);

	DeleteObject(g_RenderInstruments.hWhiteBrush);
	DeleteObject(g_RenderInstruments.hBlackBrush);
	DeleteObject(g_RenderInstruments.hRedBrush);
	DeleteObject(g_RenderInstruments.hBlueBrush);
}


void beginPaint(RenderContext& renderContext)
{
	renderContext.hBaseDC = BeginPaint(renderContext.hWnd, &renderContext.ps);
	copyStaticBuffer(renderContext.activeBuffer, renderContext.baseBuffer);
}

void endPaint(RenderContext& renderContext)
{
	copyStaticBuffer(renderContext.hBaseDC, renderContext.baseBuffer);
	EndPaint(renderContext.hWnd, &renderContext.ps);
}

void copyStaticBuffer(StaticBufferPtr& to, StaticBufferPtr& from)
{
	BitBlt(to->hDC, 0, 0, to->width, to->width, from->hDC, 0, 0, SRCCOPY);
}

void copyStaticBuffer(HDC hDC_to, StaticBufferPtr& from)
{
	BitBlt(hDC_to, 0, 0, from->width, from->width, from->hDC, 0, 0, SRCCOPY);
}
