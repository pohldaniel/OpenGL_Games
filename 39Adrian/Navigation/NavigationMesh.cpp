#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>

#include "../engine/scene/ShapeNode.h"
#include "../engine/MeshObject/Shape.h"
#include "../engine/DebugRenderer.h"

#include "NavigationMesh.h"
#include "OffMeshConnection.h"
#include "NavArea.h"
#include "NavBuildData.h"

const char* NAVIGATION_CATEGORY = "Navigation";

static const int DEFAULT_TILE_SIZE = 128;
static const float DEFAULT_CELL_SIZE = 0.3f;
static const float DEFAULT_CELL_HEIGHT = 0.2f;
static const float DEFAULT_AGENT_HEIGHT = 2.0f;
static const float DEFAULT_AGENT_RADIUS = 0.6f;
static const float DEFAULT_AGENT_MAX_CLIMB = 0.9f;
static const float DEFAULT_AGENT_MAX_SLOPE = 45.0f;
static const float DEFAULT_REGION_MIN_SIZE = 8.0f;
static const float DEFAULT_REGION_MERGE_SIZE = 20.0f;
static const float DEFAULT_EDGE_MAX_LENGTH = 12.0f;
static const float DEFAULT_EDGE_MAX_ERROR = 1.3f;
static const float DEFAULT_DETAIL_SAMPLE_DISTANCE = 6.0f;
static const float DEFAULT_DETAIL_SAMPLE_MAX_ERROR = 1.0f;
static const float M_LARGE_VALUE = 100000000.0f;

static const int MAX_POLYS = 2048;
std::random_device NavigationMesh::RandomDevice;
std::mt19937 NavigationMesh::MersenTwist(RandomDevice());
std::uniform_real_distribution<float> NavigationMesh::Dist(0.3f, 1.0f);
float NavigationMesh::Scale = 5.0f;

/// Temporary data for finding a path.
struct FindPathData{
	dtPolyRef polys[MAX_POLYS];
	dtPolyRef pathPolys[MAX_POLYS];
	Vector3f pathPoints[MAX_POLYS];
	unsigned char pathFlags[MAX_POLYS];
};

NavigationMesh::NavigationMesh() :
	navMesh_(0),
	navMeshQuery_(0),
	queryFilter_(new dtQueryFilter()),
	pathData_(new FindPathData()),
	tileSize_(DEFAULT_TILE_SIZE),
	cellSize_(DEFAULT_CELL_SIZE),
	cellHeight_(DEFAULT_CELL_HEIGHT),
	agentHeight_(DEFAULT_AGENT_HEIGHT),
	agentRadius_(DEFAULT_AGENT_RADIUS),
	agentMaxClimb_(DEFAULT_AGENT_MAX_CLIMB),
	agentMaxSlope_(DEFAULT_AGENT_MAX_SLOPE),
	regionMinSize_(DEFAULT_REGION_MIN_SIZE),
	regionMergeSize_(DEFAULT_REGION_MERGE_SIZE),
	edgeMaxLength_(DEFAULT_EDGE_MAX_LENGTH),
	edgeMaxError_(DEFAULT_EDGE_MAX_ERROR),
	detailSampleDistance_(DEFAULT_DETAIL_SAMPLE_DISTANCE),
	detailSampleMaxError_(DEFAULT_DETAIL_SAMPLE_MAX_ERROR),
	padding_(Vector3f::ONE),
	numTilesX_(0),
	numTilesZ_(0),
	partitionType_(NAVMESH_PARTITION_WATERSHED),
	drawOffMeshConnections_(false),
	drawNavAreas_(false) {

}

NavigationMesh::~NavigationMesh() {
	releaseNavigationMesh();
}

void NavigationMesh::OnRenderDebug() {

	const Matrix4f& worldTransform = Matrix4f::IDENTITY;

	const dtNavMesh* navMesh = navMesh_;
	if (!navMesh_)
		return;

	for (int j = 0; j < navMesh->getMaxTiles(); ++j){
		const dtMeshTile* tile = navMesh->getTile(j);
		
		if (!tile->header)
			continue;
		
		for (int i = 0; i < tile->header->polyCount; ++i){
			dtPoly* poly = tile->polys + i;
			for (unsigned j = 0; j < poly->vertCount; ++j){				
				DebugRenderer::Get().AddLine(
					worldTransform * *reinterpret_cast<const Vector3f*>(&tile->verts[poly->verts[j] * 3]),
					worldTransform * *reinterpret_cast<const Vector3f*>(&tile->verts[poly->verts[(j + 1) % poly->vertCount] * 3]),
					Vector4f(1.0f, 1.0f, 0.0f, 1.0f)
				);
			}
		}
	}
}

bool NavigationMesh::allocate() {
	return allocate(boundingBox_, numTilesX_, numTilesZ_);
}

