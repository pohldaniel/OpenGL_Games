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
	m_navMesh(0),
	m_navMeshQuery(0),
	m_queryFilter(new dtQueryFilter()),
	m_pathData(new FindPathData()),
	m_tileSize(DEFAULT_TILE_SIZE),
	m_cellSize(DEFAULT_CELL_SIZE),
	m_cellHeight(DEFAULT_CELL_HEIGHT),
	m_agentHeight(DEFAULT_AGENT_HEIGHT),
	m_agentRadius(DEFAULT_AGENT_RADIUS),
	m_agentMaxClimb(DEFAULT_AGENT_MAX_CLIMB),
	m_agentMaxSlope(DEFAULT_AGENT_MAX_SLOPE),
	m_regionMinSize(DEFAULT_REGION_MIN_SIZE),
	m_regionMergeSize(DEFAULT_REGION_MERGE_SIZE),
	m_edgeMaxLength(DEFAULT_EDGE_MAX_LENGTH),
	m_edgeMaxError(DEFAULT_EDGE_MAX_ERROR),
	m_detailSampleDistance(DEFAULT_DETAIL_SAMPLE_DISTANCE),
	m_detailSampleMaxError(DEFAULT_DETAIL_SAMPLE_MAX_ERROR),
	m_padding(Vector3f::ONE),
	m_numTilesX(0),
	m_numTilesZ(0),
	m_partitionType(NAVMESH_PARTITION_WATERSHED),
	m_drawOffMeshConnections(false),
	m_drawNavAreas(false) {

}

NavigationMesh::~NavigationMesh() {
	releaseNavigationMesh();
}

void NavigationMesh::OnRenderDebug() {

	if (!m_navMesh)
		return;

	const dtNavMesh* navMesh = m_navMesh;
	const Matrix4f& worldTransform = Matrix4f::IDENTITY;

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
	return allocate(m_boundingBox, m_numTilesX, m_numTilesZ);
}

bool NavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int tilesX, unsigned int tilesZ) {
	Vector3f min = boundingBox.min;
	Vector3f max = boundingBox.max;
	releaseNavigationMesh();	
	m_boundingBox.setMin(min);
	m_boundingBox.setMax(max);
	m_numTilesX = tilesX;
	m_numTilesZ = tilesZ;

	float tileEdgeLength = (float)m_tileSize * m_cellSize;
	// Calculate max number of polygons, 22 bits available to identify both tile & polygon within tile
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
	std::cout << "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;

	return true;
}

bool NavigationMesh::allocate(const BoundingBox& boundingBox, unsigned int maxTiles) {
	releaseNavigationMesh();

	m_boundingBox = boundingBox;
	maxTiles = Math::NextPowerOfTwo(maxTiles);


	int gridW = 0, gridH = 0;
	float tileEdgeLength = (float)m_tileSize * m_cellSize;
	rcCalcGridSize(&m_boundingBox.min[0], &m_boundingBox.max[0], m_cellSize, &gridW, &gridH);
	m_numTilesX = (gridW + m_tileSize - 1) / m_tileSize;
	m_numTilesZ = (gridH + m_tileSize - 1) / m_tileSize;

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
	std::cout <<  "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;
	return true;
}

bool NavigationMesh::build() {
	releaseNavigationMesh();
	std::vector<NavigationGeometryInfo> geometryList;
	collectGeometries(geometryList);

	if (geometryList.empty()) {
		std::cout << "Nothing to do" << std::endl;
		return true;
	}

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
		
		// Build each tile
		unsigned numTiles = buildTiles(geometryList, std::array<int, 2>({0, 0}), getNumTiles());
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
	writeTile(m_tileData[z * m_numTilesX + x], x, z);
	return m_tileData[z * m_numTilesX + x];
}

