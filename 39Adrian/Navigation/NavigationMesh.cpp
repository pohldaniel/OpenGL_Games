#include <DetourNavMesh.h>
#include <DetourNavMeshBuilder.h>
#include <DetourNavMeshQuery.h>
#include <Recast.h>

#include "../engine/scene/ShapeNode.h"
#include "NavigationMesh.h"
#include "Navigable.h"
#include "OffMeshConnection.h"
#include "NavArea.h"

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

void NavigationMesh::DrawDebugGeometry(DebugRenderer* debug, bool depthTest) {

}

void NavigationMesh::DrawDebugGeometry(bool depthTest) {

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
		unsigned numTiles = BuildTiles(geometryList, std::array<int, 2>({0, 0}), std::array<int, 2>({ GetNumTiles()[0] - 1, GetNumTiles()[1] - 1 }));

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
	return false;
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

	for (int z = from[1]; z <= to[1]; ++z)
	{
		for (int x = from[0]; x <= to[0]; ++x)
		{
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
	std::vector<Navigable*> navigables;
	//node_->GetComponents<Navigable>(navigables, true);

	std::hash_set<SceneNodeLC*> processedNodes;
	for (unsigned i = 0; i < navigables.size(); ++i){
		if (true)
			CollectGeometries(geometryList, navigables[i]->m_node, processedNodes, navigables[i]->IsRecursive());
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
	if (processedNodes.find(node) != processedNodes.end())
		return;
	// Exclude obstacles and crowd agents from consideration
	//if (node->HasComponent<Obstacle>() || node->HasComponent<CrowdAgent>())
	//	return;
	processedNodes.insert(node);

	//Matrix4f inverse = node_->GetWorldTransform().Inverse();
	Matrix4f inverse = Matrix4f::IDENTITY;

#if 0
	// Prefer compatible physics collision shapes (triangle mesh, convex hull, box) if found.
	// Then fallback to visible geometry
	std::vector<CollisionShape*> collisionShapes;
	//node->GetComponents<CollisionShape>(collisionShapes);
	bool collisionShapeFound = false;

	for (unsigned i = 0; i < collisionShapes.Size(); ++i)
	{
		CollisionShape* shape = collisionShapes[i];
		if (!shape->IsEnabledEffective())
			continue;

		ShapeType type = shape->GetShapeType();
		if ((type == SHAPE_BOX || type == SHAPE_TRIANGLEMESH || type == SHAPE_CONVEXHULL) && shape->GetCollisionShape())
		{
			Matrix4f shapeTransform(shape->GetPosition(), shape->GetRotation(), shape->GetSize());

			NavigationGeometryInfo info;
			info.component_ = shape;
			info.transform_ = inverse * node->GetWorldTransform() * shapeTransform;
			info.boundingBox_ = shape->GetWorldBoundingBox().Transformed(inverse);

			geometryList.Push(info);
			collisionShapeFound = true;
		}
	}
	if (!collisionShapeFound)
#endif

	{
		std::vector<ShapeNode*> drawables;
		//node->GetDerivedComponents<Drawable>(drawables);

		for (unsigned i = 0; i < drawables.size(); ++i)
		{
			/// \todo Evaluate whether should handle other types. Now StaticModel & TerrainPatch are supported, others skipped
			ShapeNode* drawable = drawables[i];
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

			//info.component_ = drawable;
			info.transform_ = inverse * node->getWorldTransformation();
			info.boundingBox_ = drawable->getWorldBoundingBox().transformed(inverse);

			geometryList.push_back(info);
		}
	}

	if (recursive){
		const std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& children = node->getChildren();
		for (auto it = children.begin(); it != children.end(); ++it) {
			CollectGeometries(geometryList, static_cast<SceneNodeLC*>((*it).get()), processedNodes, recursive);
		}
	}
}