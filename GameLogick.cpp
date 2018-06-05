#include "stdafx.h"
#include "GameLogick.h"
#include "Constants.h"

GamePtr createGame(int width, int height)
{
	auto game = std::make_unique<Game>();
	game->blueScore = 0;
	game->redScore = 0;
	game->width = width;
	game->height = height;

	game->whoseMove = Color_Red;

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


void updateMouseInfo(GamePtr& game, RenderContext& renderContext, int x, int y)
{
	int start_x = renderContext.fieldPos_x + g_freeBorderSone - g_CellSize / 2;
	int end_x = start_x + game->width * g_CellSize;

	int start_y = renderContext.fieldPos_y + g_freeBorderSone - g_CellSize / 2;
	int end_y = start_y + game->height * g_CellSize;

	if (x < start_x || x > end_x || y < start_y || y > end_y)
	{
		game->isHover = false;
		return;
	}

	game->mouseHover.first = (x - start_x) / g_CellSize;
	game->mouseHover.second = (y - start_y) / g_CellSize;

	game->isHover = true;
}

bool mouseClickProcessing(GamePtr& game)
{
	if (!game->isHover) { return false; }

	int x = game->mouseHover.first;
	int y = game->mouseHover.second;

	if (!game->points[y][x].isFree) { return false;	}

	game->points[y][x].isFree = false;
	game->points[y][x].color = game->whoseMove;

	return true;
}

bool testOnClose(GamePtr& game)
{
	PointColor color = game->points[game->mouseHover.second][game->mouseHover.first].color;

	using Pos = std::pair<int, int>;
	std::set<Pos> checkedPoints;
	std::stack<std::pair<Pos, int>> pointsStack;
	pointsStack.push({ game->mouseHover, 1 });

	while (!pointsStack.empty())
	{
		auto cords = pointsStack.top();
		pointsStack.pop();

		int x = cords.first.first;
		int y = cords.first.second;
		int mass = cords.second;

		if (x < 0 || x >= game->width
			|| y < 0 || y >= game->height
			|| game->points[y][x].color != color)
		{
			continue;
		}

		if (checkedPoints.size() > 3 && game->mouseHover == cords.first)
		{
			return true;
		}

		if (checkedPoints.find(cords.first) != checkedPoints.end())
		{
			continue;
		}
		checkedPoints.insert(cords.first);

		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				pointsStack.push({ { x - 1, y - 1 }, mass + 1 });
			}
		}
	}

	return false;
}
