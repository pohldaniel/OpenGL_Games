#include <lz4.h>
#include <DetourNavMesh.h>
#include <DetourTileCache.h>
#include <DetourNavMeshBuilder.h>
#include <DetourTileCacheBuilder.h>
#include <Recast.h>

#include "DynamicNavigationMesh.h"
#include "OffMeshConnection.h"
#include "NavArea.h"
#include "NavBuildData.h"
#include "Obstacle.h"
#include "CrowdAgent.h"

static const int DEFAULT_MAX_OBSTACLES = 1024;
static const int DEFAULT_MAX_LAYERS = 16;
static const unsigned TILECACHE_MAXLAYERS = 255;

struct DynamicNavigationMesh::TileCacheData{
	unsigned char* data;
	int dataSize;
};

struct TileCompressor : public dtTileCacheCompressor{
	virtual int maxCompressedSize(const int bufferSize) {
		return (int)(bufferSize * 1.05f);
	}

	virtual dtStatus compress(const unsigned char* buffer, const int bufferSize,
		unsigned char* compressed, const int /*maxCompressedSize*/, int* compressedSize) {
		*compressedSize = LZ4_compress_default((const char*)buffer, (char*)compressed, bufferSize, LZ4_compressBound(bufferSize));
		return DT_SUCCESS;
	}

	virtual dtStatus decompress(const unsigned char* compressed, const int compressedSize,
		unsigned char* buffer, const int maxBufferSize, int* bufferSize) {
		*bufferSize = LZ4_decompress_safe((const char*)compressed, (char*)buffer, compressedSize, maxBufferSize);
		return *bufferSize < 0 ? DT_FAILURE : DT_SUCCESS;
	}
};

struct MeshProcess : public dtTileCacheMeshProcess {
	DynamicNavigationMesh* owner_;
	std::vector<Vector3f> offMeshVertices_;
	std::vector<float> offMeshRadii_;
	std::vector<unsigned short> offMeshFlags_;
	std::vector<unsigned char> offMeshAreas_;
	std::vector<unsigned char> offMeshDir_;

	inline MeshProcess(DynamicNavigationMesh* owner) :
		owner_(owner) {
	}

	virtual void process(struct dtNavMeshCreateParams* params, unsigned char* polyAreas, unsigned short* polyFlags) {
		// Update poly flags from areas.
		// \todo Assignment of flags from areas?
		for (int i = 0; i < params->polyCount; ++i){
			if (polyAreas[i] != RC_NULL_AREA)
				polyFlags[i] = RC_WALKABLE_AREA;
		}

		BoundingBox bounds;
		rcVcopy(&bounds.min[0], params->bmin);
		rcVcopy(&bounds.max[0], params->bmin);

		// collect off-mesh connections
		const std::vector<OffMeshConnection>& offMeshConnections = owner_->m_offMeshConnections;
		if (offMeshConnections.size() > 0) {
			if (offMeshConnections.size() != offMeshRadii_.size()) {
				Matrix4f inverse = Matrix4f::IDENTITY;
				ClearConnectionData();
				for (unsigned i = 0; i < offMeshConnections.size(); ++i) {

					const OffMeshConnection& connection = offMeshConnections[i];
					Vector3f start = inverse * connection.m_node->getWorldPosition();
					Vector3f end = inverse * connection.m_endPoint->getWorldPosition();

					offMeshVertices_.push_back(start);
					offMeshVertices_.push_back(end);
					offMeshRadii_.push_back(connection.m_radius);
					offMeshFlags_.push_back((unsigned short)connection.m_mask);
					offMeshAreas_.push_back((unsigned char)connection.m_areaId);
					offMeshDir_.push_back((unsigned char)(connection.m_bidirectional ? DT_OFFMESH_CON_BIDIR : 0));
				}
			}
			params->offMeshConCount = offMeshRadii_.size();
			params->offMeshConVerts = &offMeshVertices_[0][0];
			params->offMeshConRad = &offMeshRadii_[0];
			params->offMeshConFlags = &offMeshFlags_[0];
			params->offMeshConAreas = &offMeshAreas_[0];
			params->offMeshConDir = &offMeshDir_[0];
		}
	}

