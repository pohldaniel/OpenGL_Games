#pragma once

#include <vector>
#include "NavigationMesh.h"

class dtTileCache;
struct dtTileCacheAlloc;
struct dtTileCacheCompressor;
struct dtTileCacheMeshProcess;

class OffMeshConnection;
class Obstacle;

class DynamicNavigationMesh : public NavigationMesh {

public:

	DynamicNavigationMesh();
	~DynamicNavigationMesh();

	void OnRenderDebug() override;
	bool Allocate(const BoundingBox& boundingBox, unsigned maxTiles) override;
	void ReleaseNavigationMesh() override;

	/// Used by Obstacle class to add itself to the tile cache, if 'silent' an event will not be raised.
	void AddObstacle(Obstacle* obstacle, bool silent = false);
	/// Used by Obstacle class to update itself.
	void ObstacleChanged(Obstacle* obstacle);
	/// Used by Obstacle class to remove itself from the tile cache, if 'silent' an event will not be raised.
	void RemoveObstacle(Obstacle* obstacle, bool silent = false);

	void ReleaseTileCache();

	std::vector<Obstacle*> m_obstacles;

	bool drawObstacles_;
	dtTileCache* tileCache_;
	dtTileCacheAlloc* allocator_;
	dtTileCacheCompressor* compressor_;
	dtTileCacheMeshProcess* meshProcessor_;

	/// Maximum number of obstacle objects allowed.
	unsigned maxObstacles_;
	/// Maximum number of layers that are allowed to be constructed.
	unsigned maxLayers_;

	struct TileCacheData;
};