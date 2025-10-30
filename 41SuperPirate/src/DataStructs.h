#pragma once

#include <vector>
#include <array>
#include <string>

struct Cell {
	int tileID;
	float posX;
	float posY;
	float width;
	float height;
	float centerX;
	float centerY;
	bool visibile;
	bool flipped;
};

struct TileSetData {
	std::vector<std::pair<std::string, float>> pathSizes;
	std::vector<std::pair<std::string, unsigned int>> offsets;
	std::vector<unsigned int > indices;
};