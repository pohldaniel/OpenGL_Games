#pragma once
#include <iostream>
#include <vector>
#include <array>
#include <string>
#include <engine/Rect.h>

struct Cell {
	int tileID;
	float posX;
	float posY;
	float width;
	float height;
	float centerX;
	float centerY;
	float offsetX;
	float offsetY;
	bool visibile;
	bool flipped;
};

struct TileSetData {
	std::vector<std::pair<std::string, float>> pathSizes;
	std::vector<std::pair<std::string, unsigned int>> offsets;
	std::vector<unsigned int > indices;
};

struct CollisionRect : public Rect {
	
	mutable Rect previousRect;

	CollisionRect(float posX, float posY, float width, float height, bool hasCollision = false) : 
		Rect{ posX , posY , width , height , hasCollision },
		previousRect({ posX , posY , width , height , hasCollision })
	{
		
	}
};