bool NavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int tilesX, unsigned int tilesZ) {
	Vector3f min = boundingBox.min;
	Vector3f max = boundingBox.max;
	releaseNavigationMesh();	
	boundingBox_.setMin(min);
	boundingBox_.setMax(max);
	numTilesX_ = tilesX;
	numTilesZ_ = tilesZ;

	float tileEdgeLength = (float)tileSize_ * cellSize_;
	// Calculate max number of polygons, 22 bits available to identify both tile & polygon within tile
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
	std::cout << "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;

	return true;
}

bool NavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int maxTiles) {
	releaseNavigationMesh();

	boundingBox_ = boundingBox;
	maxTiles = Math::NextPowerOfTwo(maxTiles);


	int gridW = 0, gridH = 0;
	float tileEdgeLength = (float)tileSize_ * cellSize_;
	rcCalcGridSize(&boundingBox_.min[0], &boundingBox_.max[0], cellSize_, &gridW, &gridH);
	numTilesX_ = (gridW + tileSize_ - 1) / tileSize_;
	numTilesZ_ = (gridH + tileSize_ - 1) / tileSize_;

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
	std::cout <<  "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;
	return true;
}

bool NavigationMesh::build() {
	releaseNavigationMesh();
	std::vector<NavigationGeometryInfo> geometryList;
	CollectGeometries(geometryList);

	if (geometryList.empty()) {
		std::cout << "Nothing to do" << std::endl;
		return true;
	}

	for (unsigned i = 0; i < geometryList.size(); ++i)
		boundingBox_.merge(geometryList[i].boundingBox);

	// Expand bounding box by padding
	boundingBox_.min -= padding_;
	boundingBox_.max += padding_;

	{		
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
		
		// Build each tile
		unsigned numTiles = BuildTiles(geometryList, std::array<int, 2>({0, 0}), std::array<int, 2>({ getNumTiles()[0], getNumTiles()[1] }));
		std::cout << "Built navigation mesh with " + std::to_string(maxTiles) + " tiles" << std::endl;
		return true;
	}

	return false;
}

Buffer& NavigationMesh::getTileData(Buffer& buffer, const std::array<int, 2>& tile) const {
	writeTile(buffer, tile[0], tile[1]);
	return buffer;
}

Buffer& NavigationMesh::getTileData(int x, int z) {
	writeTile(m_tileData[z * numTilesX_ + x], x, z);
	return m_tileData[z * numTilesX_ + x];
}

void NavigationMesh::writeTile(Buffer& dest, int x, int z) const{
	const dtNavMesh* navMesh = navMesh_;
	const dtMeshTile* tile = navMesh->getTileAt(x, z, 0);
	if (!tile)
		return;

	dest.resize(sizeof(int) + tile->dataSize);
	memcpy(dest.data, &tile->dataSize, sizeof(int));
	memcpy(dest.data + sizeof(int), tile->data, tile->dataSize);
}

bool NavigationMesh::readTile(const Buffer& source) {	
	int navDataSize;
	memcpy(&navDataSize, source.data, sizeof(int));

	unsigned char* navData = (unsigned char*)dtAlloc(navDataSize, DT_ALLOC_PERM);
	if (!navData){
		std::cout << "Could not allocate data for navigation mesh tile" << std::endl;
		return false;
	}

	memcpy(navData, source.data + sizeof(int), navDataSize);
	if (dtStatusFailed(navMesh_->addTile(navData, navDataSize, DT_TILE_FREE_DATA, 0, 0))){
		std::cout << "Failed to add navigation mesh tile" << std::endl;
		dtFree(navData);
		return false;
	}
	return true;
}

bool NavigationMesh::addTile(const Buffer& tileData) {
	return readTile(tileData);
}

bool NavigationMesh::addTile(int x, int z) {
	return addTile(m_tileData.at(z * numTilesX_ + x));
}

void NavigationMesh::saveToTileData() {
	clearTileData();
	for (int z = 0; z < numTilesZ_; ++z) {
		for (int x = 0; x < numTilesX_; ++x) {
			getTileData(x, z);
		}
	}
}

void NavigationMesh::addTiles() {
	for (int z = 0; z < numTilesZ_; ++z) {
		for (int x = 0; x < numTilesX_; ++x) {
			const std::array<int, 2> tileIdx = { x, z };
			addTile(m_tileData[z * numTilesX_ + x]);
		}
	}
}

void NavigationMesh::removeTile(const std::array<int, 2>& tile, unsigned int layersToRemove) {
	if (!navMesh_)
		return;

	for (unsigned int i = 0u; i < layersToRemove + 1u; i++) {
		const dtTileRef tileRef = navMesh_->getTileRefAt(tile[0], tile[1], i);
		if (!tileRef)
			continue;
		navMesh_->removeTile(tileRef, 0, 0);
	}
	return;
}

