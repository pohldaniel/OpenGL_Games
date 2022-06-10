#pragma once
#include <unordered_map>
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

	void render();

	IsoLayer * CreateLayer(unsigned int layerId);
	IsoLayer * GetLayer(unsigned int layerId) const;

	std::vector<Tile> tiles;
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;

	std::vector<IsoLayer *> mLayers;
	std::unordered_map<unsigned int, IsoLayer *> mLayersMap;
	std::vector<IsoLayer *> mLayersRenderList;
};

inline IsoLayer * IsoMap::GetLayer(unsigned int layerId) const {
	auto res = mLayersMap.find(layerId);

	if (res != mLayersMap.end())
		return res->second;
	else
		return nullptr;
}