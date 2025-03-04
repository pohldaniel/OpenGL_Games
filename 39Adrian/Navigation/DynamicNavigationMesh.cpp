#include <lz4.h>
#include <DetourNavMesh.h>
#include <DetourTileCache.h>
#include <DetourNavMeshBuilder.h>
#include <DetourTileCacheBuilder.h>
#include <Recast.h>

#include "DynamicNavigationMesh.h"
#include "NavBuildData.h"
#include "Obstacle.h"
#include "OffMeshConnection.h"

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
		for (int i = 0; i < params->polyCount; ++i)
		{
			if (polyAreas[i] != RC_NULL_AREA)
				polyFlags[i] = RC_WALKABLE_AREA;
		}

		BoundingBox bounds;
		rcVcopy(&bounds.min[0], params->bmin);
		rcVcopy(&bounds.max[0], params->bmin);

		// collect off-mesh connections
		/*std::vector<OffMeshConnection*> offMeshConnections = owner_->CollectOffMeshConnections(bounds);
		if (offMeshConnections.size() > 0) {
			if (offMeshConnections.size() != offMeshRadii_.size()) {

				//Matrix3x4 inverse = owner_->GetNode()->GetWorldTransform().Inverse();
				Matrix4f inverse = Matrix4f::IDENTITY;
				ClearConnectionData();
				for (unsigned i = 0; i < offMeshConnections.size(); ++i) {

					OffMeshConnection* connection = offMeshConnections[i];
					Vector3f start = inverse * connection->GetNode()->GetWorldPosition();
					Vector3f end = inverse * connection->GetEndPoint()->GetWorldPosition();

					offMeshVertices_.push_back(start);
					offMeshVertices_.push_back(end);
					offMeshRadii_.push_back(connection->GetRadius());
					offMeshFlags_.push_back((unsigned short)connection->GetMask());
					offMeshAreas_.push_back((unsigned char)connection->GetAreaID());
					offMeshDir_.push_back((unsigned char)(connection->IsBidirectional() ? DT_OFFMESH_CON_BIDIR : 0));
				}
			}
			params->offMeshConCount = offMeshRadii_.size();
			params->offMeshConVerts = &offMeshVertices_[0][0];
			params->offMeshConRad = &offMeshRadii_[0];
			params->offMeshConFlags = &offMeshFlags_[0];
			params->offMeshConAreas = &offMeshAreas_[0];
			params->offMeshConDir = &offMeshDir_[0];
		}*/
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
	tileCache_(nullptr),
	maxObstacles_(DEFAULT_MAX_OBSTACLES),
	maxLayers_(DEFAULT_MAX_LAYERS) {
	tileSize_ = 64;
	partitionType_ = NAVMESH_PARTITION_MONOTONE;
	allocator_ = new LinearAllocator(32000); //32kb to start
	compressor_ = new TileCompressor();
	meshProcessor_ = new MeshProcess(this);
}

DynamicNavigationMesh::~DynamicNavigationMesh() {

}

