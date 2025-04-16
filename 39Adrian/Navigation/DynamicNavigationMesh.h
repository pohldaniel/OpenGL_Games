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
	bool Allocate() override;
	bool Allocate(const BoundingBox& boundingBox, unsigned maxTiles) override;
	bool Allocate(const BoundingBox& boundingBox, unsigned tilesX, unsigned tilesZ) override;
	void ReleaseNavigationMesh() override;
	bool Build() override;
	void RemoveTile(const std::array<int, 2>& tile, unsigned int layersToRemove = 0u) override;
	bool AddTile(const Buffer& tileData) override;
	bool AddTile(int x, int z);

	Buffer& GetTileData(Buffer& buffer, const std::array<int, 2>& tile) const override;
	Buffer& GetTileData(int x, int z) override;
	void WriteTiles(Buffer& dest, int x, int z) const;
	bool ReadTiles(const Buffer& source);
	bool IsObstacleInTile(Obstacle* obstacle, const std::array<int, 2>& tile) const;

	void update(float dt);
	void wait();

	/// Used by Obstacle class to add itself to the tile cache, if 'silent' an event will not be raised.
	void AddObstacle(Obstacle* obstacle, bool silent = false);
	/// Used by Obstacle class to update itself.
	void ObstacleChanged(Obstacle* obstacle);
	/// Used by Obstacle class to remove itself from the tile cache, if 'silent' an event will not be raised.
	void RemoveObstacle(Obstacle* obstacle, bool silent = false);
	void addObstacles();

	void ReleaseTileCache();
	/// Build one tile of the navigation mesh. Return true if successful.
	int BuildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z, TileCacheData* tiles);
	
	std::vector<Obstacle*> m_obstacles;

	bool drawObstacles_;
	bool drawOffMeshConnections_;
	bool drawNavAreas_;

	dtTileCache* tileCache_;
	dtTileCacheAlloc* allocator_;
	dtTileCacheCompressor* compressor_;
	dtTileCacheMeshProcess* meshProcessor_;

	CrowdManager* m_crowdManager;

	/// Maximum number of obstacle objects allowed.
	unsigned maxObstacles_;
	/// Maximum number of layers that are allowed to be constructed.
	unsigned maxLayers_;

	std::vector<std::array<int, 2>> m_tileQueue;
	std::vector<CrowdAgent*> m_agentsToReset;

	unsigned numTiles_;
};