void NavigationMesh::removeAllTiles() {
	const dtNavMesh* navMesh = navMesh_;
	for (int i = 0; i < navMesh_->getMaxTiles(); ++i){
		const dtMeshTile* tile = navMesh->getTile(i);
		if (tile->header)
			navMesh_->removeTile(navMesh_->getTileRef(tile), 0, 0);
	}
}

void NavigationMesh::clearTileData() {
	m_tileData.clear();
}

void NavigationMesh::addNavigable(const Navigable& navigable) {
	m_navigables.push_back(navigable);
}

void NavigationMesh::addOffMeshConnection(const OffMeshConnection& offMeshConnection) {
	m_offMeshConnections.push_back(offMeshConnection);
}

void NavigationMesh::addNavArea(const NavArea& navArea) {
	m_navAreas.push_back(navArea);
}

bool NavigationMesh::hasTile(const std::array<int, 2>& tile) const {
	if (navMesh_) {
		return !!navMesh_->getTileAt(tile[0], tile[1], 0);
	}
	return false;
}

bool NavigationMesh::hasTileData(int x, int z) const {
	return m_tileData.find(z * numTilesX_ + x) != m_tileData.end();
}

bool NavigationMesh::buildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z) {
	// Remove previous tile (if any)
	navMesh_->removeTile(navMesh_->getTileRefAt(x, z, 0), 0, 0);

	BoundingBox tileBoundingBox = getTileBoudningBox(std::array<int, 2>({x, z}));	 
	m_boxes.push_back(tileBoundingBox);

	SimpleNavBuildData build;

	rcConfig cfg;
	memset(&cfg, 0, sizeof cfg);
	cfg.cs = cellSize_;
	cfg.ch = cellHeight_;
	cfg.walkableSlopeAngle = agentMaxSlope_;
	cfg.walkableHeight = Math::CeilToInt(agentHeight_ / cfg.ch);
	cfg.walkableClimb = Math::FloorToInt(agentMaxClimb_ / cfg.ch);
	cfg.walkableRadius = Math::CeilToInt(agentRadius_ / cfg.cs);
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
		return true; // Nothing to do

	build.heightField = rcAllocHeightfield();
	if (!build.heightField){
		std::cout << "Could not allocate heightfield" << std::endl;
		return false;
	}

	if (!rcCreateHeightfield(build.ctx, *build.heightField, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs, cfg.ch)){
		std::cout << "Could not create heightfield" << std::endl;
		return false;
	}

	unsigned numTriangles = build.indices.size() / 3;
	unsigned char* triAreas = new unsigned char[numTriangles];
	memset(triAreas, 0, numTriangles);

	rcMarkWalkableTriangles(build.ctx, cfg.walkableSlopeAngle, &build.vertices[0][0], build.vertices.size(), &build.indices[0], numTriangles, triAreas);
	rcRasterizeTriangles(build.ctx, &build.vertices[0][0], build.vertices.size(), &build.indices[0],triAreas, numTriangles, *build.heightField, cfg.walkableClimb);
	rcFilterLowHangingWalkableObstacles(build.ctx, cfg.walkableClimb, *build.heightField);

	rcFilterWalkableLowHeightSpans(build.ctx, cfg.walkableHeight, *build.heightField);
	rcFilterLedgeSpans(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField);

	build.compactHeightField = rcAllocCompactHeightfield();
	if (!build.compactHeightField){
		std::cout << "Could not allocate create compact heightfield" << std::endl;
		return false;
	}

	if (!rcBuildCompactHeightfield(build.ctx, cfg.walkableHeight, cfg.walkableClimb, *build.heightField, *build.compactHeightField)){
		std::cout << "Could not build compact heightfield" << std::endl;
		return false;
	}

	if (!rcErodeWalkableArea(build.ctx, cfg.walkableRadius, *build.compactHeightField)){
		std::cout << "Could not erode compact heightfield" << std::endl;
		return false;
	}

	// Mark area volumes
	for (unsigned i = 0; i < build.navAreas.size(); ++i)
		rcMarkBoxArea(build.ctx, &build.navAreas[i].bounds.min[0], &build.navAreas[i].bounds.max[0],
			build.navAreas[i].areaID, *build.compactHeightField);

	if (this->partitionType_ == NAVMESH_PARTITION_WATERSHED){
		if (!rcBuildDistanceField(build.ctx, *build.compactHeightField)){
			std::cout << "Could not build distance field" << std::endl;
			return false;
		}

		if (!rcBuildRegions(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea)){
			std::cout << "Could not build regions" << std::endl;
			return false;
		}
	}else{
		if (!rcBuildRegionsMonotone(build.ctx, *build.compactHeightField, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea)){
			std::cout << "Could not build monotone regions" << std::endl;
			return false;
		}
	}

	build.contourSet = rcAllocContourSet();
	if (!build.contourSet){
		std::cout << "Could not allocate contour set" << std::endl;
		return false;
	}

	if (!rcBuildContours(build.ctx, *build.compactHeightField, cfg.maxSimplificationError, cfg.maxEdgeLen, *build.contourSet)){
		std::cout << "Could not create contours" << std::endl;
		return false;
	}

	build.polyMesh = rcAllocPolyMesh();
	if (!build.polyMesh){
		std::cout << "Could not allocate poly mesh" << std::endl;
		return false;
	}

	if (!rcBuildPolyMesh(build.ctx, *build.contourSet, cfg.maxVertsPerPoly, *build.polyMesh)){
		std::cout << "Could not triangulate contours" << std::endl;
		return false;
	}

	build.polyMeshDetail = rcAllocPolyMeshDetail();
	if (!build.polyMeshDetail){
		std::cout << "Could not allocate detail mesh" << std::endl;
		return false;
	}

	if (!rcBuildPolyMeshDetail(build.ctx, *build.polyMesh, *build.compactHeightField, cfg.detailSampleDist, cfg.detailSampleMaxError, *build.polyMeshDetail)){
		std::cout << "Could not build detail mesh" << std::endl;
		return false;
	}

	// Set polygon flags
	/// \todo Assignment of flags from navigation areas?
	for (int i = 0; i < build.polyMesh->npolys; ++i){
		if (build.polyMesh->areas[i] != RC_NULL_AREA)
			build.polyMesh->flags[i] = 0x1;
	}

	unsigned char* navData = 0;
	int navDataSize = 0;

	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof params);
	params.verts = build.polyMesh->verts;
	params.vertCount = build.polyMesh->nverts;
	params.polys = build.polyMesh->polys;
	params.polyAreas = build.polyMesh->areas;
	params.polyFlags = build.polyMesh->flags;
	params.polyCount = build.polyMesh->npolys;
	params.nvp = build.polyMesh->nvp;
	params.detailMeshes = build.polyMeshDetail->meshes;
	params.detailVerts = build.polyMeshDetail->verts;
	params.detailVertsCount = build.polyMeshDetail->nverts;
	params.detailTris = build.polyMeshDetail->tris;
	params.detailTriCount = build.polyMeshDetail->ntris;
	params.walkableHeight = agentHeight_;
	params.walkableRadius = agentRadius_;
	params.walkableClimb = agentMaxClimb_;
	params.tileX = x;
	params.tileY = z;
	rcVcopy(params.bmin, build.polyMesh->bmin);
	rcVcopy(params.bmax, build.polyMesh->bmax);
	params.cs = cfg.cs;
	params.ch = cfg.ch;
	params.buildBvTree = true;

	// Add off-mesh connections if have them
	if (build.offMeshRadii.size()){
		params.offMeshConCount = build.offMeshRadii.size();
		params.offMeshConVerts = &build.offMeshVertices[0][0];
		params.offMeshConRad = &build.offMeshRadii[0];
		params.offMeshConFlags = &build.offMeshFlags[0];
		params.offMeshConAreas = &build.offMeshAreas[0];
		params.offMeshConDir = &build.offMeshDir[0];
	}

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize)){
		std::cout << "Could not build navigation mesh tile data" << std::endl;
		return false;
	}

	if (dtStatusFailed(navMesh_->addTile(navData, navDataSize, DT_TILE_FREE_DATA, 0, 0))){
		std::cout << "Failed to add navigation mesh tile" << std::endl;
		dtFree(navData);
		return false;
	}

	return true;
}

