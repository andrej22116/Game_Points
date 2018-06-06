#pragma once
#include "datastructures.h"
#include "Render.h"

GamePtr createGame(int width, int height);

void updateMouseInfo(GamePtr& game, RenderContext& renderContext, int x, int y);

bool mouseClickProcessing(GamePtr& game);

std::vector<std::pair<int, int>> checkingExistenceOfRing(GamePtr& game);

void modifyExistenceOfRing(std::vector<std::pair<int, int>>& way);

std::vector<std::pair<int, int>> findOtherColors(GamePtr& game, std::vector<std::pair<int, int>>& points);
