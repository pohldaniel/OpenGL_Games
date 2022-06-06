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

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetNumCols() const;
	unsigned int GetNumRows() const;
	unsigned int GetTileWidth() const;
	unsigned int GetTileHeight() const;

	Vector2f GetCellPosition(unsigned int r, unsigned int c) const;
	Vector2f GetCellPosition(unsigned int index) const;

	bool loadLevel(const std::string & filename);
	//void readBaseData(std::fstream & fs);

	std::vector<unsigned int> mMap;
	//std::vector<Vector2f> mTilePositions;

	std::vector<Tile> tiles;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;

	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices);

	void setOrigin(const float x, const float y);
	void setOrigin(const Vector2f &origin);

	Vector2f m_origin;

	void updateTilePositions();

	unsigned int m_rows = 0;
	unsigned int m_cols = 0;

};

inline Vector2f MapLoader::GetCellPosition(unsigned int r, unsigned int c) const {
	const unsigned int ind = r * m_cols + c;
	return GetCellPosition(ind);
}

inline unsigned int MapLoader::GetWidth() const { return TILE_WIDTH * m_cols; }

inline unsigned int MapLoader::GetHeight() const { return TILE_HEIGHT * m_rows; }

inline unsigned int MapLoader::GetNumCols() const { return m_cols; }

inline unsigned int MapLoader::GetNumRows() const { return m_rows; }

inline unsigned int MapLoader::GetTileWidth() const { return TILE_WIDTH; }
inline unsigned int MapLoader::GetTileHeight() const { return TILE_HEIGHT; }