void NavigationMesh::writeTile(Buffer& dest, int x, int z) const{
	const dtNavMesh* navMesh = m_navMesh;
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
	if (dtStatusFailed(m_navMesh->addTile(navData, navDataSize, DT_TILE_FREE_DATA, 0, 0))){
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
	return addTile(m_tileData.at(z * m_numTilesX + x));
}

void NavigationMesh::saveToTileData() {
	clearTileData();
	for (int z = 0; z < m_numTilesZ; ++z) {
		for (int x = 0; x < m_numTilesX; ++x) {
			getTileData(x, z);
		}
	}
}

void NavigationMesh::addTiles() {
	for (int z = 0; z < m_numTilesZ; ++z) {
		for (int x = 0; x < m_numTilesX; ++x) {
			addTile(m_tileData[z * m_numTilesX + x]);
		}
	}
}

void NavigationMesh::removeTile(const std::array<int, 2>& tile, unsigned int layersToRemove) {
	if (!m_navMesh)
		return;

	for (unsigned int i = 0u; i < layersToRemove + 1u; i++) {
		const dtTileRef tileRef = m_navMesh->getTileRefAt(tile[0], tile[1], i);
		if (!tileRef)
			continue;
		m_navMesh->removeTile(tileRef, 0, 0);
	}
	return;
}

void NavigationMesh::removeAllTiles() {
	const dtNavMesh* navMesh = m_navMesh;
	for (int i = 0; i < m_navMesh->getMaxTiles(); ++i){
		const dtMeshTile* tile = navMesh->getTile(i);
		if (tile->header)
			m_navMesh->removeTile(m_navMesh->getTileRef(tile), 0, 0);
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
	if (m_navMesh) {
		return m_navMesh->getTileAt(tile[0], tile[1], 0);
	}
	return false;
}

bool NavigationMesh::hasTileData(int x, int z) const {
	return m_tileData.find(z * m_numTilesX + x) != m_tileData.end();
}

bool NavigationMesh::buildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z) {
	// Remove previous tile (if any)
	m_navMesh->removeTile(m_navMesh->getTileRefAt(x, z, 0), 0, 0);

	BoundingBox tileBoundingBox = getTileBoudningBox(std::array<int, 2>({x, z}));	 
	m_boxes.push_back(tileBoundingBox);

	SimpleNavBuildData build;

	rcConfig cfg;
	memset(&cfg, 0, sizeof cfg);
	cfg.cs = m_cellSize;
	cfg.ch = m_cellHeight;
	cfg.walkableSlopeAngle = m_agentMaxSlope;
	cfg.walkableHeight = Math::CeilToInt(m_agentHeight / cfg.ch);
	cfg.walkableClimb = Math::FloorToInt(m_agentMaxClimb / cfg.ch);
	cfg.walkableRadius = Math::CeilToInt(m_agentRadius / cfg.cs);
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

	if (this->m_partitionType == NAVMESH_PARTITION_WATERSHED){
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
	params.walkableHeight = m_agentHeight;
	params.walkableRadius = m_agentRadius;
	params.walkableClimb = m_agentMaxClimb;
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

	if (dtStatusFailed(m_navMesh->addTile(navData, navDataSize, DT_TILE_FREE_DATA, 0, 0))){
		std::cout << "Failed to add navigation mesh tile" << std::endl;
		dtFree(navData);
		return false;
	}

	return true;
}

bool NavigationMesh::initializeQuery(){
	if (!m_navMesh)
		return false;

	if (m_navMeshQuery)
		return true;

	m_navMeshQuery = dtAllocNavMeshQuery();
	if (!m_navMeshQuery){
		std::cout << "Could not create navigation mesh query" << std::endl;
		return false;
	}

	if (dtStatusFailed(m_navMeshQuery->init(m_navMesh, MAX_POLYS))){
		std::cout << "Could not init navigation mesh query" << std::endl;
		return false;
	}

	return true;
}

void NavigationMesh::releaseNavigationMesh() {
	dtFreeNavMesh(m_navMesh);
	m_navMesh = nullptr;

	dtFreeNavMeshQuery(m_navMeshQuery);
	m_navMeshQuery = nullptr;

	m_numTilesX = 0;
	m_numTilesZ = 0;
	m_boundingBox.reset();
}

unsigned int NavigationMesh::buildTiles(std::vector<NavigationGeometryInfo>& geometryList, const std::array<int, 2>& from, const std::array<int, 2>& to){
	unsigned numTiles = 0;
	for (int z = from[1]; z < to[1]; ++z){
		for (int x = from[0]; x < to[0]; ++x){
			if (buildTile(geometryList, x, z))
				++numTiles;
		}
	}
	return numTiles;
}

void NavigationMesh::collectGeometries(std::vector<NavigationGeometryInfo>& geometryList){

	std::hash_set<SceneNodeLC*> processedNodes;
	for (unsigned i = 0; i < m_navigables.size(); ++i){
		collectGeometries(geometryList, m_navigables[i].m_node, processedNodes, m_navigables[i].isRecursive());
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

void NavigationMesh::collectGeometries(std::vector<NavigationGeometryInfo>& geometryList, SceneNodeLC* node, std::hash_set<SceneNodeLC*>& processedNodes, bool recursive) {
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
				collectGeometries(geometryList, static_cast<SceneNodeLC*>((*it).get()), processedNodes, recursive);
		}
	}
}

void NavigationMesh::getTileGeometry(NavBuildData* build, std::vector<NavigationGeometryInfo>& geometryList, BoundingBox& box){

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
					addTriMeshGeometry(build, drawable->getShape(), transform, vertexCount);
				}
			}
		}
	}
}

