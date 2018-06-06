#pragma once
#include "datastructures.h"

////_______________ Static buffer begin
struct StaticBuffer {
	HDC hDC;
	HBITMAP hBmp;

	int width;
	int height;
};

void releaseStaticBuffer(StaticBuffer* buffer);
using StaticBufferPtr = std::unique_ptr<StaticBuffer, decltype(&releaseStaticBuffer)>;


StaticBufferPtr makeStaticBuffer(HDC hBaseDC, int width, int height);
////_______________ Static buffer End


////_______________ Render context begin
struct RenderContext {
	StaticBufferPtr screenBuffer;
	StaticBufferPtr baseBuffer;
	StaticBufferPtr activeBuffer;

	int fieldPos_x;
	int fieldPos_y;

	HWND hWnd;
	RECT clientRect;
	PAINTSTRUCT ps;
	HDC hBaseDC;
	RenderContext() :
		screenBuffer(nullptr, nullptr),
		baseBuffer(nullptr, nullptr),
		activeBuffer(nullptr, nullptr) { }
};

RenderContext makeRenderContext(HWND hWnd, GamePtr& game);
void updateScreenSize(RenderContext& rendererContext);
////_______________ Render context end


void createPensAndBrushes();
void destroyPensAndBrushes();

void repaintPoint(RenderContext& renderContext, int x, int y, PointColor color);
void paintLines(RenderContext& renderContext, std::vector<std::pair<int, int>>& points, PointColor color);

void beginPaint(RenderContext& renderContext);
void endPaint(RenderContext& renderContext);