bool NavigationMesh::InitializeQuery(){
	if (!navMesh_)
		return false;

	if (navMeshQuery_)
		return true;

	navMeshQuery_ = dtAllocNavMeshQuery();
	if (!navMeshQuery_){
		std::cout << "Could not create navigation mesh query" << std::endl;
		return false;
	}

	if (dtStatusFailed(navMeshQuery_->init(navMesh_, MAX_POLYS))){
		std::cout << "Could not init navigation mesh query" << std::endl;
		return false;
	}

	return true;
}

void NavigationMesh::releaseNavigationMesh() {
	dtFreeNavMesh(navMesh_);
	navMesh_ = 0;

	dtFreeNavMeshQuery(navMeshQuery_);
	navMeshQuery_ = 0;

	numTilesX_ = 0;
	numTilesZ_ = 0;
	boundingBox_.reset();
}

unsigned int NavigationMesh::BuildTiles(std::vector<NavigationGeometryInfo>& geometryList, const std::array<int, 2>& from, const std::array<int, 2>& to){
	unsigned numTiles = 0;
	for (int z = from[1]; z < to[1]; ++z){
		for (int x = from[0]; x < to[0]; ++x){
			if (buildTile(geometryList, x, z))
				++numTiles;
		}
	}
	return numTiles;
}

