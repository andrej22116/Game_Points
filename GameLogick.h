#pragma once
#include "datastructures.h"
#include "Render.h"

GamePtr createGame(int width, int height);

void updateMouseInfo(GamePtr& game, RenderContext& renderContext, int x, int y);

bool mouseClickProcessing(GamePtr& game);

bool testOnClose(GamePtr& game);