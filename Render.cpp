#include "stdafx.h"
#include "Render.h"
#include "Constants.h"

struct RenderInstruments {
	bool isCreated = false;
	HPEN hWhitePen;
	HPEN hBlackPen;
	HPEN hRedPen;
	HPEN hRedLightPen;
	HPEN hBluePen;
	HPEN hBlueLightPen;

	HBRUSH hWhiteBrush;
	HBRUSH hBlackBrush;
	HBRUSH hRedBrush;
	HBRUSH hRedLightBrush;
	HBRUSH hBlueBrush;
	HBRUSH hBlueLightBrush;

	HBRUSH hGrayBrush;

	HFONT hFont;
} g_RenderInstruments;


void copyStaticBuffer(StaticBufferPtr& to, StaticBufferPtr& from);
void copyStaticBuffer(HDC hDC_to, StaticBufferPtr& fromm, int x, int y);
void initialDrawField(RenderContext& renderContext, GamePtr& game);
void drawLine(HDC hDC, int start_x, int start_y, int end_x, int end_y);
void drawPoints(HDC hDC, GamePtr& game);
void drawPoint(HDC hDC, int x, int y, PointColor color);


void releaseStaticBuffer(StaticBuffer* buffer)
{
	DeleteDC(buffer->hDC);
	DeleteObject(buffer->hBmp);

	buffer->hDC = 0;
	buffer->hBmp = 0;
}

StaticBufferPtr makeStaticBuffer(HDC hBaseDC, int width, int height)
{
	auto buffer = StaticBufferPtr(new StaticBuffer(), releaseStaticBuffer);

	buffer->width = width;
	buffer->height = height;
	
	buffer->hDC = CreateCompatibleDC(hBaseDC);
	buffer->hBmp = CreateCompatibleBitmap(hBaseDC, buffer->width, buffer->height);
	SelectObject(buffer->hDC, buffer->hBmp);

	return buffer;
}





RenderContext makeRenderContext(HWND hWnd, GamePtr& game)
{
	RenderContext renderCtx;

	renderCtx.hWnd = hWnd;
	HDC hBaseDC = GetDC(hWnd);
	renderCtx.hBaseDC = hBaseDC;

	int gameBufferWidth = (g_freeBorderSone * 2) + g_CellSize * (game->width - 1);
	int gameBufferHeight = (g_freeBorderSone * 2) + g_CellSize * (game->height - 1);
	renderCtx.baseBuffer = makeStaticBuffer(hBaseDC, gameBufferWidth, gameBufferHeight);
	renderCtx.activeBuffer = makeStaticBuffer(hBaseDC, gameBufferWidth, gameBufferHeight);

	GetClientRect(hWnd, &renderCtx.clientRect);
	renderCtx.screenBuffer = makeStaticBuffer(hBaseDC, renderCtx.clientRect.right, renderCtx.clientRect.bottom);

	initialDrawField(renderCtx, game);
	ReleaseDC(hWnd, hBaseDC);

	return renderCtx;
}

void updateScreenSize(RenderContext& renderContext)
{
	GetClientRect(renderContext.hWnd, &renderContext.clientRect);
	HDC hBaseDC = GetDC(renderContext.hWnd);
	renderContext.screenBuffer = makeStaticBuffer(
		hBaseDC, 
		renderContext.clientRect.right,
		renderContext.clientRect.bottom
	);
	ReleaseDC(renderContext.hWnd, hBaseDC);

	renderContext.fieldPos_x = renderContext.clientRect.right / 2 - renderContext.baseBuffer->width / 2;
	renderContext.fieldPos_y = renderContext.clientRect.bottom / 2 - renderContext.baseBuffer->height / 2;
}


void drawLine(HDC hDC, int start_x, int start_y, int end_x, int end_y)
{
	POINT point;
	MoveToEx(hDC, start_x, start_y, &point);
	LineTo(hDC, end_x, end_y);
	MoveToEx(hDC, point.x, point.y, nullptr);
}

inline void drawPoint(HDC hDC, int x, int y, PointColor color)
{
	int radius = g_PointRadius;
	switch (color)
	{
	case Color_Neutral: {
		SelectObject(hDC, g_RenderInstruments.hWhiteBrush);
		SelectObject(hDC, g_RenderInstruments.hBlackPen);
	} break;
	case Color_Red: {
		radius = g_PointBigRadius;
		SelectObject(hDC, g_RenderInstruments.hRedBrush);
		SelectObject(hDC, g_RenderInstruments.hRedPen);
	} break;
	case Color_Blue: {
		radius = g_PointBigRadius;
		SelectObject(hDC, g_RenderInstruments.hBlueBrush);
		SelectObject(hDC, g_RenderInstruments.hBluePen);
	} break;
	case Color_LightRed: {
		radius = g_PointBigRadius;
		SelectObject(hDC, g_RenderInstruments.hRedLightBrush);
		SelectObject(hDC, g_RenderInstruments.hRedLightPen);
	} break;
	case Color_LightBlue: {
		radius = g_PointBigRadius;
		SelectObject(hDC, g_RenderInstruments.hBlueLightBrush);
		SelectObject(hDC, g_RenderInstruments.hBlueLightPen);
	} break;
	}

	Ellipse(hDC, x - radius, y - radius, x + radius + 1, y + radius + 1);
}