void NavigationMesh::CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList){

	std::hash_set<SceneNodeLC*> processedNodes;
	for (unsigned i = 0; i < m_navigables.size(); ++i){
		CollectGeometries(geometryList, m_navigables[i].m_node, processedNodes, m_navigables[i].isRecursive());
	}

	// Get offmesh connections
	for (unsigned i = 0; i < m_offMeshConnections.size(); ++i){
		OffMeshConnection& connection = m_offMeshConnections[i];
		if (connection.m_isEnabled && connection.m_endPoint){
			NavigationGeometryInfo info;
			info.connection = &connection;
			info.boundingBox = BoundingBox(connection.m_node->getWorldPosition(), connection.m_radius);
			geometryList.push_back(info);
		}
	}

	// Get nav area volumes
	for (unsigned i = 0; i < m_navAreas.size(); ++i){
		NavArea& area = m_navAreas[i];
		if (true){
			NavigationGeometryInfo info;
			info.area = &area;
			info.boundingBox = area.getBoundingBox();
			geometryList.push_back(info);
		}
	}
}

void NavigationMesh::CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList, SceneNodeLC* node, std::hash_set<SceneNodeLC*>& processedNodes, bool recursive) {
	// Make sure nodes are not included twice
	if (processedNodes.find(node) != processedNodes.end()) {
		return;
	}
	
	processedNodes.insert(node);
	{
		ShapeNode* shapeNode = static_cast<ShapeNode*>(node);
		NavigationGeometryInfo info;
		info.lodLevel = 0;			
		info.component = shapeNode;
		info.transform =  node->getWorldTransformation();
		info.boundingBox = shapeNode->getWorldBoundingBox();
		geometryList.push_back(info);
	}

	if (recursive){
		const std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& children = node->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it) {
			if(dynamic_cast<ShapeNode*>((*it).get()))
				CollectGeometries(geometryList, static_cast<SceneNodeLC*>((*it).get()), processedNodes, recursive);
		}
	}
}

void NavigationMesh::GetTileGeometry(NavBuildData* build, std::vector<NavigationGeometryInfo>& geometryList, BoundingBox& box){

	Matrix4f inverse = Matrix4f::IDENTITY;
	unsigned int vertexCount = 0u;

	for (unsigned i = 0; i < geometryList.size(); ++i){
		if (box.isInsideFast(geometryList[i].boundingBox) != BoundingBox::Intersection::OUTSIDE){
			const Matrix4f& transform = geometryList[i].transform;

			if (geometryList[i].connection){
				OffMeshConnection* connection = geometryList[i].connection;
				Vector3f start = inverse * connection->m_node->getWorldPosition();
				Vector3f end = inverse * connection->m_endPoint->getWorldPosition();

				build->offMeshVertices.push_back(start);
				build->offMeshVertices.push_back(end);
				build->offMeshRadii.push_back(connection->m_radius);
				build->offMeshFlags.push_back((unsigned short)connection->m_mask);
				build->offMeshAreas.push_back((unsigned char)connection->m_areaId);
				build->offMeshDir.push_back((unsigned char)(connection->m_bidirectional ? DT_OFFMESH_CON_BIDIR : 0));

			}else if(geometryList[i].area) {
				NavArea* area = geometryList[i].area;
				NavAreaStub stub;
				stub.areaID = (unsigned char)area->m_areaID;
				stub.bounds = area->getBoundingBox();
				build->navAreas.push_back(stub);
			}else if (geometryList[i].component) {
				ShapeNode* drawable = geometryList[i].component;
				if (drawable) {
					AddTriMeshGeometry(build, drawable->getShape(), transform, vertexCount);
				}
			}
		}
	}
}

void NavigationMesh::AddTriMeshGeometry(NavBuildData* build, const Shape& shape, const Matrix4f& transform, unsigned int& vertexCount) {
	unsigned srcIndexCount = shape.getIndexBuffer().size();
	unsigned srcVertexCount = shape.getPositions().size();

	for (unsigned k = 0u; k < srcVertexCount; ++k){
		build->vertices.push_back(transform ^ shape.getPositions()[k]);
	}

	for (unsigned k = 0u; k < srcIndexCount; ++k) {
		build->indices.push_back(shape.getIndexBuffer()[k] + vertexCount);
	}

	vertexCount += srcVertexCount;
}

Vector3f NavigationMesh::moveAlongSurface(const Vector3f& start, const Vector3f& end, const Vector3f& extents, int maxVisited, const dtQueryFilter* filter) {
	if (!InitializeQuery())
		return end;

	Vector3f localStart = start;
	Vector3f localEnd = end;

	const dtQueryFilter* queryFilter = filter ? filter : queryFilter_;
	dtPolyRef startRef;
	navMeshQuery_->findNearestPoly(&localStart[0], extents.getVec(), queryFilter, &startRef, 0);
	if (!startRef)
		return end;

	Vector3f resultPos;
	int visitedCount = 0;
	maxVisited = std::max(maxVisited, 0);
	std::vector<dtPolyRef> visited((unsigned)maxVisited);
	navMeshQuery_->moveAlongSurface(startRef, &localStart[0], &localEnd[0], queryFilter, &resultPos[0], maxVisited ?
		&visited[0] : (dtPolyRef*)0, &visitedCount, maxVisited);
	return resultPos;
}