bool DynamicNavigationMesh::Allocate(const BoundingBox& boundingBox, unsigned maxTiles) {

	// Release existing navigation data and zero the bounding box
	ReleaseNavigationMesh();

	//if (!node_)
		//return false;

	//if (!node_->GetWorldScale().Equals(Vector3::ONE))
		//URHO3D_LOGWARNING("Navigation mesh root node has scaling. Agent parameters may not work as intended");

	//boundingBox_ = boundingBox.Transformed(node_->GetWorldTransform().Inverse());

	std::vector<NavigationGeometryInfo> geometryList;
	CollectGeometries(geometryList);

	if (geometryList.empty()) {
		std::cout << "Nothing to do" << std::endl;
		return true; // Nothing to do
	}

	boundingBox_ = boundingBox;
	maxTiles = NextPowerOfTwo(maxTiles);

	// Calculate number of tiles
	int gridW = 0, gridH = 0;
	float tileEdgeLength = (float)tileSize_ * cellSize_;
	rcCalcGridSize(&boundingBox_.min[0], &boundingBox_.max[0], cellSize_, &gridW, &gridH);
	numTilesX_ = (gridW + tileSize_ - 1) / tileSize_;
	numTilesZ_ = (gridH + tileSize_ - 1) / tileSize_;

	// Calculate max number of polygons, 22 bits available to identify both tile & polygon within tile
	unsigned tileBits = LogBaseTwo(maxTiles);
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
		ReleaseNavigationMesh();
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
		ReleaseNavigationMesh();
		return false;
	}

	if (dtStatusFailed(tileCache_->init(&tileCacheParams, allocator_, compressor_, meshProcessor_))){
		std::cout << "Could not initialize tile cache" << std::endl;;
		ReleaseNavigationMesh();
		return false;
	}

	// Build each tile
	unsigned numTiles = 0;

	for (int z = 0; z < numTilesZ_; ++z)
	{
		for (int x = 0; x < numTilesX_; ++x)
		{
			TileCacheData tiles[TILECACHE_MAXLAYERS];
			int layerCt = BuildTile(geometryList, x, z, tiles);
			for (int i = 0; i < layerCt; ++i)
			{
				dtCompressedTileRef tileRef;
				int status = tileCache_->addTile(tiles[i].data, tiles[i].dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tileRef);
				if (dtStatusFailed((dtStatus)status))
				{
					dtFree(tiles[i].data);
					tiles[i].data = 0x0;
				}
			}
			tileCache_->buildNavMeshTilesAt(x, z, navMesh_);
			++numTiles;
		}
	}

	// For a full build it's necessary to update the nav mesh
	// not doing so will cause dependent components to crash, like CrowdManager
	tileCache_->update(0.0f, navMesh_);

	std::cout << "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;

	// Scan for obstacles to insert into us
	std::vector<SceneNodeLC*> obstacles;
	for (unsigned i = 0; i < m_obstacles.size(); ++i){
		Obstacle* obs = m_obstacles[i];
		if (obs && obs->isEnabled_)
			AddObstacle(obs);
	}

	return true;
}

bool DynamicNavigationMesh::Build() {
	//URHO3D_PROFILE(BuildNavigationMesh);

	// Release existing navigation data and zero the bounding box
	ReleaseNavigationMesh();

	//if (!node_)
		//return false;

	//if (!node_->GetWorldScale().Equals(Vector3::ONE))
		//URHO3D_LOGWARNING("Navigation mesh root node has scaling. Agent parameters may not work as intended");

	std::vector<NavigationGeometryInfo> geometryList;
	CollectGeometries(geometryList);

	if (geometryList.empty()) {
		std::cout << "Nothing to do" << std::endl;
		return true; // Nothing to do
	}

	// Build the combined bounding box
	for (unsigned i = 0; i < geometryList.size(); ++i)
		boundingBox_.merge(geometryList[i].boundingBox_);

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
		unsigned maxTiles = NextPowerOfTwo((unsigned)(numTilesX_ * numTilesZ_));
		unsigned tileBits = LogBaseTwo(maxTiles);
		unsigned maxPolys = (unsigned)(1 << (22 - tileBits));

		dtNavMeshParams params;
		rcVcopy(params.orig, &boundingBox_.min[0]);
		params.tileWidth = tileEdgeLength;
		params.tileHeight = tileEdgeLength;
		params.maxTiles = maxTiles;
		params.maxPolys = maxPolys;

		navMesh_ = dtAllocNavMesh();
		if (!navMesh_)
		{
			std::cout << "Could not allocate navigation mesh" << std::endl;
			return false;
		}

		if (dtStatusFailed(navMesh_->init(&params))){
			std::cout << "Could not initialize navigation mesh" << std::endl;
			ReleaseNavigationMesh();
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
			ReleaseNavigationMesh();
			return false;
		}

		if (dtStatusFailed(tileCache_->init(&tileCacheParams, allocator_, compressor_, meshProcessor_))){
			std::cout << "Could not initialize tile cache" << std::endl;
			ReleaseNavigationMesh();
			return false;
		}

		// Build each tile
		unsigned numTiles = 0;

		for (int z = 0; z < numTilesZ_; ++z)
		{
			for (int x = 0; x < numTilesX_; ++x)
			{
				TileCacheData tiles[TILECACHE_MAXLAYERS];
				int layerCt = BuildTile(geometryList, x, z, tiles);
				for (int i = 0; i < layerCt; ++i)
				{
					dtCompressedTileRef tileRef;
					int status = tileCache_->addTile(tiles[i].data, tiles[i].dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tileRef);
					if (dtStatusFailed((dtStatus)status))
					{
						dtFree(tiles[i].data);
						tiles[i].data = 0x0;
					}
				}
				tileCache_->buildNavMeshTilesAt(x, z, navMesh_);
				++numTiles;
			}
		}

		// For a full build it's necessary to update the nav mesh
	    // not doing so will cause dependent components to crash, like CrowdManager
		tileCache_->update(0.0f, navMesh_);

		std::cout << "Built navigation mesh with " + std::to_string(numTiles) + " tiles" << std::endl;

		// Send a notification event to concerned parties that we've been fully rebuilt
		/*{
			using namespace NavigationMeshRebuilt;
			VariantMap& buildEventParams = GetContext()->GetEventDataMap();
			buildEventParams[P_NODE] = node_;
			buildEventParams[P_MESH] = this;
			SendEvent(E_NAVIGATION_MESH_REBUILT, buildEventParams);
		}*/

		// Scan for obstacles to insert into us
		for (unsigned i = 0; i < m_obstacles.size(); ++i){
			Obstacle* obs = m_obstacles[i];
			if (obs && obs->isEnabled_)
				AddObstacle(obs);
		}

		return true;
	}
}