void drawPoints(HDC hDC, GamePtr& game)
{
	int y = g_freeBorderSone;
	for (int point_y = 0; point_y < game->height; point_y++)
	{
		int x = g_freeBorderSone;
		for (int point_x = 0; point_x < game->width; point_x++)
		{
			drawPoint(hDC, x, y, game->points[point_y][point_x].color);
			x += g_CellSize;
		}
		y += g_CellSize;
	}
}

void initialDrawField(RenderContext& renderContext, GamePtr& game)
{
	SelectObject(renderContext.baseBuffer->hDC, g_RenderInstruments.hBlackPen);
	SelectObject(renderContext.baseBuffer->hDC, g_RenderInstruments.hWhiteBrush);

	RECT rect;
	rect.left = rect.top = 0;
	rect.right = renderContext.baseBuffer->width;
	rect.bottom = renderContext.baseBuffer->height;

	FillRect(renderContext.baseBuffer->hDC, &rect, g_RenderInstruments.hGrayBrush);

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

	drawPoints(renderContext.baseBuffer->hDC, game);
}

void createPensAndBrushes()
{
	if (g_RenderInstruments.isCreated) { return; }
	g_RenderInstruments.isCreated = true;

	g_RenderInstruments.hWhitePen = CreatePen(PS_SOLID, g_BorderLineWidth, RGB(255, 255, 255));
	g_RenderInstruments.hBlackPen = CreatePen(PS_SOLID, g_BaseLineWidth, RGB(0, 0, 0));
	g_RenderInstruments.hRedPen = CreatePen(PS_SOLID, g_BorderLineWidth, g_RedColor);
	g_RenderInstruments.hBluePen = CreatePen(PS_SOLID, g_BorderLineWidth, g_BlueColor);
	g_RenderInstruments.hRedLightPen = CreatePen(PS_SOLID, g_BorderLineWidth, g_RedLightColor);
	g_RenderInstruments.hBlueLightPen = CreatePen(PS_SOLID, g_BorderLineWidth, g_BlueLightColor);

	g_RenderInstruments.hWhiteBrush = CreateSolidBrush(RGB(255, 255, 255));
	g_RenderInstruments.hBlackBrush = CreateSolidBrush(RGB(0, 0, 0));
	g_RenderInstruments.hRedBrush = CreateSolidBrush(g_RedFillColor);
	g_RenderInstruments.hBlueBrush = CreateSolidBrush(g_BlueFillColor);
	g_RenderInstruments.hRedLightBrush = CreateSolidBrush(g_RedLightColor);
	g_RenderInstruments.hBlueLightBrush = CreateSolidBrush(g_BlueLightColor);

	g_RenderInstruments.hGrayBrush = CreateSolidBrush(RGB(125, 125, 140));
}

void destroyPensAndBrushes()
{
	if (!g_RenderInstruments.isCreated) { return; }
	g_RenderInstruments.isCreated = false;

	DeleteObject(g_RenderInstruments.hWhitePen);
	DeleteObject(g_RenderInstruments.hBlackPen);
	DeleteObject(g_RenderInstruments.hRedPen);
	DeleteObject(g_RenderInstruments.hBluePen);
	DeleteObject(g_RenderInstruments.hRedLightPen);
	DeleteObject(g_RenderInstruments.hBlueLightPen);

	DeleteObject(g_RenderInstruments.hWhiteBrush);
	DeleteObject(g_RenderInstruments.hBlackBrush);
	DeleteObject(g_RenderInstruments.hRedBrush);
	DeleteObject(g_RenderInstruments.hBlueBrush);
	DeleteObject(g_RenderInstruments.hRedLightBrush);
	DeleteObject(g_RenderInstruments.hBlueLightBrush);

	DeleteObject(g_RenderInstruments.hGrayBrush);
}


void beginPaint(RenderContext& renderContext)
{
	FillRect(renderContext.screenBuffer->hDC, &renderContext.clientRect, g_RenderInstruments.hGrayBrush);
	renderContext.hBaseDC = BeginPaint(renderContext.hWnd, &renderContext.ps);
	copyStaticBuffer(renderContext.activeBuffer, renderContext.baseBuffer);
}

void endPaint(RenderContext& renderContext)
{
	copyStaticBuffer(renderContext.screenBuffer->hDC, renderContext.activeBuffer,
		renderContext.fieldPos_x, renderContext.fieldPos_y);
	copyStaticBuffer(renderContext.hBaseDC, renderContext.screenBuffer, 0, 0);
	EndPaint(renderContext.hWnd, &renderContext.ps);
}

