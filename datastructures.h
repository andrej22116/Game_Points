#pragma once

enum PointColor {
	Color_Neutral,
	Color_Blue,
	Color_Red
};

struct GamePoint {
	PointColor color;
	bool isFree;
};

struct Game {
	std::vector<std::vector<GamePoint>> points;
	int width;
	int height;
	int redScore;
	int blueScore;
};

using GamePtr = std::unique_ptr<Game>;