int DynamicNavigationMesh::BuildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z, TileCacheData* tiles) {
	//URHO3D_PROFILE(BuildNavigationMeshTile);

	tileCache_->removeTile(navMesh_->getTileRefAt(x, z, 0), 0, 0);

	BoundingBox tileBoundingBox = GetTileBoudningBox(std::array<int, 2>({ x, z }));

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

	if (build.vertices_.empty() || build.indices_.empty())
		return 0; // Nothing to do

	build.heightField_ = rcAllocHeightfield();
	if (!build.heightField_)
	{
		std::cout << "Could not allocate heightfield" << std::endl;
		return 0;
	}

	if (!rcCreateHeightfield(build.ctx_, *build.heightField_, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs,
		cfg.ch))
	{
		std::cout << "Could not create heightfield" << std::endl;
		return 0;
	}

	unsigned numTriangles = build.indices_.size() / 3;
	unsigned char* triAreas = new unsigned char[numTriangles];
	memset(triAreas, 0, numTriangles);

	rcMarkWalkableTriangles(build.ctx_, cfg.walkableSlopeAngle, &build.vertices_[0][0], build.vertices_.size(),
		&build.indices_[0], numTriangles, triAreas);
	rcRasterizeTriangles(build.ctx_, &build.vertices_[0][0], build.vertices_.size(), &build.indices_[0],
		triAreas, numTriangles, *build.heightField_, cfg.walkableClimb);
	rcFilterLowHangingWalkableObstacles(build.ctx_, cfg.walkableClimb, *build.heightField_);

	rcFilterLedgeSpans(build.ctx_, cfg.walkableHeight, cfg.walkableClimb, *build.heightField_);
	rcFilterWalkableLowHeightSpans(build.ctx_, cfg.walkableHeight, *build.heightField_);

	build.compactHeightField_ = rcAllocCompactHeightfield();
	if (!build.compactHeightField_) {
		std::cout << "Could not allocate create compact heightfield" << std::endl;
		return 0;
	}

	if (!rcBuildCompactHeightfield(build.ctx_, cfg.walkableHeight, cfg.walkableClimb, *build.heightField_,
		*build.compactHeightField_)) {
		std::cout << "Could not build compact heightfield" << std::endl;
		return 0;
	}

	if (!rcErodeWalkableArea(build.ctx_, cfg.walkableRadius, *build.compactHeightField_)) {
		std::cout << "Could not erode compact heightfield" << std::endl;
		return 0;
	}

	// area volumes
	for (unsigned i = 0; i < build.navAreas_.size(); ++i)
		rcMarkBoxArea(build.ctx_, &build.navAreas_[i].bounds_.min[0], &build.navAreas_[i].bounds_.max[0],
			build.navAreas_[i].areaID_, *build.compactHeightField_);

	if (this->partitionType_ == NAVMESH_PARTITION_WATERSHED) {
		if (!rcBuildDistanceField(build.ctx_, *build.compactHeightField_)) {
			std::cout << "Could not build distance field" << std::endl;
			return 0;
		}

		if (!rcBuildRegions(build.ctx_, *build.compactHeightField_, cfg.borderSize, cfg.minRegionArea,
			cfg.mergeRegionArea)) {
			std::cout << "Could not build regions" << std::endl;
			return 0;
		}
	}else {
		if (!rcBuildRegionsMonotone(build.ctx_, *build.compactHeightField_, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea)) {
			std::cout << "Could not build monotone regions" << std::endl;
			return 0;
		}
	}

	build.heightFieldLayers_ = rcAllocHeightfieldLayerSet();
	if (!build.heightFieldLayers_) {
		std::cout << "Could not allocate height field layer set" << std::endl;
		return 0;
	}

	if (!rcBuildHeightfieldLayers(build.ctx_, *build.compactHeightField_, cfg.borderSize, cfg.walkableHeight,
		*build.heightFieldLayers_)) {
		std::cout << "Could not build height field layers" << std::endl;
		return 0;
	}

	int retCt = 0;
	for (int i = 0; i < build.heightFieldLayers_->nlayers; ++i) {
		dtTileCacheLayerHeader header;
		header.magic = DT_TILECACHE_MAGIC;
		header.version = DT_TILECACHE_VERSION;
		header.tx = x;
		header.ty = z;
		header.tlayer = i;

		rcHeightfieldLayer* layer = &build.heightFieldLayers_->layers[i];

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

		if (dtStatusFailed(
			dtBuildTileCacheLayer(compressor_/*compressor*/, &header, layer->heights, layer->areas/*areas*/, layer->cons,
				&(tiles[retCt].data), &tiles[retCt].dataSize)))
		{
			std::cout << "Failed to build tile cache layers" << std::endl;
			return 0;
		}
		else
			++retCt;
	}

	// Send a notification of the rebuild of this tile to anyone interested
	/*{
		using namespace NavigationAreaRebuilt;
		VariantMap& eventData = GetContext()->GetEventDataMap();
		eventData[P_NODE] = GetNode();
		eventData[P_MESH] = this;
		eventData[P_BOUNDSMIN] = Variant(tileBoundingBox.min_);
		eventData[P_BOUNDSMAX] = Variant(tileBoundingBox.max_);
		SendEvent(E_NAVIGATION_AREA_REBUILT, eventData);
	}*/

	return retCt;
}