void copyStaticBuffer(StaticBufferPtr& to, StaticBufferPtr& from)
{
	BitBlt(to->hDC, 0, 0, to->width, to->height, from->hDC, 0, 0, SRCCOPY);
}

void copyStaticBuffer(HDC hDC_to, StaticBufferPtr& from, int x, int y)
{
	BitBlt(hDC_to, x, y, from->width, from->height, from->hDC, 0, 0, SRCCOPY);
}

void repaintPoint(RenderContext& renderContext, int x, int y, PointColor color)
{
	int pos_x = g_freeBorderSone + x * g_CellSize;
	int pos_y = g_freeBorderSone + y * g_CellSize;

	drawPoint(renderContext.baseBuffer->hDC, pos_x, pos_y, color);
	InvalidateRect(renderContext.hWnd, nullptr, false);
}

void paintLines(RenderContext& renderContext, std::vector<std::pair<int, int>>& points, PointColor color)
{
	if (color == Color_Blue)
	{
		SelectObject(renderContext.baseBuffer->hDC, g_RenderInstruments.hBluePen);
	}
	else
	{
		SelectObject(renderContext.baseBuffer->hDC, g_RenderInstruments.hRedPen);
	}

	for (int i = 0, size = points.size() - 1; i < size; i++)
	{
		drawLine(renderContext.baseBuffer->hDC,
			points[i].first * g_CellSize + g_freeBorderSone, points[i].second * g_CellSize + g_freeBorderSone,
			points[i + 1].first * g_CellSize + g_freeBorderSone, points[i + 1].second * g_CellSize + g_freeBorderSone);
	}
}

void drawWhoMove(RenderContext& rendererContext, GamePtr& game)
{
	std::string hwoMove;
	SetBkMode(rendererContext.screenBuffer->hDC, TRANSPARENT);
	HBRUSH fillBrush = nullptr;
	
	switch (game->whoseMove)
	{
	case Color_Blue: {
		hwoMove = "Ход синего игрока!";
		fillBrush = g_RenderInstruments.hBlueBrush;
		SetTextColor(rendererContext.screenBuffer->hDC, g_BlueTextColor);
	} break;
	case Color_Red: {
		hwoMove = "Ход красного игрока!";
		fillBrush = g_RenderInstruments.hRedBrush;
		SetTextColor(rendererContext.screenBuffer->hDC, g_RedTextColor);
	} break;
	}

	SIZE textSize;
	GetTextExtentPoint32(
		rendererContext.screenBuffer->hDC,
		hwoMove.c_str(),
		hwoMove.size(),
		&textSize);

	RECT rectForDrawText = rendererContext.clientRect;
	rectForDrawText.bottom = textSize.cy + 10;

	FillRect(rendererContext.screenBuffer->hDC, &rectForDrawText, fillBrush);
	DrawText(rendererContext.screenBuffer->hDC,
		hwoMove.c_str(),
		hwoMove.size(),
		&rectForDrawText,
		DT_CENTER | DT_VCENTER
	);
}

void drawScores(RenderContext& rendererContext, GamePtr& game)
{
	SetBkMode(rendererContext.screenBuffer->hDC, TRANSPARENT);
	std::string info = "  Красный игрок: " + std::to_string(game->redScore);

	SIZE textSize;
	GetTextExtentPoint32(
		rendererContext.screenBuffer->hDC,
		info.c_str(),
		info.size(),
		&textSize);

	RECT rectForDrawText = rendererContext.clientRect;
	rectForDrawText.bottom = textSize.cy + 10;

	SetTextColor(rendererContext.screenBuffer->hDC, g_RedTextColor);
	DrawText(rendererContext.screenBuffer->hDC,
		info.c_str(),
		info.size(),
		&rectForDrawText,
		DT_LEFT | DT_VCENTER
	);

	info = "Синий игрок: " + std::to_string(game->blueScore) + "  ";
	SetTextColor(rendererContext.screenBuffer->hDC, g_BlueTextColor);
	DrawText(rendererContext.screenBuffer->hDC,
		info.c_str(),
		info.size(),
		&rectForDrawText,
		DT_RIGHT | DT_VCENTER
	);
}

void drawHoverPoint(RenderContext& rendererContext, GamePtr& game)
{
	if (!game->isHover) { return; }
	if (game->points[game->mouseHover.second][game->mouseHover.first].color) { return; }
	if (game->points[game->mouseHover.second][game->mouseHover.first].whoCaptured) { return; }

	int point_x = g_freeBorderSone + game->mouseHover.first * g_CellSize;
	int point_y = g_freeBorderSone + game->mouseHover.second * g_CellSize;

	switch (game->whoseMove)
	{
	case Color_Blue: {
		drawPoint(rendererContext.activeBuffer->hDC, point_x, point_y, Color_LightBlue);
	} break;
	case Color_Red: {
		drawPoint(rendererContext.activeBuffer->hDC, point_x, point_y, Color_LightRed);
	} break;
	}
	
}

