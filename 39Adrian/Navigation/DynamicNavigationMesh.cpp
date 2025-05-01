#include <lz4.h>
#include <DetourNavMesh.h>
#include <DetourTileCache.h>
#include <DetourNavMeshBuilder.h>
#include <DetourTileCacheBuilder.h>
#include <Recast.h>

#include "DynamicNavigationMesh.h"
#include "OffMeshConnection.h"
#include "NavArea.h"
#include "NavPolygon.h"
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
	m_drawObstacles(true),
	m_tileCache(nullptr),
	m_crowdManager(nullptr),
	m_maxObstacles(DEFAULT_MAX_OBSTACLES),
	m_maxLayers(DEFAULT_MAX_LAYERS) {
	m_tileSize = 64;
	m_numTiles = 0u;
	m_partitionType = NAVMESH_PARTITION_MONOTONE;
	m_allocator = new LinearAllocator(32000); //32kb to start
	m_compressor = new TileCompressor();
	m_meshProcessor = new MeshProcess(this);
}

DynamicNavigationMesh::~DynamicNavigationMesh() {
	removeObstacles();
	releaseNavigationMesh();
}

bool DynamicNavigationMesh::allocate() {
	return allocate(m_boundingBox, m_numTilesX, m_numTilesZ);
}

bool DynamicNavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int tilesX, unsigned int tilesZ) {
	Vector3f min = boundingBox.min;
	Vector3f max = boundingBox.max;

	releaseNavigationMesh();
	m_boundingBox.setMin(min);
	m_boundingBox.setMax(max);

	m_numTilesX = tilesX;
	m_numTilesZ = tilesZ;
	unsigned maxTiles = Math::NextPowerOfTwo((unsigned)(m_numTilesX * m_numTilesZ));
	unsigned tileBits = Math::LogBaseTwo(maxTiles);
	unsigned maxPolys = (unsigned)(1 << (22 - tileBits));
	float tileEdgeLength = (float)m_tileSize * m_cellSize;

	dtNavMeshParams params;
	rcVcopy(params.orig, &m_boundingBox.min[0]);
	params.tileWidth = tileEdgeLength;
	params.tileHeight = tileEdgeLength;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolys;

	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh) {
		std::cout << "Could not allocate navigation mesh" << std::endl;
		return false;
	}

	if (dtStatusFailed(m_navMesh->init(&params))) {
		std::cout << "Could not initialize navigation mesh" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	dtTileCacheParams tileCacheParams;
	memset(&tileCacheParams, 0, sizeof(tileCacheParams));
	rcVcopy(tileCacheParams.orig, &m_boundingBox.min[0]);
	tileCacheParams.ch = m_cellHeight;
	tileCacheParams.cs = m_cellSize;
	tileCacheParams.width = m_tileSize;
	tileCacheParams.height = m_tileSize;
	tileCacheParams.maxSimplificationError = m_edgeMaxError;
	tileCacheParams.maxTiles = m_numTilesX * m_numTilesZ * m_maxLayers;
	tileCacheParams.maxObstacles = m_maxObstacles;
	// Settings from NavigationMesh
	tileCacheParams.walkableClimb = m_agentMaxClimb;
	tileCacheParams.walkableHeight = m_agentHeight;
	tileCacheParams.walkableRadius = m_agentRadius;

	m_tileCache = dtAllocTileCache();
	if (!m_tileCache) {
		std::cout << "Could not allocate tile cache" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	if (dtStatusFailed(m_tileCache->init(&tileCacheParams, m_allocator, m_compressor, m_meshProcessor))) {
		std::cout << "Could not initialize tile cache" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	//wait();
	std::cout << "Built navigation mesh with " + std::to_string(m_numTilesX * m_numTilesZ) + " tiles" << std::endl;

	return true;
}

bool DynamicNavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int maxTiles) {
	
	// Release existing navigation data and zero the bounding box
	releaseNavigationMesh();
	m_boundingBox = boundingBox;
	maxTiles = Math::NextPowerOfTwo(maxTiles);

	// Calculate number of tiles
	int gridW = 0, gridH = 0;
	float tileEdgeLength = (float)m_tileSize * m_cellSize;
	rcCalcGridSize(&m_boundingBox.min[0], &m_boundingBox.max[0], m_cellSize, &gridW, &gridH);
	m_numTilesX = (gridW + m_tileSize - 1) / m_tileSize;
	m_numTilesZ = (gridH + m_tileSize - 1) / m_tileSize;
	// Calculate max number of polygons, 22 bits available to identify both tile & polygon within tile
	unsigned tileBits = Math::LogBaseTwo(maxTiles);
	unsigned maxPolys = (unsigned)(1 << (22 - tileBits));

	dtNavMeshParams params;
	rcVcopy(params.orig, &m_boundingBox.min[0]);
	params.tileWidth = tileEdgeLength;
	params.tileHeight = tileEdgeLength;
	params.maxTiles = maxTiles;
	params.maxPolys = maxPolys;
	
	m_navMesh = dtAllocNavMesh();
	if (!m_navMesh){
		std::cout << "Could not allocate navigation mesh" << std::endl;
		return false;
	}

	if (dtStatusFailed(m_navMesh->init(&params))){
		std::cout << "Could not initialize navigation mesh" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	dtTileCacheParams tileCacheParams;
	memset(&tileCacheParams, 0, sizeof(tileCacheParams));
	rcVcopy(tileCacheParams.orig, &m_boundingBox.min[0]);
	tileCacheParams.ch = m_cellHeight;
	tileCacheParams.cs = m_cellSize;
	tileCacheParams.width = m_tileSize;
	tileCacheParams.height = m_tileSize;
	tileCacheParams.maxSimplificationError = m_edgeMaxError;
	tileCacheParams.maxTiles = maxTiles * m_maxLayers;
	tileCacheParams.maxObstacles = m_maxObstacles;
	// Settings from NavigationMesh
	tileCacheParams.walkableClimb = m_agentMaxClimb;
	tileCacheParams.walkableHeight = m_agentHeight;
	tileCacheParams.walkableRadius = m_agentRadius;

	m_tileCache = dtAllocTileCache();
	if (!m_tileCache){
		std:: cout << "Could not allocate tile cache" << std::endl;
		releaseNavigationMesh();
		return false;
	}

	if (dtStatusFailed(m_tileCache->init(&tileCacheParams, m_allocator, m_compressor, m_meshProcessor))){
		std::cout << "Could not initialize tile cache" << std::endl;;
		releaseNavigationMesh();
		return false;
	}

	std::cout << "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;
	return true;
}

bool DynamicNavigationMesh::build() {
	releaseNavigationMesh();

	std::vector<NavigationGeometryInfo> geometryList;
	collectGeometries(geometryList);

	if (geometryList.empty()) {
		std::cout << "Nothing to do" << std::endl;
		return true; // Nothing to do
	}

	// Build the combined bounding box
	for (unsigned i = 0; i < geometryList.size(); ++i)
		m_boundingBox.merge(geometryList[i].boundingBox);

	// Expand bounding box by padding
	m_boundingBox.min -= m_padding;
	m_boundingBox.max += m_padding;
	{
		// Calculate number of tiles
		int gridW = 0, gridH = 0;
		float tileEdgeLength = (float)m_tileSize * m_cellSize;
		rcCalcGridSize(&m_boundingBox.min[0], &m_boundingBox.max[0], m_cellSize, &gridW, &gridH);
		m_numTilesX = (gridW + m_tileSize - 1) / m_tileSize;
		m_numTilesZ = (gridH + m_tileSize - 1) / m_tileSize;

		// Calculate max. number of tiles and polygons, 22 bits available to identify both tile & polygon within tile
		unsigned maxTiles = Math::NextPowerOfTwo((unsigned)(m_numTilesX * m_numTilesZ));
		unsigned tileBits = Math::LogBaseTwo(maxTiles);
		unsigned maxPolys = (unsigned)(1 << (22 - tileBits));

		dtNavMeshParams params;
		rcVcopy(params.orig, &m_boundingBox.min[0]);
		params.tileWidth = tileEdgeLength;
		params.tileHeight = tileEdgeLength;
		params.maxTiles = maxTiles;
		params.maxPolys = maxPolys;

		m_navMesh = dtAllocNavMesh();
		if (!m_navMesh){
			std::cout << "Could not allocate navigation mesh" << std::endl;
			return false;
		}

		if (dtStatusFailed(m_navMesh->init(&params))){
			std::cout << "Could not initialize navigation mesh" << std::endl;
			releaseNavigationMesh();
			return false;
		}
		
		dtTileCacheParams tileCacheParams;
		memset(&tileCacheParams, 0, sizeof(tileCacheParams));
		rcVcopy(tileCacheParams.orig, &m_boundingBox.min[0]);
		tileCacheParams.ch = m_cellHeight;
		tileCacheParams.cs = m_cellSize;
		tileCacheParams.width = m_tileSize;
		tileCacheParams.height = m_tileSize;
		tileCacheParams.maxSimplificationError = m_edgeMaxError;
		tileCacheParams.maxTiles = m_numTilesX * m_numTilesZ * m_maxLayers;
		tileCacheParams.maxObstacles = m_maxObstacles;
		// Settings from NavigationMesh
		tileCacheParams.walkableClimb = m_agentMaxClimb;
		tileCacheParams.walkableHeight = m_agentHeight;
		tileCacheParams.walkableRadius = m_agentRadius;

		m_tileCache = dtAllocTileCache();
		if (!m_tileCache){
			std::cout << "Could not allocate tile cache" << std::endl;
			releaseNavigationMesh();
			return false;
		}

		if (dtStatusFailed(m_tileCache->init(&tileCacheParams, m_allocator, m_compressor, m_meshProcessor))){
			std::cout << "Could not initialize tile cache" << std::endl;
			releaseNavigationMesh();
			return false;
		}

		// Build each tile
		for (int z = 0; z < m_numTilesZ; ++z){
			for (int x = 0; x < m_numTilesX; ++x){
				TileCacheData tiles[TILECACHE_MAXLAYERS];
				int layerCt = buildTile(geometryList, x, z, tiles);
				for (int i = 0; i < layerCt; ++i){
					dtCompressedTileRef tileRef;
					int status = m_tileCache->addTile(tiles[i].data, tiles[i].dataSize, DT_COMPRESSEDTILE_FREE_DATA, &tileRef);
					if (dtStatusFailed((dtStatus)status)){
						dtFree(tiles[i].data);
						tiles[i].data = 0x0;
					}
				}
				m_tileCache->buildNavMeshTilesAt(x, z, m_navMesh);
				++m_numTiles;
			}
		}

		wait();
		std::cout << "Built navigation mesh with " + std::to_string(m_numTiles) + " tiles" << std::endl;

		addObstacles();
		return true;
	}
}

int DynamicNavigationMesh::buildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z, TileCacheData* tiles) {
	
	m_tileCache->removeTile(m_navMesh->getTileRefAt(x, z, 0), 0, 0);
	BoundingBox tileBoundingBox = getTileBoudningBox(std::array<int, 2>({ x, z }));
	DynamicNavBuildData build(m_allocator);

	rcConfig cfg;
	memset(&cfg, 0, sizeof cfg);
	cfg.cs = m_cellSize;
	cfg.ch = m_cellHeight;
	cfg.walkableSlopeAngle = m_agentMaxSlope;
	cfg.walkableHeight = (int)ceilf(m_agentHeight / cfg.ch);
	cfg.walkableClimb = (int)floorf(m_agentMaxClimb / cfg.ch);
	cfg.walkableRadius = (int)ceilf(m_agentRadius / cfg.cs);
	cfg.maxEdgeLen = (int)(m_edgeMaxLength / m_cellSize);
	cfg.maxSimplificationError = m_edgeMaxError;
	cfg.minRegionArea = (int)sqrtf(m_regionMinSize);
	cfg.mergeRegionArea = (int)sqrtf(m_regionMergeSize);
	cfg.maxVertsPerPoly = 6;
	cfg.tileSize = m_tileSize;
	cfg.borderSize = cfg.walkableRadius + 3; // Add padding
	cfg.width = cfg.tileSize + cfg.borderSize * 2;
	cfg.height = cfg.tileSize + cfg.borderSize * 2;
	cfg.detailSampleDist = m_detailSampleDistance < 0.9f ? 0.0f : m_cellSize * m_detailSampleDistance;
	cfg.detailSampleMaxError = m_cellHeight * m_detailSampleMaxError;

	rcVcopy(cfg.bmin, &tileBoundingBox.min[0]);
	rcVcopy(cfg.bmax, &tileBoundingBox.max[0]);
	cfg.bmin[0] -= cfg.borderSize * cfg.cs;
	cfg.bmin[2] -= cfg.borderSize * cfg.cs;
	cfg.bmax[0] += cfg.borderSize * cfg.cs;
	cfg.bmax[2] += cfg.borderSize * cfg.cs;

	BoundingBox expandedBox(*reinterpret_cast<Vector3f*>(cfg.bmin), *reinterpret_cast<Vector3f*>(cfg.bmax));
	getTileGeometry(&build, geometryList, expandedBox);

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

	// Mark area volumes
	for (unsigned i = 0; i < build.navAreas.size(); ++i)
		rcMarkBoxArea(build.ctx, &build.navAreas[i].bounds.min[0], &build.navAreas[i].bounds.max[0],
			build.navAreas[i].areaID, *build.compactHeightField);

	// Mark polygon volumes
	for (unsigned i = 0; i < build.polygons.size(); ++i)
		rcMarkConvexPolyArea(build.ctx, build.polygons[i].verts, build.polygons[i].numVerts, build.polygons[i].minY, build.polygons[i].maxY,
			build.polygons[i].areaID, *build.compactHeightField);

	if (m_partitionType == NAVMESH_PARTITION_WATERSHED) {
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

		if (dtStatusFailed(dtBuildTileCacheLayer(m_compressor, &header, layer->heights, layer->areas/*areas*/, layer->cons,&(tiles[retCt].data), &tiles[retCt].dataSize))){
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
	releaseTileCache();
	m_numTiles = 0u;	
}

void DynamicNavigationMesh::releaseTileCache() {
	dtFreeTileCache(m_tileCache);
	m_tileCache = nullptr;
}

void DynamicNavigationMesh::OnRenderDebug() {
	NavigationMesh::OnRenderDebug();
	// Draw Obstacle components
	if (m_drawObstacles) {
		for (unsigned i = 0; i < m_obstacles.size(); ++i) {
			Obstacle* obstacle = m_obstacles[i];
			if (obstacle && obstacle->m_isEnabled) {
				obstacle->OnRenderDebug();
			}
		}
	}
}

void DynamicNavigationMesh::addObstacle(Obstacle* obstacle, bool force) {
	if (force) {
		obstacle->setOwnerMesh(this);
		m_obstacles.push_back(obstacle);
	}

	if (m_tileCache){
		float pos[3];
		const Vector3f& obsPos = obstacle->m_node->getWorldPosition();
		rcVcopy(pos, obsPos.getVec());
		dtObstacleRef refHolder;
		// Because dtTileCache doesn't process obstacle requests while updating tiles
		// it's necessary update until sufficient request space is available
		while (m_tileCache->isObstacleQueueFull()) {
			wait();
		}

		if (dtStatusFailed(m_tileCache->addObstacle(pos, obstacle->m_radius, obstacle->m_height, &refHolder))){
			std::cout << "Failed to add obstacle" << std::endl;
			return;
		}
		obstacle->m_obstacleId = refHolder;
		wait();
	}
}

void DynamicNavigationMesh::removeObstacle(Obstacle* obstacle, bool force) {
	
	if (m_tileCache && obstacle->m_obstacleId > 0) {
		while (m_tileCache->isObstacleQueueFull()) {
			wait();
		}
		
		if (dtStatusFailed(m_tileCache->removeObstacle(obstacle->m_obstacleId))){
			std::cout << "Failed to remove obstacle" << std::endl;
			return;
		}
		wait();

		obstacle->m_obstacleId = 0;		

		if (force) {
			delete obstacle;
			m_obstacles.erase(std::remove(m_obstacles.begin(), m_obstacles.end(), obstacle), m_obstacles.end());
		}
	}
}

void DynamicNavigationMesh::obstacleChanged(Obstacle* obstacle) {
	if (m_tileCache) {
		removeObstacle(obstacle, false);
		addObstacle(obstacle, false);
	}
}

void DynamicNavigationMesh::update(float dt) {
	if (m_tileCache && m_navMesh)
		m_tileCache->update(0.0f, m_navMesh);
}

void DynamicNavigationMesh::wait() {
	bool upToDate = false;
	while (!upToDate){
		m_tileCache->update(0.0f, m_navMesh, &upToDate);
	}		
}

void DynamicNavigationMesh::removeTile(const std::array<int, 2>& tile, unsigned int layersToRemove) {
	if (!m_navMesh)
		return;

	dtCompressedTileRef existing[TILECACHE_MAXLAYERS];
	const int existingCt = m_tileCache->getTilesAt(tile[0], tile[1], existing, m_maxLayers);
	for (int i = 0; i < existingCt; ++i){
		unsigned char* data = 0x0;
		if (!dtStatusFailed(m_tileCache->removeTile(existing[i], &data, 0)) && data != 0x0)
			dtFree(data);
	}

	NavigationMesh::removeTile(tile, layersToRemove);
	return;
}

bool DynamicNavigationMesh::addTile(const Buffer& tileData){
	return readTiles(tileData);
}

bool DynamicNavigationMesh::addTile(int x, int z) {
	return addTile(m_tileData.at(z * m_numTilesX + x));
}

bool DynamicNavigationMesh::readTiles(const Buffer& source){
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
		if (dtStatusFailed(m_tileCache->addTile(data, dataSize, DT_TILE_FREE_DATA, 0))){
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
		m_tileCache->buildNavMeshTilesAt(m_tileQueue[i][0], m_tileQueue[i][1], m_navMesh);

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
	writeTiles(buffer, tile[0], tile[1]);
	return buffer;
}

Buffer& DynamicNavigationMesh::getTileData(int x, int z) {
	writeTiles(m_tileData[z * m_numTilesX + x], x, z);
	return m_tileData[z * m_numTilesX + x];
}

void DynamicNavigationMesh::writeTiles(Buffer& dest, int x, int z) const {
	dtCompressedTileRef tiles[TILECACHE_MAXLAYERS];
	const int ct = m_tileCache->getTilesAt(x, z, tiles, m_maxLayers);
	for (int i = 0; i < ct; ++i){

		const dtCompressedTile* tile = m_tileCache->getTileByRef(tiles[i]);
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
			addObstacle(obs, false);
	}
}

void DynamicNavigationMesh::removeObstacles() {
	for (unsigned i = 0; i < m_obstacles.size(); ++i) {
		Obstacle* obs = m_obstacles[i];
		if (obs && obs->m_isEnabled)
			removeObstacle(obs, false);

		delete obs;
	}
	m_obstacles.clear();
	m_obstacles.shrink_to_fit();
}

bool DynamicNavigationMesh::isObstacleInTile(Obstacle* obstacle, const std::array<int, 2>& tile) const {
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

const std::vector<CrowdAgent*>& DynamicNavigationMesh::getAgentsToReset() const {
	return m_agentsToReset;
}

std::vector<CrowdAgent*>& DynamicNavigationMesh::agentsToReset() {
	return m_agentsToReset;
}

void DynamicNavigationMesh::setCrowdManager(CrowdManager* crowdManager) {
	m_crowdManager = crowdManager;
}

void DynamicNavigationMesh::initObstacles() {
	m_tileCache->initObstacles();
}