Vector3f NavigationMesh::findNearestPoint(const Vector3f& point, const Vector3f& extents, const dtQueryFilter* filter,dtPolyRef* nearestRef){
	if (!InitializeQuery())
		return point;

	Vector3f localPoint = point;
	Vector3f nearestPoint;

	dtPolyRef pointRef;
	if (!nearestRef)
		nearestRef = &pointRef;
	navMeshQuery_->findNearestPoly(localPoint.getVec(), extents.getVec(), filter ? filter : queryFilter_, nearestRef, &nearestPoint[0]);

	return *nearestRef ? nearestPoint : point;
}

void NavigationMesh::findPath(std::vector<Vector3f>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents, const dtQueryFilter* filter) {
	std::vector<NavigationPathPoint> navPathPoints;
	findPath(navPathPoints, start, end, extents, filter);

	dest.clear();
	for (unsigned i = 0; i < navPathPoints.size(); ++i)
		dest.push_back(navPathPoints[i].m_position);
}

void NavigationMesh::findPath(std::vector<NavigationPathPoint>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents, const dtQueryFilter* filter) {
	dest.clear();

	if (!InitializeQuery())
		return;

	Vector3f localStart = start;
	Vector3f localEnd = end;

	const dtQueryFilter* queryFilter = filter ? filter : queryFilter_;
	dtPolyRef startRef;
	dtPolyRef endRef;
	navMeshQuery_->findNearestPoly(&localStart[0], extents.getVec(), queryFilter, &startRef, 0);
	navMeshQuery_->findNearestPoly(&localEnd[0], extents.getVec(), queryFilter, &endRef, 0);

	if (!startRef || !endRef)
		return;

	int numPolys = 0;
	int numPathPoints = 0;

	navMeshQuery_->findPath(startRef, endRef, &localStart[0], &localEnd[0], queryFilter, pathData_->polys, &numPolys, MAX_POLYS);
	if (!numPolys)
		return;

	Vector3f actualLocalEnd = localEnd;

	// If full path was not found, clamp end point to the end polygon
	if (pathData_->polys[numPolys - 1] != endRef)
		navMeshQuery_->closestPointOnPoly(pathData_->polys[numPolys - 1], &localEnd[0], &actualLocalEnd[0], 0);

	navMeshQuery_->findStraightPath(&localStart[0], &actualLocalEnd[0], pathData_->polys, numPolys,
		&pathData_->pathPoints[0][0], pathData_->pathFlags, pathData_->pathPolys, &numPathPoints, MAX_POLYS);

	// Transform path result back to world space
	for (int i = 0; i < numPathPoints; ++i) {
		NavigationPathPoint pt;
		pt.m_position = pathData_->pathPoints[i];
		pt.m_flag = (NavigationPathPointFlag)pathData_->pathFlags[i];

		// Walk through all NavAreas and find nearest
		unsigned nearestNavAreaID = 0;       // 0 is the default nav area ID
		float nearestDistance = M_LARGE_VALUE;
		for (unsigned j = 0; j < m_navAreas.size(); j++) {
			const NavArea& area = m_navAreas[j];
			if (area.m_isEnabled) {
				BoundingBox bb = area.getBoundingBox();
				if (bb.isInside(pt.m_position) == BoundingBox::Intersection::INSIDE) {
					Vector3f areaWorldCenter = bb.getCenter();
					float distance = (areaWorldCenter - pt.m_position).lengthSq();
					if (distance < nearestDistance) {
						nearestDistance = distance;
						nearestNavAreaID = area.m_areaID;
					}
				}
			}
		}
		pt.m_areaID = (unsigned char)nearestNavAreaID;

		dest.push_back(pt);
	}
}

Vector3f NavigationMesh::raycast(const Vector3f& start, const Vector3f& end, const Vector3f& extents, const dtQueryFilter* filter, Vector3f* hitNormal) {
	if (hitNormal)
		*hitNormal = Vector3f::DOWN;

	if (!InitializeQuery())
		return end;

	Vector3f localStart = start;
	Vector3f localEnd = end;

	const dtQueryFilter* queryFilter = filter ? filter : queryFilter_;
	dtPolyRef startRef;
	navMeshQuery_->findNearestPoly(&localStart[0], extents.getVec(), queryFilter, &startRef, 0);

	if (!startRef)
		return end;

	Vector3f normal;
	if (!hitNormal)
		hitNormal = &normal;
	float t;
	int numPolys;

	navMeshQuery_->raycast(startRef, &localStart[0], &localEnd[0], queryFilter, &t, &hitNormal->getVec()[0], pathData_->polys, &numPolys,
		MAX_POLYS);

	if (t == FLT_MAX)
		t = 1.0f;

	return Math::Lerp(start, end, t);
}

