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


std::vector<std::pair<int, int>> checkingExistenceOfRing(GamePtr& game)
{
	using Point = std::pair<int, int>;
	Point firstSuspectPoint = game->mouseHover;
	PointColor color = game->points[game->mouseHover.second][game->mouseHover.first].color;
	std::stack<Point> suspectedPoints;
	std::stack<Point> ololoTheWay;
	std::stack<Point> backWayPoints;
	std::set<Point> checkedPoints;

	suspectedPoints.push(firstSuspectPoint);
	while (!suspectedPoints.empty())
	{
		auto point = suspectedPoints.top();
		suspectedPoints.pop();

		Point lastPoint;
		if (backWayPoints.empty())
		{
			lastPoint = { -1, -1 };
		}
		else
		{
			lastPoint = backWayPoints.top();
			backWayPoints.pop();
		}

		int x = point.first;
		int y = point.second;

		if (x < 0 || x >= game->width
			|| y < 0 || y >= game->height
			|| game->points[y][x].color != color)
		{
			ololoTheWay.pop();
			continue;
		}

		if (point == firstSuspectPoint && backWayPoints.size() > 3)
		{
			std::vector<std::pair<int, int>> res;
			while (!ololoTheWay.empty())
			{
				point = ololoTheWay.top();
				ololoTheWay.pop();
				if (game->points[point.second][point.first].color == color && checkedPoints.find(point) != checkedPoints.end())
				{
					checkedPoints.erase(point);
					res.push_back(point);
				}
			}
			return res;
		}

		if (checkedPoints.find(point) != checkedPoints.end())
		{
			ololoTheWay.pop();
			continue;
		}
		checkedPoints.insert(point);

		for (int offset_y = -1; offset_y <= 1; offset_y++)
		{
			for (int offset_x = -1; offset_x <= 1; offset_x++)
			{
				if ((offset_x == offset_y && offset_y == 0)
					|| Point(x + offset_x, y + offset_y) == lastPoint)
				{
					continue;
				}
				suspectedPoints.push({ x + offset_x, y + offset_y });
				ololoTheWay.push(point);
				backWayPoints.push(point);
			}
		}
	}

	return {};
}

void modifyExistenceOfRing(std::vector<std::pair<int, int>>& way)
{
	for (int i = 0; i < way.size() - 2;)
	{
		if ((way[i].second == way[i + 1].second && way[i + 1].first == way[i + 2].first)
			|| (way[i].first == way[i + 1].first && way[i + 1].second == way[i + 2].second))
		{
			way.erase(way.begin() + i + 1);
		}
		else
		{
			i++;
		}
	}

	way.push_back(way[0]);
}


std::vector<std::pair<int, int>> findOtherColors(GamePtr& game, std::vector<std::pair<int, int>>& points)
{
	auto max = points[0];

	for (int i = 1; i < points.size(); i++)
	{
		if (points[i].second > max.second)
		{
			max = points[i];
		}
	}

	max.second--;

	using Point = std::pair<int, int>;
	PointColor color = game->points[game->mouseHover.second][game->mouseHover.first].color;
	std::queue<Point> suspectPoints;
	std::set<Point> checkedPoints;
	suspectPoints.push(max);

	std::vector<Point> res;

	while (!suspectPoints.empty())
	{
		auto point = suspectPoints.front();
		suspectPoints.pop();

		int x = point.first;
		int y = point.second;

		if (x < 0 || x >= game->width
			|| y < 0 || y >= game->height
			|| game->points[y][x].color == color)
		{
			continue;
		}

		if (checkedPoints.find(point) != checkedPoints.end())
		{
			continue;
		}
		checkedPoints.insert(point);

		if (game->points[y][x].color != Color_Neutral)
		{
			res.push_back(point);
		}

		for (int offset_y = -1; offset_y <= 1; offset_y++)
		{
			for (int offset_x = -1; offset_x <= 1; offset_x++)
			{
				if (offset_x == offset_y && offset_y == 0)
				{
					continue;
				}
				suspectPoints.push({ x + offset_x, y + offset_y });
			}
		}
	}

	return res;
}


