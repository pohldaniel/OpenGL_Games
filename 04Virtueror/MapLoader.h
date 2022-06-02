#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "engine/Vector.h"
#include "engine/Spritesheet.h"

#include "Constants.h"
struct Tile {
	Vector2f position;
	Vector2f size;
	unsigned int gid;
};

struct MapLoader {

public:

	bool loadLevel(const std::string & filename);
	//void readBaseData(std::fstream & fs);

	std::vector<unsigned int> mMap;
	std::vector<Vector2f> mTilePositions;

	std::vector<Tile> tiles;

	Spritesheet *m_spriteSheet;


	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;

	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices);
};