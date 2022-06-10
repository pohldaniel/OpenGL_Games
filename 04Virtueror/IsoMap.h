#pragma once
#include <unordered_map>
#include "engine/Vector.h"
#include "Constants.h"
#include "IsoLayer.h"

struct Tile {
	Vector2f position;
	Vector2f size;
	unsigned int gid;
};

class IsoMap {

public:
	IsoMap();
	~IsoMap();

	void render(Matrix4f& transform);

	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices);
	void updateTilePositions();
	void setOrigin(const float x, const float y);
	void setOrigin(const Vector2f &origin);
	void setSize(unsigned int rows, unsigned int cols);
	void updateBuffer();

	unsigned int GetWidth() const;
	unsigned int GetHeight() const;
	unsigned int GetNumCols() const;
	unsigned int GetNumRows() const;
	unsigned int GetTileWidth() const;
	unsigned int GetTileHeight() const;

	Vector2f GetCellPosition(unsigned int r, unsigned int c) const;
	Vector2f GetCellPosition(unsigned int index) const;


	IsoLayer * CreateLayer(unsigned int layerId);
	IsoLayer * GetLayer(unsigned int layerId) const;

	std::vector<Tile> tiles;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;

	std::vector<IsoLayer *> mLayers;
	std::unordered_map<unsigned int, IsoLayer *> mLayersMap;
	std::vector<IsoLayer *> mLayersRenderList;

	Vector2f m_origin;
	Spritesheet *m_spriteSheet;
	Shader *m_shaderLevel;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;

	unsigned int m_rows = 0;
	unsigned int m_cols = 0;
};

inline Vector2f IsoMap::GetCellPosition(unsigned int r, unsigned int c) const {
	const unsigned int ind = r * m_cols + c;
	return GetCellPosition(ind);
}

inline unsigned int IsoMap::GetWidth() const { return TILE_WIDTH * m_cols; }

inline unsigned int IsoMap::GetHeight() const { return TILE_HEIGHT * m_rows; }

inline unsigned int IsoMap::GetNumCols() const { return m_cols; }

inline unsigned int IsoMap::GetNumRows() const { return m_rows; }

inline unsigned int IsoMap::GetTileWidth() const { return TILE_WIDTH; }
inline unsigned int IsoMap::GetTileHeight() const { return TILE_HEIGHT; }

inline IsoLayer * IsoMap::GetLayer(unsigned int layerId) const {
	auto res = mLayersMap.find(layerId);

	if (res != mLayersMap.end())
		return res->second;
	else
		return nullptr;
}