Vector3f NavigationMesh::randPoint(const Vector3f& center, float radius) {
	const float length = sqrt(Random()) * radius;
	const float degree = Random() * TWO_PI;
	const float x = center[0] + length * cos(degree);
	const float z = center[2] + length * sin(degree);
	return { x, center[1], z };
}

bool NavigationMesh::isInitialized() const {
	return navMesh_ != 0; 
}

void NavigationMesh::setPartitionType(NavmeshPartitionType ptype){
	partitionType_ = ptype;
}

void NavigationMesh::setPadding(const Vector3f& padding) {
	padding_ = padding;
}

void NavigationMesh::setAreaCost(unsigned int areaID, float cost) {
	if (queryFilter_)
		queryFilter_->setAreaCost((int)areaID, cost);
}

void NavigationMesh::setTileSize(int size) {
	tileSize_ = std::max(size, 1);
}

void NavigationMesh::setAgentHeight(float height) {
	agentHeight_ = std::max(height, EPSILON);
}

void NavigationMesh::setCellHeight(float height) {
	cellHeight_ = std::max(height, EPSILON);
}

void NavigationMesh::setAgentMaxClimb(float maxClimb) {
	agentMaxClimb_ = std::max(maxClimb, EPSILON);
}

void NavigationMesh::setAgentMaxSlope(float maxSlope) {
	agentMaxSlope_ = std::max(maxSlope, 0.0f);
}

void NavigationMesh::setAgentRadius(float radius) {
	agentRadius_ = std::max(radius, EPSILON);
}

void NavigationMesh::setCellSize(float size) {
	cellSize_ = std::max(size, EPSILON);
}

int NavigationMesh::setPolyFlag(const Vector3f& point, unsigned short polyMask, const Vector3f& extents, const dtQueryFilter* filter) {
	if (!InitializeQuery())
		return 0;

	Vector3f localPoint = point;

	const dtQueryFilter* queryFilter = filter ? filter : queryFilter_;
	dtPolyRef polyRef[16];
	int polyCnt = 0;

	if (navMeshQuery_->queryPolygons(&localPoint[0], extents.getVec(), queryFilter, polyRef, &polyCnt, 16) == DT_SUCCESS) {
		for (int i = 0; i < polyCnt; ++i) {
			navMesh_->setPolyFlags(polyRef[i], polyMask);
		}
	}

	return polyCnt;
}

void NavigationMesh::setDrawOffMeshConnections(bool enable) { 
	drawOffMeshConnections_ = enable; 
}

void NavigationMesh::setDrawNavAreas(bool enable) { 
	drawNavAreas_ = enable; 
}

float NavigationMesh::getAreaCost(unsigned areaID) const {
	if (queryFilter_)
		return queryFilter_->getAreaCost((int)areaID);
	return 1.0f;
}

BoundingBox NavigationMesh::getTileBoudningBox(const std::array<int, 2>& tile) const {
	const float tileEdgeLength = (float)tileSize_ * cellSize_;
	return BoundingBox(
		Vector3f(
			boundingBox_.min[0] + tileEdgeLength * (float)tile[0],
			boundingBox_.min[1],
			boundingBox_.min[2] + tileEdgeLength * (float)tile[1]
		),
		Vector3f(
			boundingBox_.min[0] + tileEdgeLength * (float)(tile[0] + 1),
			boundingBox_.max[1],
			boundingBox_.min[2] + tileEdgeLength * (float)(tile[1] + 1)
		));
}

Vector3f NavigationMesh::getRandomPoint(const dtQueryFilter* filter, dtPolyRef* randomRef) {
	if (!InitializeQuery())
		return Vector3f::ZERO;

	dtPolyRef polyRef;
	Vector3f point(Vector3f::ZERO);
	navMeshQuery_->findRandomPoint(filter ? filter : queryFilter_, Random, randomRef ? randomRef : &polyRef, &point[0]);
	return point;
}

Vector3f NavigationMesh::getRandomPointInCircle(const Vector3f& center, float radius, const Vector3f& extents, const dtQueryFilter* filter, dtPolyRef* randomRef) {
	if (randomRef)
		*randomRef = 0;

	if (!InitializeQuery())
		return center;

	Vector3f localCenter = center;
	const dtQueryFilter* queryFilter = filter ? filter : queryFilter_;
	dtPolyRef startRef;
	navMeshQuery_->findNearestPoly(&localCenter[0], extents.getVec(), queryFilter, &startRef, 0);

	if (!startRef)
		return center;

	dtPolyRef polyRef;
	if (!randomRef)
		randomRef = &polyRef;
	Vector3f point(localCenter);

	//navMeshQuery_->findRandomPointAroundCircle(startRef, &localCenter[0], radius, queryFilter, Random, randomRef, &point[0]);
	return randPoint(point, radius);
}

