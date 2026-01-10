#pragma once

#include <vector>
#include "NavigationMesh.h"

class dtTileCache;
struct dtTileCacheAlloc;
struct dtTileCacheCompressor;
struct dtTileCacheMeshProcess;

class Obstacle;
class CrowdManager;
class CrowdAgent;

class DynamicNavigationMesh : public NavigationMesh {

	struct TileCacheData;

public:

	DynamicNavigationMesh();
	~DynamicNavigationMesh();

	void OnRenderDebug() override;
	bool allocate() override;
	bool allocate(const BoundingBox& boundingBox, unsigned int maxTiles) override;
	bool allocate(const BoundingBox& boundingBox, unsigned int tilesX, unsigned int tilesZ) override;
	void releaseNavigationMesh() override;
	bool build() override;
	void removeTile(const std::array<int, 2>& tile, unsigned int layersToRemove = 0u) override;
	bool addTile(const Buffer& tileData) override;
	bool addTile(int x, int z);
	Buffer& getTileData(Buffer& buffer, const std::array<int, 2>& tile) const override;
	
	void update(float dt);
	void wait();

	
	Buffer& getTileData(int x, int z) override;
	void writeTiles(Buffer& dest, int x, int z) const;
	bool readTiles(const Buffer& source);
	void releaseTileCache();

	void addObstacle(Obstacle* obstacle, bool force = true);
	void obstacleChanged(Obstacle* obstacle);
	void removeObstacle(Obstacle* obstacle, bool force = true);
	void addObstacles();
	void removeObstacles();
	bool isObstacleInTile(Obstacle* obstacle, const std::array<int, 2>& tile) const;
	const std::vector<CrowdAgent*>& getAgentsToReset() const;
	std::vector<CrowdAgent*>& agentsToReset();
	void setCrowdManager(CrowdManager* crowdManager);
	void initObstacles();
	
private:

	int buildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z, TileCacheData* tiles);
	
	dtTileCache* m_tileCache;
	dtTileCacheAlloc* m_allocator;
	dtTileCacheCompressor* m_compressor;
	dtTileCacheMeshProcess* m_meshProcessor;

	CrowdManager* m_crowdManager;
	std::vector<CrowdAgent*> m_agentsToReset;
	std::vector<Obstacle*> m_obstacles;
	std::vector<std::array<int, 2>> m_tileQueue;

	unsigned int m_maxObstacles;
	unsigned int m_maxLayers;
	unsigned int m_numTiles;

	bool m_drawObstacles;

};