	void ClearConnectionData() {
		offMeshVertices_.clear();
		offMeshRadii_.clear();
		offMeshFlags_.clear();
		offMeshAreas_.clear();
		offMeshDir_.clear();
	}
};


// From the Detour/Recast Sample_TempObstacles.cpp
struct LinearAllocator : public dtTileCacheAlloc {
	unsigned char* buffer;
	int capacity;
	int top;
	int high;

	LinearAllocator(const int cap) :
		buffer(0), capacity(0), top(0), high(0) {
		resize(cap);
	}

	~LinearAllocator() {
		dtFree(buffer);
	}

	void resize(const int cap) {
		if (buffer)
			dtFree(buffer);
		buffer = (unsigned char*)dtAlloc(cap, DT_ALLOC_PERM);
		capacity = cap;
	}

	virtual void reset() {
		high = std::max(high, top);
		top = 0;
	}

	virtual void* alloc(const int size) {
		if (!buffer)
			return 0;
		if (top + size > capacity)
			return 0;
		unsigned char* mem = &buffer[top];
		top += size;
		return mem;
	}

	virtual void free(void*) {
	}
};


DynamicNavigationMesh::DynamicNavigationMesh() : 
	NavigationMesh(), 
	drawObstacles_(true), 
	drawOffMeshConnections_(true),
	drawNavAreas_(true),
	tileCache_(nullptr),
	m_crowdManager(nullptr),
	maxObstacles_(DEFAULT_MAX_OBSTACLES),
	maxLayers_(DEFAULT_MAX_LAYERS) {
	tileSize_ = 64;
	numTiles_ = 0u;
	partitionType_ = NAVMESH_PARTITION_MONOTONE;
	allocator_ = new LinearAllocator(32000); //32kb to start
	compressor_ = new TileCompressor();
	meshProcessor_ = new MeshProcess(this);
}

DynamicNavigationMesh::~DynamicNavigationMesh() {

}

bool DynamicNavigationMesh::allocate() {
	return allocate(boundingBox_, numTilesX_, numTilesZ_);
}

