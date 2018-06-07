#pragma once

enum PointColor {
	Color_Neutral,
	Color_Blue,
	Color_Red,
	Color_LightBlue,
	Color_LightRed
};

struct GamePoint {
	PointColor whoCaptured;
	PointColor color;
	bool isFree;
};

struct Game {
	std::vector<std::vector<GamePoint>> points;
	int width;
	int height;
	int pointsAmount;
	int freePoints;

	int redScore;
	int blueScore;
	PointColor whoseMove;

	bool isHover = false;
	std::pair<int, int> mouseHover;
};

using GamePtr = std::unique_ptr<Game>;