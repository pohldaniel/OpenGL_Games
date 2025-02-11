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

static const int MAX_POLYS = 2048;
std::random_device NavigationMesh::RandomDevice;
std::mt19937 NavigationMesh::MersenTwist(RandomDevice());
std::uniform_real_distribution<float> NavigationMesh::Dist(0.3f, 1.0f);
float NavigationMesh::Scale = 5.0f;

/// Temporary data for finding a path.
struct FindPathData
{
	// Polygons.
	dtPolyRef polys_[MAX_POLYS];
	// Polygons on the path.
	dtPolyRef pathPolys_[MAX_POLYS];
	// Points on the path.
	Vector3f pathPoints_[MAX_POLYS];
	// Flags on the path.
	unsigned char pathFlags_[MAX_POLYS];
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
	keepInterResults_(false),
	drawOffMeshConnections_(false),
	drawNavAreas_(false) {

}

NavigationMesh::~NavigationMesh() {
	ReleaseNavigationMesh();
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

bool NavigationMesh::Allocate(const BoundingBox& boundingBox, unsigned maxTiles) {

	// Release existing navigation data and zero the bounding box
	ReleaseNavigationMesh();

	//if (!node_)
	//	return false;

	//if (!node_->GetWorldScale().Equals(Vector3::ONE))
	//	URHO3D_LOGWARNING("Navigation mesh root node has scaling. Agent parameters may not work as intended");

	//boundingBox_ = boundingBox.Transformed(node_->GetWorldTransform().Inverse());
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
	if (!navMesh_)
	{
		std::cout << "Could not allocate navigation mesh" << std::endl;
		return false;
	}

	if (dtStatusFailed(navMesh_->init(&params)))
	{
		std::cout << "Could not initialize navigation mesh" << std::endl;
		ReleaseNavigationMesh();
		return false;
	}


	std::cout <<  "Allocated empty navigation mesh with max " + std::to_string(maxTiles) + " tiles" << std::endl;

	// Send a notification event to concerned parties that we've been fully rebuilt
	{
		//using namespace NavigationMeshRebuilt;
		//VariantMap& buildEventParams = GetContext()->GetEventDataMap();
		//buildEventParams[P_NODE] = node_;
		//buildEventParams[P_MESH] = this;
		//SendEvent(E_NAVIGATION_MESH_REBUILT, buildEventParams);
	}
	return true;
}

bool NavigationMesh::Build() {
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

		if (dtStatusFailed(navMesh_->init(&params)))
		{
			std::cout << "Could not initialize navigation mesh" << std::endl;
			ReleaseNavigationMesh();
			return false;
		}
		
		// Build each tile
		unsigned numTiles = BuildTiles(geometryList, std::array<int, 2>({0, 0}), std::array<int, 2>({ GetNumTiles()[0], GetNumTiles()[1] }));

		std::cout << "Built navigation mesh with " + std::to_string(maxTiles) + " tiles" << std::endl;
		// Send a notification event to concerned parties that we've been fully rebuilt
		{
			//	using namespace NavigationMeshRebuilt;
			//VariantMap& buildEventParams = GetContext()->GetEventDataMap();
			//buildEventParams[P_NODE] = node_;
			//buildEventParams[P_MESH] = this;
			//SendEvent(E_NAVIGATION_MESH_REBUILT, buildEventParams);
		}

		return true;
	}

	return false;
}

bool NavigationMesh::Build(const BoundingBox& boundingBox) {
	return false;
}

bool NavigationMesh::Build(const std::array<int, 2>& from, const std::array<int, 2>& to) {
	return false;
}

unsigned char* NavigationMesh::GetTileData(const std::array<int, 2>& tile) const {
	return nullptr;
}

bool NavigationMesh::AddTile(const unsigned char*& tileData) {
	return false;
}

void NavigationMesh::RemoveTile(const std::array<int, 2>& tile) {

}

void NavigationMesh::RemoveAllTiles() {
	
}

void NavigationMesh::SetNavigationDataAttr(const unsigned char*& value) {

}

unsigned char* NavigationMesh::GetNavigationDataAttr() const {
	return nullptr;
}

bool NavigationMesh::BuildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z) {
	// Remove previous tile (if any)
	//navMesh_->removeTile(navMesh_->getTileRefAt(x, z, 0), 0, 0);

	BoundingBox tileBoundingBox = GetTileBoudningBox(std::array<int, 2>({x, z}));	 
	m_boxes.push_back(tileBoundingBox);

	SimpleNavBuildData build;

	rcConfig cfg;
	memset(&cfg, 0, sizeof cfg);
	cfg.cs = cellSize_;
	cfg.ch = cellHeight_;
	cfg.walkableSlopeAngle = agentMaxSlope_;
	cfg.walkableHeight = CeilToInt(agentHeight_ / cfg.ch);
	cfg.walkableClimb = FloorToInt(agentMaxClimb_ / cfg.ch);
	cfg.walkableRadius = CeilToInt(agentRadius_ / cfg.cs);
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
		return true; // Nothing to do

	build.heightField_ = rcAllocHeightfield();
	if (!build.heightField_)
	{
		//URHO3D_LOGERROR("Could not allocate heightfield");
		std::cout << "Could not allocate heightfield" << std::endl;
		return false;
	}

	if (!rcCreateHeightfield(build.ctx_, *build.heightField_, cfg.width, cfg.height, cfg.bmin, cfg.bmax, cfg.cs,
		cfg.ch))
	{
		//URHO3D_LOGERROR("Could not create heightfield");
		std::cout << "Could not create heightfield" << std::endl;
		return false;
	}

	unsigned numTriangles = build.indices_.size() / 3;
	unsigned char* triAreas = new unsigned char[numTriangles];
	memset(triAreas, 0, numTriangles);

	rcMarkWalkableTriangles(build.ctx_, cfg.walkableSlopeAngle, &build.vertices_[0][0], build.vertices_.size(),&build.indices_[0], numTriangles, triAreas);
	rcRasterizeTriangles(build.ctx_, &build.vertices_[0][0], build.vertices_.size(), &build.indices_[0],triAreas, numTriangles, *build.heightField_, cfg.walkableClimb);
	rcFilterLowHangingWalkableObstacles(build.ctx_, cfg.walkableClimb, *build.heightField_);

	rcFilterWalkableLowHeightSpans(build.ctx_, cfg.walkableHeight, *build.heightField_);
	rcFilterLedgeSpans(build.ctx_, cfg.walkableHeight, cfg.walkableClimb, *build.heightField_);

	build.compactHeightField_ = rcAllocCompactHeightfield();
	if (!build.compactHeightField_)
	{
		//URHO3D_LOGERROR("Could not allocate create compact heightfield");
		std::cout << "Could not allocate create compact heightfield" << std::endl;
		return false;
	}
	if (!rcBuildCompactHeightfield(build.ctx_, cfg.walkableHeight, cfg.walkableClimb, *build.heightField_,
		*build.compactHeightField_))
	{
		//URHO3D_LOGERROR("Could not build compact heightfield");
		std::cout << "Could not build compact heightfield" << std::endl;
		return false;
	}
	if (!rcErodeWalkableArea(build.ctx_, cfg.walkableRadius, *build.compactHeightField_))
	{
		//URHO3D_LOGERROR("Could not erode compact heightfield");
		std::cout << "Could not erode compact heightfield" << std::endl;
		return false;
	}

	// Mark area volumes
	for (unsigned i = 0; i < build.navAreas_.size(); ++i)
		rcMarkBoxArea(build.ctx_, &build.navAreas_[i].bounds_.min[0], &build.navAreas_[i].bounds_.max[0],
			build.navAreas_[i].areaID_, *build.compactHeightField_);

	if (this->partitionType_ == NAVMESH_PARTITION_WATERSHED)
	{
		if (!rcBuildDistanceField(build.ctx_, *build.compactHeightField_))
		{
			//URHO3D_LOGERROR("Could not build distance field");
			std::cout << "Could not build distance field" << std::endl;
			return false;
		}
		if (!rcBuildRegions(build.ctx_, *build.compactHeightField_, cfg.borderSize, cfg.minRegionArea,
			cfg.mergeRegionArea))
		{
			//URHO3D_LOGERROR("Could not build regions");
			std::cout << "Could not build regions" << std::endl;
			return false;
		}
	}
	else
	{
		if (!rcBuildRegionsMonotone(build.ctx_, *build.compactHeightField_, cfg.borderSize, cfg.minRegionArea, cfg.mergeRegionArea))
		{
			//URHO3D_LOGERROR("Could not build monotone regions");
			std::cout << "Could not build monotone regions" << std::endl;
			return false;
		}
	}

	build.contourSet_ = rcAllocContourSet();
	if (!build.contourSet_)
	{
		//URHO3D_LOGERROR("Could not allocate contour set");
		std::cout << "Could not allocate contour set" << std::endl;
		return false;
	}
	if (!rcBuildContours(build.ctx_, *build.compactHeightField_, cfg.maxSimplificationError, cfg.maxEdgeLen,
		*build.contourSet_))
	{
		//URHO3D_LOGERROR("Could not create contours");
		std::cout << "Could not create contours" << std::endl;
		return false;
	}

	build.polyMesh_ = rcAllocPolyMesh();
	if (!build.polyMesh_)
	{
		//URHO3D_LOGERROR("Could not allocate poly mesh");
		std::cout << "Could not allocate poly mesh" << std::endl;
		return false;
	}
	if (!rcBuildPolyMesh(build.ctx_, *build.contourSet_, cfg.maxVertsPerPoly, *build.polyMesh_))
	{
		//URHO3D_LOGERROR("Could not triangulate contours");
		std::cout << "Could not triangulate contours" << std::endl;
		return false;
	}

	build.polyMeshDetail_ = rcAllocPolyMeshDetail();
	if (!build.polyMeshDetail_)
	{
		//URHO3D_LOGERROR("Could not allocate detail mesh");
		std::cout << "Could not allocate detail mesh" << std::endl;
		return false;
	}
	if (!rcBuildPolyMeshDetail(build.ctx_, *build.polyMesh_, *build.compactHeightField_, cfg.detailSampleDist,
		cfg.detailSampleMaxError, *build.polyMeshDetail_))
	{
		//URHO3D_LOGERROR("Could not build detail mesh");
		std::cout << "Could not build detail mesh" << std::endl;
		return false;
	}

	// Set polygon flags
	/// \todo Assignment of flags from navigation areas?
	for (int i = 0; i < build.polyMesh_->npolys; ++i)
	{
		if (build.polyMesh_->areas[i] != RC_NULL_AREA)
			build.polyMesh_->flags[i] = 0x1;
	}

	unsigned char* navData = 0;
	int navDataSize = 0;

	dtNavMeshCreateParams params;
	memset(&params, 0, sizeof params);
	params.verts = build.polyMesh_->verts;
	params.vertCount = build.polyMesh_->nverts;
	params.polys = build.polyMesh_->polys;
	params.polyAreas = build.polyMesh_->areas;
	params.polyFlags = build.polyMesh_->flags;
	params.polyCount = build.polyMesh_->npolys;
	params.nvp = build.polyMesh_->nvp;
	params.detailMeshes = build.polyMeshDetail_->meshes;
	params.detailVerts = build.polyMeshDetail_->verts;
	params.detailVertsCount = build.polyMeshDetail_->nverts;
	params.detailTris = build.polyMeshDetail_->tris;
	params.detailTriCount = build.polyMeshDetail_->ntris;
	params.walkableHeight = agentHeight_;
	params.walkableRadius = agentRadius_;
	params.walkableClimb = agentMaxClimb_;
	params.tileX = x;
	params.tileY = z;
	rcVcopy(params.bmin, build.polyMesh_->bmin);
	rcVcopy(params.bmax, build.polyMesh_->bmax);
	params.cs = cfg.cs;
	params.ch = cfg.ch;
	params.buildBvTree = true;

	// Add off-mesh connections if have them
	if (build.offMeshRadii_.size())
	{
		params.offMeshConCount = build.offMeshRadii_.size();
		params.offMeshConVerts = &build.offMeshVertices_[0][0];
		params.offMeshConRad = &build.offMeshRadii_[0];
		params.offMeshConFlags = &build.offMeshFlags_[0];
		params.offMeshConAreas = &build.offMeshAreas_[0];
		params.offMeshConDir = &build.offMeshDir_[0];
	}

	if (!dtCreateNavMeshData(&params, &navData, &navDataSize))
	{
		//URHO3D_LOGERROR("Could not build navigation mesh tile data");
		std::cout << "Could not build navigation mesh tile data" << std::endl;
		return false;
	}

	if (dtStatusFailed(navMesh_->addTile(navData, navDataSize, DT_TILE_FREE_DATA, 0, 0)))
	{
		//URHO3D_LOGERROR("Failed to add navigation mesh tile");
		std::cout << "Failed to add navigation mesh tile" << std::endl;
		dtFree(navData);
		return false;
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

void NavigationMesh::ReleaseNavigationMesh() {
	dtFreeNavMesh(navMesh_);
	navMesh_ = 0;

	dtFreeNavMeshQuery(navMeshQuery_);
	navMeshQuery_ = 0;

	numTilesX_ = 0;
	numTilesZ_ = 0;
	boundingBox_.reset();
}

unsigned NavigationMesh::BuildTiles(std::vector<NavigationGeometryInfo>& geometryList, const std::array<int, 2>& from, const std::array<int, 2>& to)
{
	unsigned numTiles = 0;
	for (int z = from[1]; z < to[1]; ++z){
		for (int x = from[0]; x < to[0]; ++x){
			if (BuildTile(geometryList, x, z))
				++numTiles;
		}
	}
	return numTiles;
}

void NavigationMesh::CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList){
	//URHO3D_PROFILE(CollectNavigationGeometry);

	// Get Navigable components from child nodes, not from whole scene. This makes it possible to partition
	// the scene into several navigation meshes
	//std::vector<Navigable*> navigables;
	//node_->GetComponents<Navigable>(navigables, true);
	std::hash_set<SceneNodeLC*> processedNodes;
	for (unsigned i = 0; i < m_navigables.size(); ++i){
		//if (true)
			CollectGeometries(geometryList, m_navigables[i]->m_node, processedNodes, m_navigables[i]->IsRecursive());
	}

	// Get offmesh connections
	//Matrix4f inverse = node_->GetWorldTransform().Inverse();
	std::vector<OffMeshConnection*> connections;
	//node_->GetComponents<OffMeshConnection>(connections, true);

	for (unsigned i = 0; i < connections.size(); ++i)
	{
		OffMeshConnection* connection = connections[i];
		if (true && connection->GetEndPoint())
		{
			const Matrix4f& transform = connection->m_node->getWorldTransformation();

			NavigationGeometryInfo info;
			//info.component_ = connection;
			//info.boundingBox_ = BoundingBox(Sphere(transform.Translation(), connection->GetRadius())).Transformed(inverse);

			geometryList.push_back(info);
		}
	}

	// Get nav area volumes
	std::vector<NavArea*> navAreas;
	//node_->GetComponents<NavArea>(navAreas, true);
	areas_.clear();
	for (unsigned i = 0; i < navAreas.size(); ++i)
	{
		NavArea* area = navAreas[i];
		if (true)
		{
			NavigationGeometryInfo info;
			//info.component_ = area;
			info.boundingBox_ = area->GetWorldBoundingBox();
			geometryList.push_back(info);
			areas_.push_back(area);
		}
	}
}

void NavigationMesh::CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList, SceneNodeLC* node, std::hash_set<SceneNodeLC*>& processedNodes, bool recursive) {
	// Make sure nodes are not included twice
	if (processedNodes.find(node) != processedNodes.end()) {
		return;
	}
	// Exclude obstacles and crowd agents from consideration
	//if (node->HasComponent<Obstacle>() || node->HasComponent<CrowdAgent>())
	//	return;
	processedNodes.insert(node);

	//Matrix4f inverse = node_->GetWorldTransform().Inverse();
	Matrix4f inverse = Matrix4f::IDENTITY;
	{
		//std::vector<ShapeNode*> drawables;
		//node->GetDerivedComponents<Drawable>(drawables);
		ShapeNode* shapeNode = static_cast<ShapeNode*>(node);

		//const Shape& shape = shapeNode->getShape();

		//for (unsigned i = 0; i < drawables.size(); ++i)
		//{
			/// \todo Evaluate whether should handle other types. Now StaticModel & TerrainPatch are supported, others skipped
			//ShapeNode* drawable = drawables[i];
			//if (!drawable->IsEnabledEffective())
			//	continue;

			NavigationGeometryInfo info;
			info.lodLevel_ = 0;

			//if (drawable->GetType() == StaticModel::GetTypeStatic())
			//	info.lodLevel_ = static_cast<StaticModel*>(drawable)->GetOcclusionLodLevel();
			//else if (drawable->GetType() == TerrainPatch::GetTypeStatic())
			//	info.lodLevel_ = 0;
			//else
				//continue;

			info.component_ = shapeNode;
			info.transform_ = inverse * node->getWorldTransformation();
			info.boundingBox_ = shapeNode->getWorldBoundingBox().transformed(inverse);

			//info.transform_.print();

			geometryList.push_back(info);
		//}
	}

	if (recursive){
		const std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& children = node->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it) {
			CollectGeometries(geometryList, static_cast<SceneNodeLC*>((*it).get()), processedNodes, recursive);
		}
	}
}

