#pragma once
#include "datastructures.h"

////_______________ Static buffer begin
struct StaticBuffer {
	HDC hDC;
	HBITMAP hBmp;

	int width;
	int height;
};

void releaseStaticBuffer(StaticBuffer& buffer);
using StaticBufferPtr = std::unique_ptr<StaticBuffer, decltype(&releaseStaticBuffer)>;

StaticBufferPtr makeStaticBuffer(HDC hBaseDC, GamePtr& game);
////_______________ Static buffer End


////_______________ Render context begin
struct RenderContext {
	StaticBufferPtr baseBuffer;
	StaticBufferPtr activeBuffer;

	HWND hWnd;
	PAINTSTRUCT ps;
	HDC hBaseDC;
	RenderContext() : baseBuffer(nullptr, nullptr), activeBuffer(nullptr, nullptr) { }
};

RenderContext makeRenderContext(HWND hWnd, GamePtr& game);
////_______________ Render context end


void createPensAndBrushes();
void destroyPensAndBrushes();

void beginPaint(RenderContext& renderContext);
void endPaint(RenderContext& renderContext);