bool DynamicNavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int tilesX, unsigned int tilesZ) {
	Vector3f min = boundingBox.min;
	Vector3f max = boundingBox.max;

	releaseNavigationMesh();
	boundingBox_.setMin(min);
	boundingBox_.setMax(max);

	numTilesX_ = tilesX;
	numTilesZ_ = tilesZ;
	unsigned maxTiles = Math::NextPowerOfTwo((unsigned)(numTilesX_ * numTilesZ_));
	unsigned tileBits = Math::LogBaseTwo(maxTiles);
	unsigned maxPolys = (unsigned)(1 << (22 - tileBits));
	float tileEdgeLength = (float)tileSize_ * cellSize_;

	dtNavMeshParams params;
	rcVcopy(params.orig, &boundingBox_.min[0]);
	params.tileWidth = tileEdgeLength;
	params.tileHeight = tileEdgeLength;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolys;

	navMesh_ = dtAllocNavMesh();
	if (!navMesh_) {
		std::cout << "Could not allocate navigation mesh" << std::endl;
		return false;
	}

	if (dtStatusFailed(navMesh_->init(&params))) {
		std::cout << "Could not initialize navigation mesh" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	dtTileCacheParams tileCacheParams;
	memset(&tileCacheParams, 0, sizeof(tileCacheParams));
	rcVcopy(tileCacheParams.orig, &boundingBox_.min[0]);
	tileCacheParams.ch = cellHeight_;
	tileCacheParams.cs = cellSize_;
	tileCacheParams.width = tileSize_;
	tileCacheParams.height = tileSize_;
	tileCacheParams.maxSimplificationError = edgeMaxError_;
	tileCacheParams.maxTiles = numTilesX_ * numTilesZ_ * maxLayers_;
	tileCacheParams.maxObstacles = maxObstacles_;
	// Settings from NavigationMesh
	tileCacheParams.walkableClimb = agentMaxClimb_;
	tileCacheParams.walkableHeight = agentHeight_;
	tileCacheParams.walkableRadius = agentRadius_;

	tileCache_ = dtAllocTileCache();
	if (!tileCache_) {
		std::cout << "Could not allocate tile cache" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	if (dtStatusFailed(tileCache_->init(&tileCacheParams, allocator_, compressor_, meshProcessor_))) {
		std::cout << "Could not initialize tile cache" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	//wait();
	std::cout << "Built navigation mesh with " + std::to_string(numTilesX_ * numTilesZ_) + " tiles" << std::endl;

	//addObstacles();
	return true;
}

bool DynamicNavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int maxTiles) {
	
	// Release existing navigation data and zero the bounding box
	releaseNavigationMesh();
	boundingBox_ = boundingBox;
	maxTiles = Math::NextPowerOfTwo(maxTiles);

	// Calculate number of tiles
	int gridW = 0, gridH = 0;
	float tileEdgeLength = (float)tileSize_ * cellSize_;
	rcCalcGridSize(&boundingBox_.min[0], &boundingBox_.max[0], cellSize_, &gridW, &gridH);
	numTilesX_ = (gridW + tileSize_ - 1) / tileSize_;
	numTilesZ_ = (gridH + tileSize_ - 1) / tileSize_;
	// Calculate max number of polygons, 22 bits available to identify both tile & polygon within tile
	unsigned tileBits = Math::LogBaseTwo(maxTiles);
	unsigned maxPolys = (unsigned)(1 << (22 - tileBits));

	dtNavMeshParams params;
	rcVcopy(params.orig, &boundingBox_.min[0]);
	params.tileWidth = tileEdgeLength;
	params.tileHeight = tileEdgeLength;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolys;

	navMesh_ = dtAllocNavMesh();
	if (!navMesh_){
		std::cout << "Could not allocate navigation mesh" << std::endl;
		return false;
	}

	if (dtStatusFailed(navMesh_->init(&params))){
		std::cout << "Could not initialize navigation mesh" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	dtTileCacheParams tileCacheParams;
	memset(&tileCacheParams, 0, sizeof(tileCacheParams));
	rcVcopy(tileCacheParams.orig, &boundingBox_.min[0]);
	tileCacheParams.ch = cellHeight_;
	tileCacheParams.cs = cellSize_;
	tileCacheParams.width = tileSize_;
	tileCacheParams.height = tileSize_;
	tileCacheParams.maxSimplificationError = edgeMaxError_;
	tileCacheParams.maxTiles = maxTiles * maxLayers_;
	tileCacheParams.maxObstacles = maxObstacles_;
	// Settings from NavigationMesh
	tileCacheParams.walkableClimb = agentMaxClimb_;
	tileCacheParams.walkableHeight = agentHeight_;
	tileCacheParams.walkableRadius = agentRadius_;

	tileCache_ = dtAllocTileCache();
	if (!tileCache_){
		std:: cout << "Could not allocate tile cache" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	if (dtStatusFailed(tileCache_->init(&tileCacheParams, allocator_, compressor_, meshProcessor_))){
		std::cout << "Could not initialize tile cache" << std::endl;;
		releaseNavigationMesh();
		return false;
	}

	std::cout << "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;
	//addObstacles();
	return true;
}

bool DynamicNavigationMesh::build() {
	releaseNavigationMesh();

	std::vector<NavigationGeometryInfo> geometryList;
	CollectGeometries(geometryList);

	if (geometryList.empty()) {
		std::cout << "Nothing to do" << std::endl;
		return true; // Nothing to do
	}

	// Build the combined bounding box
	for (unsigned i = 0; i < geometryList.size(); ++i)
		boundingBox_.merge(geometryList[i].boundingBox);

	// Expand bounding box by padding
	boundingBox_.min -= padding_;
	boundingBox_.max += padding_;
	{
		//URHO3D_PROFILE(BuildNavigationMesh);

		// Calculate number of tiles
		int gridW = 0, gridH = 0;
		float tileEdgeLength = (float)tileSize_ * cellSize_;
		rcCalcGridSize(&boundingBox_.min[0], &boundingBox_.max[0], cellSize_, &gridW, &gridH);
		numTilesX_ = (gridW + tileSize_ - 1) / tileSize_;
		numTilesZ_ = (gridH + tileSize_ - 1) / tileSize_;

		// Calculate max. number of tiles and polygons, 22 bits available to identify both tile & polygon within tile
		unsigned maxTiles = Math::NextPowerOfTwo((unsigned)(numTilesX_ * numTilesZ_));
		unsigned tileBits = Math::LogBaseTwo(maxTiles);
		unsigned maxPolys = (unsigned)(1 << (22 - tileBits));

		dtNavMeshParams params;
		rcVcopy(params.orig, &boundingBox_.min[0]);
		params.tileWidth = tileEdgeLength;
		params.tileHeight = tileEdgeLength;
		params.maxTiles = maxTiles;
		params.maxPolys = maxPolys;

		navMesh_ = dtAllocNavMesh();
		if (!navMesh_){
			std::cout << "Could not allocate navigation mesh" << std::endl;
			return false;
		}

		if (dtStatusFailed(navMesh_->init(&params))){
			std::cout << "Could not initialize navigation mesh" << std::endl;
			releaseNavigationMesh();
			return false;
		}
		
		dtTileCacheParams tileCacheParams;
		memset(&tileCacheParams, 0, sizeof(tileCacheParams));
		rcVcopy(tileCacheParams.orig, &boundingBox_.min[0]);
		tileCacheParams.ch = cellHeight_;
		tileCacheParams.cs = cellSize_;
		tileCacheParams.width = tileSize_;
		tileCacheParams.height = tileSize_;
		tileCacheParams.maxSimplificationError = edgeMaxError_;
		tileCacheParams.maxTiles = numTilesX_ * numTilesZ_ * maxLayers_;
		tileCacheParams.maxObstacles = maxObstacles_;
		// Settings from NavigationMesh
		tileCacheParams.walkableClimb = agentMaxClimb_;
		tileCacheParams.walkableHeight = agentHeight_;
		tileCacheParams.walkableRadius = agentRadius_;

		tileCache_ = dtAllocTileCache();
		if (!tileCache_){
			std::cout << "Could not allocate tile cache" << std::endl;
			releaseNavigationMesh();
			return false;
		}

		if (dtStatusFailed(tileCache_->init(&tileCacheParams, allocator_, compressor_, meshProcessor_))){
			std::cout << "Could not initialize tile cache" << std::endl;
			releaseNavigationMesh();
			return false;
		}

		// Build each tile
		for (int z = 0; z < numTilesZ_; ++z){
			for (int x = 0; x < numTilesX_; ++x){
				TileCacheData tiles[TILECACHE_MAXLAYERS];
				int layerCt = BuildTile(geometryList, x, z, tiles);
				for (int i = 0; i < layerCt; ++i){
					dtCompressedTileRef tileRef;
					int status = tileCache_->addTile(tiles[i].data, tiles[i].dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tileRef);
					if (dtStatusFailed((dtStatus)status)){
						dtFree(tiles[i].data);
						tiles[i].data = 0x0;
					}
				}
				tileCache_->buildNavMeshTilesAt(x, z, navMesh_);
				++numTiles_;
			}
		}

		// For a full build it's necessary to update the nav mesh
	    // not doing so will cause dependent components to crash, like CrowdManager
		//tileCache_->update(0.0f, navMesh_);
		wait();
		std::cout << "Built navigation mesh with " + std::to_string(numTiles_) + " tiles" << std::endl;

		addObstacles();
		return true;
	}
}

int DynamicNavigationMesh::BuildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z, TileCacheData* tiles) {
	//URHO3D_PROFILE(BuildNavigationMeshTile);

	tileCache_->removeTile(navMesh_->getTileRefAt(x, z, 0), 0, 0);

	BoundingBox tileBoundingBox = getTileBoudningBox(std::array<int, 2>({ x, z }));

	DynamicNavBuildData build(allocator_);

	rcConfig cfg;
	memset(&cfg, 0, sizeof cfg);
	cfg.cs = cellSize_;
	cfg.ch = cellHeight_;
	cfg.walkableSlopeAngle = agentMaxSlope_;
	cfg.walkableHeight = (int)ceilf(agentHeight_ / cfg.ch);
	cfg.walkableClimb = (int)floorf(agentMaxClimb_ / cfg.ch);
	cfg.walkableRadius = (int)ceilf(agentRadius_ / cfg.cs);
	cfg.maxEdgeLen = (int)(edgeMaxLength_ / cellSize_);
	cfg.maxSimplificationError = edgeMaxError_;
	cfg.minRegionArea = (int)sqrtf(regionMinSize_);
	cfg.mergeRegionArea = (int)sqrtf(regionMergeSize_);
	cfg.maxVertsPerPoly = 6;
	cfg.tileSize = tileSize_;
	cfg.borderSize = cfg.walkableRadius + 3; // Add padding
	cfg.width = cfg.tileSize + cfg.borderSize * 2;
	cfg.height = cfg.tileSize + cfg.borderSize * 2;
	cfg.detailSampleDist = detailSampleDistance_ < 0.9f ? 0.0f : cellSize_ * detailSampleDistance_;
	cfg.detailSampleMaxError = cellHeight_ * detailSampleMaxError_;

	rcVcopy(cfg.bmin, &tileBoundingBox.min[0]);
	rcVcopy(cfg.bmax, &tileBoundingBox.max[0]);
	cfg.bmin[0] -= cfg.borderSize * cfg.cs;
	cfg.bmin[2] -= cfg.borderSize * cfg.cs;
	cfg.bmax[0] += cfg.borderSize * cfg.cs;
	cfg.bmax[2] += cfg.borderSize * cfg.cs;

	BoundingBox expandedBox(*reinterpret_cast<Vector3f*>(cfg.bmin), *reinterpret_cast<Vector3f*>(cfg.bmax));
	GetTileGeometry(&build, geometryList, expandedBox);

	if (build.vertices.empty() || build.indices.empty())
		return 0; // Nothing to do

	build.heightField = rcAllocHeightfield();
	if (!build.heightField){
		std::cout << "Could not allocate heightfield" << std::endl;
		return 0;
	}

	if (!rcCreateHeightfield(build.ctx, *build.heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs,cfg.ch)){
		std::cout << "Could not create heightfield" << std::endl;
		return 0;
	}

	unsigned numTriangles = build.indices.size() / 3;
	unsigned char* triAreas = new unsigned char[numTriangles];
	memset(triAreas, 0, numTriangles);

	rcMarkWalkableTriangles(build.ctx, cfg.walkableSlopeAngle, &build.vertices[0][0], build.vertices.size(),
		&build.indices[0], numTriangles, triAreas);
	rcRasterizeTriangles(build.ctx, &build.vertices[0][0], build.vertices.size(), &build.indices[0],
		triAreas, numTriangles, *build.heightField, cfg.walkableClimb);
	rcFilterLowHangingWalkableObstacles(build.ctx, cfg.walkableClimb, *build.heightField);

	rcFilterLedgeSpans(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField);
	rcFilterWalkableLowHeightSpans(build.ctx, cfg.walkableHeight, *build.heightField);

	build.compactHeightField = rcAllocCompactHeightfield();
	if (!build.compactHeightField) {
		std::cout << "Could not allocate create compact heightfield" << std::endl;
		return 0;
	}

	if (!rcBuildCompactHeightfield(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField,
		*build.compactHeightField)) {
		std::cout << "Could not build compact heightfield" << std::endl;
		return 0;
	}

	if (!rcErodeWalkableArea(build.ctx, cfg.walkableRadius, *build.compactHeightField)) {
		std::cout << "Could not erode compact heightfield" << std::endl;
		return 0;
	}

	// area volumes
	for (unsigned i = 0; i < build.navAreas.size(); ++i)
		rcMarkBoxArea(build.ctx, &build.navAreas[i].bounds.min[0], &build.navAreas[i].bounds.max[0],
			build.navAreas[i].areaID, *build.compactHeightField);

	if (this->partitionType_ == NAVMESH_PARTITION_WATERSHED) {
		if (!rcBuildDistanceField(build.ctx, *build.compactHeightField)) {
			std::cout << "Could not build distance field" << std::endl;
			return 0;
		}

		if (!rcBuildRegions(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea,
			cfg.mergeRegionArea)) {
			std::cout << "Could not build regions" << std::endl;
			return 0;
		}
	}else {
		if (!rcBuildRegionsMonotone(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea)) {
			std::cout << "Could not build monotone regions" << std::endl;
			return 0;
		}
	}

	build.heightFieldLayers = rcAllocHeightfieldLayerSet();
	if (!build.heightFieldLayers) {
		std::cout << "Could not allocate height field layer set" << std::endl;
		return 0;
	}

	if (!rcBuildHeightfieldLayers(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.walkableHeight,
		*build.heightFieldLayers)) {
		std::cout << "Could not build height field layers" << std::endl;
		return 0;
	}

	int retCt = 0;
	for (int i = 0; i < build.heightFieldLayers->nlayers; ++i) {
		dtTileCacheLayerHeader header;
		header.magic = DT_TILECACHE_MAGIC;
		header.version = DT_TILECACHE_VERSION;
		header.tx = x;
		header.ty = z;
		header.tlayer = i;

		rcHeightfieldLayer* layer = &build.heightFieldLayers->layers[i];

		// Tile info.
		rcVcopy(header.bmin, layer->bmin);
		rcVcopy(header.bmax, layer->bmax);
		header.width = (unsigned char)layer->width;
		header.height = (unsigned char)layer->height;
		header.minx = (unsigned char)layer->minx;
		header.maxx = (unsigned char)layer->maxx;
		header.miny = (unsigned char)layer->miny;
		header.maxy = (unsigned char)layer->maxy;
		header.hmin = (unsigned short)layer->hmin;
		header.hmax = (unsigned short)layer->hmax;

		if (dtStatusFailed(dtBuildTileCacheLayer(compressor_, &header, layer->heights, layer->areas/*areas*/, layer->cons,&(tiles[retCt].data), &tiles[retCt].dataSize))){
			std::cout << "Failed to build tile cache layers" << std::endl;
			return 0;
		}
		else
			++retCt;
	}
	return retCt;
}

void DynamicNavigationMesh::releaseNavigationMesh() {
	NavigationMesh::releaseNavigationMesh();
	ReleaseTileCache();
}

void DynamicNavigationMesh::ReleaseTileCache() {
	dtFreeTileCache(tileCache_);
	tileCache_ = 0;
}

void DynamicNavigationMesh::OnRenderDebug() {
	NavigationMesh::OnRenderDebug();
	// Draw Obstacle components
	if (drawObstacles_) {
		for (unsigned i = 0; i < m_obstacles.size(); ++i) {
			Obstacle* obstacle = m_obstacles[i];
			if (obstacle && obstacle->m_isEnabled) {
				obstacle->OnRenderDebug();
			}
		}
	}

	// Draw OffMeshConnection components
	if (drawOffMeshConnections_){
		for (unsigned i = 0; i < m_offMeshConnections.size(); ++i) {
			const OffMeshConnection& offMeshConnections = m_offMeshConnections[i];
			if (offMeshConnections.m_isEnabled)
				offMeshConnections.OnRenderDebug();
		}
	}

	// Draw NavArea components
	if (drawNavAreas_){
		for (unsigned i = 0; i < m_navAreas.size(); ++i){
			const NavArea& area = m_navAreas[i];
			if (area.m_isEnabled)
				area.OnRenderDebug();
		}
	}
}

void DynamicNavigationMesh::AddObstacle(Obstacle* obstacle, bool silent) {

	if (tileCache_){
		float pos[3];
		const Vector3f& obsPos = obstacle->m_node->getWorldPosition();
		rcVcopy(pos, obsPos.getVec());
		dtObstacleRef refHolder;
		// Because dtTileCache doesn't process obstacle requests while updating tiles
		// it's necessary update until sufficient request space is available
		while (tileCache_->isObstacleQueueFull()) {
			wait();
		}

		if (dtStatusFailed(tileCache_->addObstacle(pos, obstacle->m_radius, obstacle->m_height, &refHolder))){
			std::cout << "Failed to add obstacle" << std::endl;
			return;
		}
		obstacle->m_obstacleId = refHolder;
		wait();
	}
}

void DynamicNavigationMesh::RemoveObstacle(Obstacle* obstacle, bool silent) {
	
	if (tileCache_ && obstacle->m_obstacleId > 0) {
		while (tileCache_->isObstacleQueueFull()) {
			//tileCache_->update(0.0f, navMesh_);
			wait();
		}
		
		if (dtStatusFailed(tileCache_->removeObstacle(obstacle->m_obstacleId))){
			std::cout << "Failed to remove obstacle" << std::endl;
			return;
		}

		obstacle->m_obstacleId = 0;
	}
}

void DynamicNavigationMesh::ObstacleChanged(Obstacle* obstacle) {
	if (tileCache_) {		
		RemoveObstacle(obstacle, true);
		AddObstacle(obstacle, true);
	}
}

void DynamicNavigationMesh::update(float dt) {
	if (tileCache_ && navMesh_)
		tileCache_->update(0.0f, navMesh_);
}

void DynamicNavigationMesh::wait() {
	bool upToDate = false;
	while (!upToDate){
		tileCache_->update(0.0f, navMesh_, &upToDate);
	}		
}

void DynamicNavigationMesh::removeTile(const std::array<int, 2>& tile, unsigned int layersToRemove) {
	if (!navMesh_)
		return;

	dtCompressedTileRef existing[TILECACHE_MAXLAYERS];
	const int existingCt = tileCache_->getTilesAt(tile[0], tile[1], existing, maxLayers_);
	for (int i = 0; i < existingCt; ++i){
		unsigned char* data = 0x0;
		if (!dtStatusFailed(tileCache_->removeTile(existing[i], &data, 0)) && data != 0x0)
			dtFree(data);
	}

	NavigationMesh::removeTile(tile, layersToRemove);
	return;
}

bool DynamicNavigationMesh::addTile(const Buffer& tileData){
	return ReadTiles(tileData);
}

bool DynamicNavigationMesh::addTile(int x, int z) {
	return addTile(m_tileData.at(z * numTilesX_ + x));
}

bool DynamicNavigationMesh::ReadTiles(const Buffer& source){
	m_tileQueue.clear();

	size_t size = source.size;
	size_t offset = 0;

	while (size > 0){
		dtTileCacheLayerHeader header;
		memcpy(&header, source.data + offset, sizeof(dtTileCacheLayerHeader));

		int dataSize;

		memcpy(&dataSize, source.data + offset + sizeof(dtTileCacheLayerHeader), sizeof(int));
		unsigned char* data = (unsigned char*)dtAlloc(dataSize, DT_ALLOC_PERM);
		if (!data){
			std::cout << "Could not allocate data for navigation mesh tile" << std::endl;
			return false;
		}
		memcpy(data, source.data + offset + sizeof(dtTileCacheLayerHeader) + sizeof(int), dataSize);
		if (dtStatusFailed(tileCache_->addTile(data, dataSize, DT_TILE_FREE_DATA, 0))){
			//std::cout << "Failed to add tile" << std::endl;
			dtFree(data);
			return false;
		}

		const std::array<int, 2> tileIdx = { header.tx, header.ty };
		if (m_tileQueue.empty() || m_tileQueue.back() != tileIdx)
			m_tileQueue.push_back(tileIdx);

		size -= sizeof(dtTileCacheLayerHeader) + sizeof(int) + dataSize;
		offset += sizeof(dtTileCacheLayerHeader) + sizeof(int) + dataSize;
	}

	for (unsigned i = 0; i < m_tileQueue.size(); ++i)
		tileCache_->buildNavMeshTilesAt(m_tileQueue[i][0], m_tileQueue[i][1], navMesh_);

	wait();

	for (unsigned j = 0; j < m_tileQueue.size(); ++j) {
		for(Obstacle* obstacle : m_obstacles)
			obstacle->OnTileAdded(m_tileQueue[j]);		
	}

	if (m_crowdManager) {
		for (unsigned j = 0; j < m_tileQueue.size(); ++j) {
			for (CrowdAgent* agent : m_crowdManager->getAgents())
				if (agent->OnTileAdded(m_tileQueue[j])) {
					m_agentsToReset.push_back(agent);
				}
		}
	}
	return true;
}


Buffer& DynamicNavigationMesh::getTileData(Buffer& buffer, const std::array<int, 2>& tile) const {
	WriteTiles(buffer, tile[0], tile[1]);
	return buffer;
}

Buffer& DynamicNavigationMesh::getTileData(int x, int z) {
	WriteTiles(m_tileData[z * numTilesX_ + x], x, z);
	return m_tileData[z * numTilesX_ + x];
}

void DynamicNavigationMesh::WriteTiles(Buffer& dest, int x, int z) const {
	dtCompressedTileRef tiles[TILECACHE_MAXLAYERS];
	const int ct = tileCache_->getTilesAt(x, z, tiles, maxLayers_);
	for (int i = 0; i < ct; ++i){

		const dtCompressedTile* tile = tileCache_->getTileByRef(tiles[i]);
		if (!tile || !tile->header || !tile->dataSize)
			continue; // Don't write "void-space" tiles
					  // The header conveniently has the majority of the information required

		size_t offset = dest.size;
		dest.resize(sizeof(dtTileCacheLayerHeader) + sizeof(int) + tile->dataSize);
		memcpy(dest.data + offset, tile->header, sizeof(dtTileCacheLayerHeader));
		memcpy(dest.data + offset + sizeof(dtTileCacheLayerHeader), &tile->dataSize, sizeof(int));
		memcpy(dest.data + offset + sizeof(dtTileCacheLayerHeader) + sizeof(int), tile->data, tile->dataSize);
	}
}

void DynamicNavigationMesh::addObstacles() {
	for (unsigned i = 0; i < m_obstacles.size(); ++i) {
		Obstacle* obs = m_obstacles[i];
		if (obs && obs->m_isEnabled)
			AddObstacle(obs);
	}
}

bool DynamicNavigationMesh::IsObstacleInTile(Obstacle* obstacle, const std::array<int, 2>& tile) const {
	const BoundingBox tileBoundingBox = getTileBoudningBox(tile);
	const Vector3f obstaclePosition = obstacle->m_node->getWorldPosition();
	return tileBoundingBox.distance(obstaclePosition) < obstacle->m_radius;
}

void Buffer::resize(size_t _size) {
	size_t newSize = size + _size;
	char* newArr = new char[newSize];

	memcpy(newArr, data, size * sizeof(char));

	size = newSize;
	delete[] data;
	data = newArr;
}