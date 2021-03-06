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

	game->freePoints = game->pointsAmount = width * height;

	return game;
}


void updateMouseInfo(GamePtr& game, RenderContext& renderContext, int x, int y)
{
	int start_x = renderContext.fieldPos_x + g_freeBorderSone - g_CellSize / 2;
	int end_x = start_x + game->width * g_CellSize - 1;

	int start_y = renderContext.fieldPos_y + g_freeBorderSone - g_CellSize / 2;
	int end_y = start_y + game->height * g_CellSize - 1;

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

	if (game->points[y][x].color || game->points[y][x].whoCaptured) { return false;	}

	game->points[y][x].color = game->whoseMove;
	game->points[y][x].whoCaptured = game->whoseMove;
	game->freePoints--;

	return true;
}


std::vector<std::pair<int, int>> checkingExistenceOfRing(GamePtr& game)
{
	using Point = std::pair<int, int>;
	Point firstSuspectPoint = game->mouseHover;
	PointColor color = game->points[game->mouseHover.second][game->mouseHover.first].color;
	std::set<Point> checkedPoints;

	std::vector<std::pair<int, int>> res;

	auto isFree = [&game, color](int x, int y) -> bool
	{
		if (x < 0 || x >= game->width
			|| y < 0 || y >= game->height)
		{
			return false;
		}
		if (!game->points[y][x].isFree 
			|| game->points[y][x].color != color
			|| game->points[y][x].whoCaptured != color)
		{
			return false;
		}
		return true;
	};

	auto find = [&game, &checkedPoints, &res, color, firstSuspectPoint, isFree](Point point) -> void
	{
		auto subFind = [&game, &checkedPoints, &res, color, firstSuspectPoint, isFree]
		(Point point, int depth, auto find) -> bool
		{
			if (point == firstSuspectPoint && depth >= 4) return true;

			if (!isFree(point.first, point.second)) return false;
			if (checkedPoints.find(point) != checkedPoints.end()) return false;
			checkedPoints.insert(point);

			for (int offset_y = -1; offset_y <= 1; offset_y++)
			{
				for (int offset_x = -1; offset_x <= 1; offset_x++)
				{
					Point nextPoint(point.first + offset_x, point.second + offset_y);
					if (find(nextPoint, depth + 1, find))
					{
						res.push_back(nextPoint);
						return true;
					}
				}
			}

			return false;
		};

		subFind(point, 1, subFind);
	};

	find(firstSuspectPoint);

	return res;
}

void modifyExistenceOfRing(std::vector<std::pair<int, int>>& way)
{
	for (int lol = 0; lol < 2; lol++)
	{
		for (int i = 0; i < way.size() - 2;)
		{
			if ((way[i].second == way[i + 1].second && way[i + 1].first == way[i + 2].first)
				|| (way[i].first == way[i + 1].first && way[i + 1].second == way[i + 2].second))
			{
				way.erase(way.begin() + i + 1);
			}
			else if ((way[i].first == way[i + 2].first && way[i].second == way[i + 1].second)
				|| (way[i].first == way[i + 2].first && way[i + 1].second == way[i + 2].second))
			{
				way.erase(way.begin() + i + 1, way.begin() + i + 2);
			}
			else
			{
				i++;
			}
		}

		std::rotate(way.begin(), way.end() - 1, way.end());
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

	if (game->points[max.second][max.first].color == color) return {};

	std::vector<Point> res;
	std::set<Point> setOfPoints(points.begin(), points.end());

	auto isFree = [&game, color, setOfPoints](int x, int y) -> bool
	{
		if (x < 0 || x >= game->width
			|| y < 0 || y >= game->height)
		{
			return false;
		}
		if ((game->points[y][x].color == color && game->points[y][x].isFree) && setOfPoints.find({ x, y }) != setOfPoints.end())
		{
			return false;
		}
		return true;
	};

	while (!suspectPoints.empty())
	{
		auto point = suspectPoints.front();
		suspectPoints.pop();

		int x = point.first;
		int y = point.second;

		if (checkedPoints.find(point) != checkedPoints.end())
		{
			continue;
		}
		checkedPoints.insert(point);

		if (color == Color_Blue)
		{
			if (game->points[y][x].whoCaptured == Color_Red)
			{
				if (game->points[y][x].color == Color_Red) game->blueScore++;
				else if (game->points[y][x].color == Color_Blue) game->redScore--;
			}
		}
		else if (color == Color_Red)
		{
			if (game->points[y][x].whoCaptured == Color_Blue)
			{
				if (game->points[y][x].color == Color_Blue) game->redScore++;
				else if (game->points[y][x].color == Color_Red) game->blueScore--;
			}
		}

		if (!game->points[y][x].color) game->freePoints--;

		game->points[y][x].whoCaptured = color;
		game->points[y][x].isFree = false;
		res.push_back(point);

		bool freeMatrix[3][3] = { false };
		for (int offset_y = -1; offset_y <= 1; offset_y++)
		{
			for (int offset_x = -1; offset_x <= 1; offset_x++)
			{
				freeMatrix[offset_y + 1][offset_x + 1] = isFree(x + offset_x, y + offset_y);
			}
		}

		if (freeMatrix[0][1]) suspectPoints.push({ x, y - 1 });
		if (freeMatrix[2][1]) suspectPoints.push({ x, y + 1 });
		if (freeMatrix[1][0]) suspectPoints.push({ x - 1, y });
		if (freeMatrix[1][2]) suspectPoints.push({ x + 1, y });


		if (freeMatrix[0][0] && (freeMatrix[0][1] || freeMatrix[1][0])) suspectPoints.push({ x - 1, y - 1 });
		if (freeMatrix[0][2] && (freeMatrix[0][1] || freeMatrix[1][2])) suspectPoints.push({ x + 1, y - 1 });
		if (freeMatrix[2][0] && (freeMatrix[1][0] || freeMatrix[2][1])) suspectPoints.push({ x - 1, y + 1 });
		if (freeMatrix[2][2] && (freeMatrix[2][1] || freeMatrix[1][2])) suspectPoints.push({ x + 1, y + 1 });
	}

	return res;
}


