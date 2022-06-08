#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_set>

#include "engine/Vector.h"
#include "engine/Spritesheet.h"

#include "Constants.h"
#include "IsoLayer.h"
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
	bool ReadObjectsData(const std::string & filename);
	void CreateObjectFromFile(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols);
	void CreateObject(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols);

	std::vector<unsigned int> mMap;
	//std::vector<Vector2f> mTilePositions;

	std::vector<IsoLayer *> mLayers;
	std::unordered_map<unsigned int, IsoLayer *> mLayersMap;
	std::vector<IsoLayer *> mLayersRenderList;

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

	IsoLayer * CreateLayer(unsigned int layerId);
	IsoLayer * GetLayer(unsigned int layerId) const;

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

inline IsoLayer * MapLoader::GetLayer(unsigned int layerId) const {
	auto res = mLayersMap.find(layerId);

	if (res != mLayersMap.end())
		return res->second;
	else
		return nullptr;
}