void DynamicNavigationMesh::ReleaseNavigationMesh() {
	NavigationMesh::ReleaseNavigationMesh();
	ReleaseTileCache();
}

void DynamicNavigationMesh::ReleaseTileCache() {
	dtFreeTileCache(tileCache_);
	tileCache_ = 0;
}

void DynamicNavigationMesh::OnRenderDebug() {
	NavigationMesh::OnRenderDebug();

	if (drawObstacles_) {
		for (unsigned i = 0; i < m_obstacles.size(); ++i) {
			Obstacle* obstacle = m_obstacles[i];
			if (obstacle && obstacle->isEnabled_)
				obstacle->OnRenderDebug();
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
			tileCache_->update(0.0f, navMesh_);
		}

		if (dtStatusFailed(tileCache_->addObstacle(pos, obstacle->GetRadius(), obstacle->GetHeight(), &refHolder))){
			std::cout << "Failed to add obstacle" << std::endl;
			return;
		}
		obstacle->obstacleId_ = refHolder;

		wait();
	}
}

void DynamicNavigationMesh::RemoveObstacle(Obstacle* obstacle, bool silent) {
	
	if (tileCache_ && obstacle->obstacleId_ > 0) {
		while (tileCache_->isObstacleQueueFull())
			tileCache_->update(0.0f, navMesh_);

		if (dtStatusFailed(tileCache_->removeObstacle(obstacle->obstacleId_))){
			std::cout << "Failed to remove obstacle" << std::endl;
			return;
		}

		obstacle->obstacleId_ = 0;
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

void DynamicNavigationMesh::RemoveTile(const std::array<int, 2>& tile) {
	if (!navMesh_)
		return;

	dtCompressedTileRef existing[TILECACHE_MAXLAYERS];
	const int existingCt = tileCache_->getTilesAt(tile[0], tile[1], existing, maxLayers_);
	for (int i = 0; i < existingCt; ++i){
		unsigned char* data = 0x0;
		if (!dtStatusFailed(tileCache_->removeTile(existing[i], &data, 0)) && data != 0x0)
			dtFree(data);
	}

	NavigationMesh::RemoveTile(tile);
}

bool DynamicNavigationMesh::AddTile(const Buffer& tileData){
	//MemoryBuffer buffer(tileData);
	//return ReadTiles(buffer, false);
	return ReadTiles(tileData);
}

bool DynamicNavigationMesh::ReadTiles(const Buffer& source){
	m_tileQueue.clear();
	size_t size = source.size;
	size_t offset = 0;

	while (size >= 0){
		dtTileCacheLayerHeader header;
		memcpy(&header, source.data + offset, sizeof(dtTileCacheLayerHeader));

		int dataSize;

		memcpy(&dataSize, source.data + offset + sizeof(dtTileCacheLayerHeader), sizeof(int));
		unsigned char* data = (unsigned char*)dtAlloc(dataSize, DT_ALLOC_PERM);
		if (!data){
			std::cout << "Could not allocate data for navigation mesh tile" << std::endl;
			return false;
		}

		memcpy(&data, source.data + offset + sizeof(dtTileCacheLayerHeader) + sizeof(int), dataSize);
		if (dtStatusFailed(tileCache_->addTile(data, dataSize, DT_TILE_FREE_DATA, 0))){
			std::cout << "Failed to add tile" << std::endl;
			dtFree(data);
			return false;
		}

		const std::array<int, 2> tileIdx = { header.tx, header.ty };
		if (m_tileQueue.empty() || m_tileQueue.back() != tileIdx)
			m_tileQueue.push_back(tileIdx);

		size -= offset + sizeof(dtTileCacheLayerHeader) + sizeof(int);
	}

	for (unsigned i = 0; i < m_tileQueue.size(); ++i)
		tileCache_->buildNavMeshTilesAt(m_tileQueue[i][0], m_tileQueue[i][1], navMesh_);

	tileCache_->update(0, navMesh_);
	return true;
}

Buffer DynamicNavigationMesh::GetTileData(const std::array<int, 2>& tile) const {
	Buffer ret;
	WriteTiles(ret, tile[0], tile[1]);
	return ret;
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
		memcpy(dest.data + offset  + sizeof(dtTileCacheLayerHeader) + sizeof(int), tile->data, tile->dataSize);

		//dest.Write(tile->header, sizeof(dtTileCacheLayerHeader));
		//dest.WriteInt(tile->dataSize);
		//dest.Write(tile->data, (unsigned)tile->dataSize);
	}
}

void Buffer::resize(size_t _size) {
	size_t newSize = size + _size;
	unsigned char* newArr = new unsigned char[newSize];

	memcpy(newArr, data, size * sizeof(unsigned char));

	size = newSize;
	delete[] data;
	data = newArr;
}