void NavigationMesh::SetPadding(const Vector3f& padding) {
	padding_ = padding;
}

BoundingBox NavigationMesh::GetTileBoudningBox(const std::array<int, 2>& tile) const
{
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

void NavigationMesh::GetTileGeometry(NavBuildData* build, std::vector<NavigationGeometryInfo>& geometryList, BoundingBox& box){

	Matrix4f inverse = Matrix4f::IDENTITY;
	unsigned int vertexCount = 0u;

	for (unsigned i = 0; i < geometryList.size(); ++i){
		if (box.isInsideFast(geometryList[i].boundingBox_) != BoundingBox::Intersection::OUTSIDE){
			const Matrix4f& transform = geometryList[i].transform_;

			ShapeNode* drawable = geometryList[i].component_;
			if (drawable){
				AddTriMeshGeometry(build, drawable->getShape(), transform, vertexCount);
			}
		}
	}
}

void NavigationMesh::AddTriMeshGeometry(NavBuildData* build, const Shape& shape, const Matrix4f& transform, unsigned int& vertexCount) {
	unsigned srcIndexStart = 0u;
	unsigned srcIndexCount = shape.getIndexBuffer().size();
	unsigned srcVertexStart = 0u;
	unsigned srcVertexCount = shape.getPositions().size();

	for (unsigned k = 0u; k < srcVertexCount; ++k){
		build->vertices_.push_back(transform ^ shape.getPositions()[k]);
	}

	for (unsigned k = 0u; k < srcIndexCount; ++k) {
		build->indices_.push_back(shape.getIndexBuffer()[k] + vertexCount);
	}

	vertexCount += srcVertexCount;
}

void NavigationMesh::SetTileSize(int size){
	tileSize_ = std::max(size, 1);
}

void NavigationMesh::SetAgentHeight(float height){
	agentHeight_ = std::max(height, EPSILON);
}

void NavigationMesh::SetCellHeight(float height){
	cellHeight_ = std::max(height, EPSILON);
}

void NavigationMesh::SetAgentMaxClimb(float maxClimb){
	agentMaxClimb_ = std::max(maxClimb, EPSILON);
}

void NavigationMesh::SetAgentMaxSlope(float maxSlope){
	agentMaxSlope_ = std::max(maxSlope, 0.0f);
}

void NavigationMesh::SetAgentRadius(float radius){
	agentRadius_ = std::max(radius, EPSILON);
}

void NavigationMesh::SetCellSize(float size){
	cellSize_ = std::max(size, EPSILON);
}

Vector3f NavigationMesh::FindNearestPoint(const Vector3f& point, const Vector3f& extents, const dtQueryFilter* filter,dtPolyRef* nearestRef){
	if (!InitializeQuery())
		return point;

	const Matrix4f& transform = Matrix4f::IDENTITY;
	Matrix4f inverse = transform;

	Vector3f localPoint = inverse * point;
	Vector3f nearestPoint;

	dtPolyRef pointRef;
	if (!nearestRef)
		nearestRef = &pointRef;
	navMeshQuery_->findNearestPoly(localPoint.getVec(), extents.getVec(), filter ? filter : queryFilter_, nearestRef, &nearestPoint[0]);

	return *nearestRef ? transform * nearestPoint : point;
}

Vector3f NavigationMesh::randPoint(const Vector3f& center, float radius) {
	const float length = sqrt(Random()) * radius;
	const float degree = Random() * TWO_PI;
	const float x = center[0] + length * cos(degree);
	const float z = center[2] + length * sin(degree);
	return { x, 0.0f, z };
}

Vector3f NavigationMesh::GetRandomPointInCircle(const Vector3f& center, float radius, const Vector3f& extents, const dtQueryFilter* filter, dtPolyRef* randomRef){
	if (randomRef)
		*randomRef = 0;

	if (!InitializeQuery())
		return center;

	//const Matrix3x4& transform = node_->GetWorldTransform();
	//Matrix4f inverse = Matrix4f::IDENTITY;
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

float NavigationMesh::Random() {
	return Dist(MersenTwist) * Scale;
}