float NavigationMesh::getDistanceToWall(const Vector3f& point, float radius, const Vector3f& extents, const dtQueryFilter* filter, Vector3f* hitPos, Vector3f* hitNormal) {
	if (hitPos)
		*hitPos = Vector3f::ZERO;
	if (hitNormal)
		*hitNormal = Vector3f::DOWN;

	if (!InitializeQuery())
		return radius;

	Vector3f localPoint = point;

	const dtQueryFilter* queryFilter = filter ? filter : queryFilter_;
	dtPolyRef startRef;
	navMeshQuery_->findNearestPoly(&localPoint[0], extents.getVec(), queryFilter, &startRef, 0);
	if (!startRef)
		return radius;

	float hitDist = radius;
	Vector3f pos;
	if (!hitPos)
		hitPos = &pos;
	Vector3f normal;
	if (!hitNormal)
		hitNormal = &normal;

	navMeshQuery_->findDistanceToWall(startRef, &localPoint[0], radius, queryFilter, &hitDist, &hitPos->getVec()[0], &hitNormal->getVec()[0]);
	return hitDist;
}

std::array<int, 2> NavigationMesh::getTileIndex(const Vector3f& position) const {
	const float tileEdgeLength = (float)tileSize_ * cellSize_;
	const Vector3f localPosition = position - boundingBox_.min;
	const Vector2f localPosition2D(localPosition[0], localPosition[2]);
	const std::array<int, 2> max = { std::max(0, static_cast<int>(floor(localPosition2D[0] / tileEdgeLength))), std::max(0, static_cast<int>(floor(localPosition2D[1] / tileEdgeLength))) };
	return { std::min(max[0],  getNumTiles()[0] - 1), std::min(max[1],  getNumTiles()[1] - 1) };
}

bool NavigationMesh::getDrawOffMeshConnections() const { 
	return drawOffMeshConnections_; 
}

int NavigationMesh::getTileSize() const { 
	return tileSize_; 
}

float NavigationMesh::getCellSize() const { 
	return cellSize_; 
}

dtNavMesh* NavigationMesh::getDetourNavMesh() { 
	return navMesh_; 
}

float NavigationMesh::getCellHeight() const { 
	return cellHeight_; 
}

float NavigationMesh::getAgentHeight() const { 
	return agentHeight_; 
}

float NavigationMesh::getAgentRadius() const { 
	return agentRadius_; 
}

float NavigationMesh::getAgentMaxClimb() const { 
	return agentMaxClimb_; 
}

float NavigationMesh::getAgentMaxSlope() const { 
	return agentMaxSlope_; 
}

float NavigationMesh::getRegionMinSize() const { 
	return regionMinSize_; 
}

float NavigationMesh::getRegionMergeSize() const { 
	return regionMergeSize_; 
}

float NavigationMesh::getEdgeMaxLength() const { 
	return edgeMaxLength_; 
}

float NavigationMesh::getEdgeMaxError() const { 
	return edgeMaxError_; 
}

float NavigationMesh::getDetailSampleDistance() const { 
	return detailSampleDistance_; 
}

float NavigationMesh::getDetailSampleMaxError() const { 
	return detailSampleMaxError_; 
}

const Vector3f& NavigationMesh::getPadding() const { 
	return padding_; 
}

const BoundingBox& NavigationMesh::getBoundingBox() const { 
	return boundingBox_; 
}

BoundingBox& NavigationMesh::boundingBox() {
	return boundingBox_;
}

std::array<int, 2> NavigationMesh::getNumTiles() const { 
	return std::array<int, 2>({ numTilesX_, numTilesZ_ }); 
}

const int NavigationMesh::getNumTilesX() const {
	return numTilesX_;
}

const int NavigationMesh::getNumTilesZ() const {
	return numTilesZ_;
}

int& NavigationMesh::numTilesX() {
	return numTilesX_;
}

int& NavigationMesh::numTilesZ() {
	return numTilesZ_;
}

NavmeshPartitionType NavigationMesh::getPartitionType() const { 
	return partitionType_; 
}

bool NavigationMesh::getDrawNavAreas() const { 
	return drawNavAreas_; 
}

std::unordered_map<int, Buffer>& NavigationMesh::tileData() {
	return m_tileData;
}

const std::vector<NavArea>& NavigationMesh::getNavAreas() const {
	return m_navAreas;
}

std::vector<NavArea>& NavigationMesh::navAreas() {
	return m_navAreas;
}

float NavigationMesh::Random() {
	return Dist(MersenTwist) * Scale;
}