void NavigationMesh::addTriMeshGeometry(NavBuildData* build, const Shape& shape, const Matrix4f& transform, unsigned int& vertexCount) {
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
	if (!initializeQuery())
		return end;

	Vector3f localStart = start;
	Vector3f localEnd = end;

	const dtQueryFilter* queryFilter = filter ? filter : m_queryFilter;
	dtPolyRef startRef;
	m_navMeshQuery->findNearestPoly(&localStart[0], extents.getVec(), queryFilter, &startRef, 0);
	if (!startRef)
		return end;

	Vector3f resultPos;
	int visitedCount = 0;
	maxVisited = std::max(maxVisited, 0);
	std::vector<dtPolyRef> visited((unsigned)maxVisited);
	m_navMeshQuery->moveAlongSurface(startRef, &localStart[0], &localEnd[0], queryFilter, &resultPos[0], maxVisited ?
		&visited[0] : (dtPolyRef*)0, &visitedCount, maxVisited);
	return resultPos;
}

Vector3f NavigationMesh::findNearestPoint(const Vector3f& point, const Vector3f& extents, const dtQueryFilter* filter,dtPolyRef* nearestRef){
	if (!initializeQuery())
		return point;

	Vector3f localPoint = point;
	Vector3f nearestPoint;

	dtPolyRef pointRef;
	if (!nearestRef)
		nearestRef = &pointRef;
	m_navMeshQuery->findNearestPoly(localPoint.getVec(), extents.getVec(), filter ? filter : m_queryFilter, nearestRef, &nearestPoint[0]);

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

	if (!initializeQuery())
		return;

	Vector3f localStart = start;
	Vector3f localEnd = end;

	const dtQueryFilter* queryFilter = filter ? filter : m_queryFilter;
	dtPolyRef startRef;
	dtPolyRef endRef;
	m_navMeshQuery->findNearestPoly(&localStart[0], extents.getVec(), queryFilter, &startRef, 0);
	m_navMeshQuery->findNearestPoly(&localEnd[0], extents.getVec(), queryFilter, &endRef, 0);

	if (!startRef || !endRef)
		return;

	int numPolys = 0;
	int numPathPoints = 0;

	m_navMeshQuery->findPath(startRef, endRef, &localStart[0], &localEnd[0], queryFilter, m_pathData->polys, &numPolys, MAX_POLYS);
	if (!numPolys)
		return;

	Vector3f actualLocalEnd = localEnd;

	// If full path was not found, clamp end point to the end polygon
	if (m_pathData->polys[numPolys - 1] != endRef)
		m_navMeshQuery->closestPointOnPoly(m_pathData->polys[numPolys - 1], &localEnd[0], &actualLocalEnd[0], 0);

	m_navMeshQuery->findStraightPath(&localStart[0], &actualLocalEnd[0], m_pathData->polys, numPolys,
		&m_pathData->pathPoints[0][0], m_pathData->pathFlags, m_pathData->pathPolys, &numPathPoints, MAX_POLYS);

	// Transform path result back to world space
	for (int i = 0; i < numPathPoints; ++i) {
		NavigationPathPoint pt;
		pt.m_position = m_pathData->pathPoints[i];
		pt.m_flag = (NavigationPathPointFlag)m_pathData->pathFlags[i];

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

	if (!initializeQuery())
		return end;

	Vector3f localStart = start;
	Vector3f localEnd = end;

	const dtQueryFilter* queryFilter = filter ? filter : m_queryFilter;
	dtPolyRef startRef;
	m_navMeshQuery->findNearestPoly(&localStart[0], extents.getVec(), queryFilter, &startRef, 0);

	if (!startRef)
		return end;

	Vector3f normal;
	if (!hitNormal)
		hitNormal = &normal;
	float t;
	int numPolys;

	m_navMeshQuery->raycast(startRef, &localStart[0], &localEnd[0], queryFilter, &t, &hitNormal->getVec()[0], m_pathData->polys, &numPolys, MAX_POLYS);

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
	return m_navMesh != 0;
}

void NavigationMesh::setPartitionType(NavmeshPartitionType ptype){
	m_partitionType = ptype;
}

void NavigationMesh::setPadding(const Vector3f& padding) {
	m_padding = padding;
}

void NavigationMesh::setAreaCost(unsigned int areaID, float cost) {
	if (m_queryFilter)
		m_queryFilter->setAreaCost((int)areaID, cost);
}

void NavigationMesh::setTileSize(int size) {
	m_tileSize = std::max(size, 1);
}

void NavigationMesh::setAgentHeight(float height) {
	m_agentHeight = std::max(height, EPSILON);
}

void NavigationMesh::setCellHeight(float height) {
	m_cellHeight = std::max(height, EPSILON);
}

void NavigationMesh::setAgentMaxClimb(float maxClimb) {
	m_agentMaxClimb = std::max(maxClimb, EPSILON);
}

void NavigationMesh::setAgentMaxSlope(float maxSlope) {
	m_agentMaxSlope = std::max(maxSlope, 0.0f);
}

void NavigationMesh::setAgentRadius(float radius) {
	m_agentRadius = std::max(radius, EPSILON);
}

void NavigationMesh::setCellSize(float size) {
	m_cellSize = std::max(size, EPSILON);
}

int NavigationMesh::setPolyFlag(const Vector3f& point, unsigned short polyMask, const Vector3f& extents, const dtQueryFilter* filter) {
	if (!initializeQuery())
		return 0;

	Vector3f localPoint = point;

	const dtQueryFilter* queryFilter = filter ? filter : m_queryFilter;
	dtPolyRef polyRef[16];
	int polyCnt = 0;

	if (m_navMeshQuery->queryPolygons(&localPoint[0], extents.getVec(), queryFilter, polyRef, &polyCnt, 16) == DT_SUCCESS) {
		for (int i = 0; i < polyCnt; ++i) {
			m_navMesh->setPolyFlags(polyRef[i], polyMask);
		}
	}

	return polyCnt;
}

void NavigationMesh::setDrawOffMeshConnections(bool enable) { 
	m_drawOffMeshConnections = enable;
}

void NavigationMesh::setDrawNavAreas(bool enable) { 
	m_drawNavAreas = enable;
}

float NavigationMesh::getAreaCost(unsigned areaID) const {
	if (m_queryFilter)
		return m_queryFilter->getAreaCost((int)areaID);
	return 1.0f;
}

BoundingBox NavigationMesh::getTileBoudningBox(const std::array<int, 2>& tile) const {
	const float tileEdgeLength = (float)m_tileSize * m_cellSize;
	return BoundingBox(
		Vector3f(
			m_boundingBox.min[0] + tileEdgeLength * (float)tile[0],
			m_boundingBox.min[1],
			m_boundingBox.min[2] + tileEdgeLength * (float)tile[1]
		),
		Vector3f(
			m_boundingBox.min[0] + tileEdgeLength * (float)(tile[0] + 1),
			m_boundingBox.max[1],
			m_boundingBox.min[2] + tileEdgeLength * (float)(tile[1] + 1)
		));
}

Vector3f NavigationMesh::getRandomPoint(const dtQueryFilter* filter, dtPolyRef* randomRef) {
	if (!initializeQuery())
		return Vector3f::ZERO;

	dtPolyRef polyRef;
	Vector3f point(Vector3f::ZERO);
	m_navMeshQuery->findRandomPoint(filter ? filter : m_queryFilter, Random, randomRef ? randomRef : &polyRef, &point[0]);
	return point;
}

Vector3f NavigationMesh::getRandomPointInCircle(const Vector3f& center, float radius, const Vector3f& extents, const dtQueryFilter* filter, dtPolyRef* randomRef) {
	if (randomRef)
		*randomRef = 0;

	if (!initializeQuery())
		return center;

	Vector3f localCenter = center;
	const dtQueryFilter* queryFilter = filter ? filter : m_queryFilter;
	dtPolyRef startRef;
	m_navMeshQuery->findNearestPoly(&localCenter[0], extents.getVec(), queryFilter, &startRef, 0);

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

	if (!initializeQuery())
		return radius;

	Vector3f localPoint = point;

	const dtQueryFilter* queryFilter = filter ? filter : m_queryFilter;
	dtPolyRef startRef;
	m_navMeshQuery->findNearestPoly(&localPoint[0], extents.getVec(), queryFilter, &startRef, 0);
	if (!startRef)
		return radius;

	float hitDist = radius;
	Vector3f pos;
	if (!hitPos)
		hitPos = &pos;
	Vector3f normal;
	if (!hitNormal)
		hitNormal = &normal;

	m_navMeshQuery->findDistanceToWall(startRef, &localPoint[0], radius, queryFilter, &hitDist, &hitPos->getVec()[0], &hitNormal->getVec()[0]);
	return hitDist;
}

std::array<int, 2> NavigationMesh::getTileIndex(const Vector3f& position) const {
	const float tileEdgeLength = (float)m_tileSize * m_cellSize;
	const Vector3f localPosition = position - m_boundingBox.min;
	const Vector2f localPosition2D(localPosition[0], localPosition[2]);
	const std::array<int, 2> max = { std::max(0, static_cast<int>(floor(localPosition2D[0] / tileEdgeLength))), std::max(0, static_cast<int>(floor(localPosition2D[1] / tileEdgeLength))) };
	return { std::min(max[0],  getNumTiles()[0] - 1), std::min(max[1],  getNumTiles()[1] - 1) };
}

bool NavigationMesh::getDrawOffMeshConnections() const { 
	return m_drawOffMeshConnections;
}

int NavigationMesh::getTileSize() const { 
	return m_tileSize;
}

float NavigationMesh::getCellSize() const { 
	return m_cellSize;
}

dtNavMesh* NavigationMesh::getDetourNavMesh() { 
	return m_navMesh;
}

float NavigationMesh::getCellHeight() const { 
	return m_cellHeight;
}

float NavigationMesh::getAgentHeight() const { 
	return m_agentHeight;
}

float NavigationMesh::getAgentRadius() const { 
	return m_agentRadius;
}

float NavigationMesh::getAgentMaxClimb() const { 
	return m_agentMaxClimb;
}

float NavigationMesh::getAgentMaxSlope() const { 
	return m_agentMaxSlope;
}

float NavigationMesh::getRegionMinSize() const { 
	return m_regionMinSize;
}

float NavigationMesh::getRegionMergeSize() const { 
	return m_regionMergeSize;
}

float NavigationMesh::getEdgeMaxLength() const { 
	return m_edgeMaxLength;
}

float NavigationMesh::getEdgeMaxError() const { 
	return m_edgeMaxError;
}

float NavigationMesh::getDetailSampleDistance() const { 
	return m_detailSampleDistance;
}

float NavigationMesh::getDetailSampleMaxError() const { 
	return m_detailSampleMaxError;
}

const Vector3f& NavigationMesh::getPadding() const { 
	return m_padding;
}

const BoundingBox& NavigationMesh::getBoundingBox() const { 
	return m_boundingBox;
}

BoundingBox& NavigationMesh::boundingBox() {
	return m_boundingBox;
}

std::array<int, 2> NavigationMesh::getNumTiles() const { 
	return std::array<int, 2>({ m_numTilesX, m_numTilesZ });
}

const int NavigationMesh::getNumTilesX() const {
	return m_numTilesX;
}

const int NavigationMesh::getNumTilesZ() const {
	return m_numTilesZ;
}

int& NavigationMesh::numTilesX() {
	return m_numTilesX;
}

int& NavigationMesh::numTilesZ() {
	return m_numTilesZ;
}

NavmeshPartitionType NavigationMesh::getPartitionType() const { 
	return m_partitionType;
}

bool NavigationMesh::getDrawNavAreas() const { 
	return m_drawNavAreas;
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