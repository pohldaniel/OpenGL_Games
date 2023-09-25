#pragma once

#include <vector>
#include "Tile.h"

class Map {

public:
	
	void drawRaw();
	void createBuffer(std::vector<Tile>& tiles);

private:

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;

	unsigned int drawCount;
};