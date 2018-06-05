#include "stdafx.h"
#include "GameLogick.h"

GamePtr createGame(int width, int height)
{
	auto game = std::make_unique<Game>();
	game->blueScore = 0;
	game->redScore = 0;
	game->width = width;
	game->height = height;

	game->points.resize(height);
	for (auto& line : game->points)
	{
		line.resize(width);
		for (auto& point : line)
		{
			point.color = Color_Neutral;
			point.isFree = true;
		}
	}

	return game;
}
