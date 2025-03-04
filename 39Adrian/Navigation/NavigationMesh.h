#pragma once
#include <array>
#include <vector>
#include <string>
#include <hash_set>
#include <memory>
#include <random>

#include "../engine/Vector.h"
#include "../engine/BoundingBox.h"
#include "../engine/DebugRenderer.h"
#include "Navigable.h"

#ifdef DT_POLYREF64
typedef uint64_t dtPolyRef;
#else
typedef unsigned int dtPolyRef;
#endif

class dtNavMesh;
class dtNavMeshQuery;
class dtQueryFilter;

enum NavmeshPartitionType
{
	NAVMESH_PARTITION_WATERSHED = 0,
	NAVMESH_PARTITION_MONOTONE
};

class Geometry;
class NavArea;
class ShapeNode;
class Shape;

struct FindPathData;
struct NavBuildData;

/// Description of a navigation mesh geometry component, with transform and bounds information.
struct NavigationGeometryInfo
{
	/// Component.
	ShapeNode* component_;
	/// Geometry LOD level if applicable.
	unsigned lodLevel_;
	/// Transform relative to the navigation mesh root node.
	Matrix4f transform_;
	/// Bounding box relative to the navigation mesh root node.
	BoundingBox boundingBox_;

};

/// A flag representing the type of path point- none, the start of a path segment, the end of one, or an off-mesh connection.
enum NavigationPathPointFlag
{
	NAVPATHFLAG_NONE = 0,
	NAVPATHFLAG_START = 0x01,
	NAVPATHFLAG_END = 0x02,
	NAVPATHFLAG_OFF_MESH = 0x04
};

struct NavigationPathPoint
{
	/// World-space position of the path point.
	Vector3f position_;
	/// Detour flag.
	NavigationPathPointFlag flag_;
	/// Detour area ID.
	unsigned char areaID_;
};

inline unsigned NextPowerOfTwo(unsigned value)
{
	// http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
	--value;
	value |= value >> 1;
	value |= value >> 2;
	value |= value >> 4;
	value |= value >> 8;
	value |= value >> 16;
	return ++value;
}

template <class T> int CeilToInt(T x) { return static_cast<int>(ceil(x)); }
template <class T> int FloorToInt(T x) { return static_cast<int>(floor(x)); }

inline unsigned LogBaseTwo(unsigned value)
{
	// http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogObvious
	unsigned ret = 0;
	while (value >>= 1)     // Unroll for more speed...
		++ret;
	return ret;
}

struct Buffer {
	
	Buffer() : size(0), data(nullptr){

	}

	void resize(size_t size);

	size_t size;
	unsigned char* data;
	
};

class NavigationMesh
{
	friend class CrowdManager;

public:

	/// Construct.
	NavigationMesh();
	/// Destruct.
	virtual ~NavigationMesh();
	/// Register object factory.
	static void RegisterObject();

	/// Visualize the component as debug geometry.
	virtual void OnRenderDebug();

	/// Set tile size.
	void SetTileSize(int size);
	/// Set cell size.
	void SetCellSize(float size);
	/// Set cell height.
	void SetCellHeight(float height);
	/// Set navigation agent height.
	void SetAgentHeight(float height);
	/// Set navigation agent radius.
	void SetAgentRadius(float radius);
	/// Set navigation agent max vertical climb.
	void SetAgentMaxClimb(float maxClimb);
	/// Set navigation agent max slope.
	void SetAgentMaxSlope(float maxSlope);
	/// Set region minimum size.
	void SetRegionMinSize(float size);
	/// Set region merge size.
	void SetRegionMergeSize(float size);
	/// Set edge max length.
	void SetEdgeMaxLength(float length);
	/// Set edge max error.
	void SetEdgeMaxError(float error);
	/// Set detail sampling distance.
	void SetDetailSampleDistance(float distance);
	/// Set detail sampling maximum error.
	void SetDetailSampleMaxError(float error);
	/// Set padding of the navigation mesh bounding box. Having enough padding allows to add geometry on the extremities of the navigation mesh when doing partial rebuilds.
	void SetPadding(const Vector3f& padding);
	/// Set the cost of an area.
	void SetAreaCost(unsigned areaID, float cost);
	/// Allocate the navigation mesh without building any tiles. Bounding box is not padded. Return true if successful.
	virtual bool Allocate(const BoundingBox& boundingBox, unsigned maxTiles);
	/// Rebuild the navigation mesh. Return true if successful.
	virtual bool Build();
	/// Rebuild part of the navigation mesh contained by the world-space bounding box. Return true if successful.
	virtual bool Build(const BoundingBox& boundingBox);
	/// Rebuild part of the navigation mesh in the rectangular area. Return true if successful.
	virtual bool Build(const std::array<int,2>& from, const std::array<int, 2>& to);
	/// Return tile data.
	virtual Buffer GetTileData(const std::array<int, 2>& tile) const;
	/// Add tile to navigation mesh.
	virtual bool AddTile(const Buffer& tileData);
	/// Remove tile from navigation mesh.
	virtual void RemoveTile(const std::array<int, 2>& tile);
	/// Remove all tiles from navigation mesh.
	virtual void RemoveAllTiles();
	/// Return whether the navigation mesh has tile.
	bool HasTile(const std::array<int, 2>& tile) const;
	/// Return bounding box of the tile in the node space.
	BoundingBox GetTileBoudningBox(const std::array<int, 2>& tile) const;
	/// Return index of the tile at the position.
	std::array<int, 2> GetTileIndex(const Vector3f& position) const;
	/// Find the nearest point on the navigation mesh to a given point. Extents specifies how far out from the specified point to check along each axis.
	Vector3f FindNearestPoint(const Vector3f& point, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, dtPolyRef* nearestRef = 0);
	/// Try to move along the surface from one point to another.
	Vector3f MoveAlongSurface(const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, int maxVisited = 3, const dtQueryFilter* filter = 0);
	/// Find a path between world space points. Return non-empty list of points if successful. Extents specifies how far off the navigation mesh the points can be.
	void FindPath(std::vector<Vector3f>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);
	/// Find a path between world space points. Return non-empty list of navigation path points if successful. Extents specifies how far off the navigation mesh the points can be.
	void FindPath(std::vector<NavigationPathPoint>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);
	/// Return a random point on the navigation mesh.
	Vector3f GetRandomPoint(const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	/// Return a random point on the navigation mesh within a circle. The circle radius is only a guideline and in practice the returned point may be further away.
	Vector3f GetRandomPointInCircle(const Vector3f& center, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	/// Return distance to wall from a point. Maximum search radius must be specified.
	float GetDistanceToWall(const Vector3f& point, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, Vector3f* hitPos = 0, Vector3f* hitNormal = 0);
	/// Perform a walkability raycast on the navigation mesh between start and end and return the point where a wall was hit, or the end point if no walls.
	Vector3f Raycast(const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, Vector3f* hitNormal = 0);
	///Lumak - editable feature
	int SetPolyFlag(const Vector3f& point, unsigned short polyMask, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);

	/// Return the given name of this navigation mesh.
	std::string GetMeshName() const { return meshName_; }

	/// Set the name of this navigation mesh.
	void SetMeshName(const std::string& newName);

	/// Return tile size.
	int GetTileSize() const { return tileSize_; }

	/// Return cell size.
	float GetCellSize() const { return cellSize_; }

	dtNavMesh* GetDetourNavMesh() { return navMesh_; }

	/// Return cell height.
	float GetCellHeight() const { return cellHeight_; }

	/// Return navigation agent height.
	float GetAgentHeight() const { return agentHeight_; }

	/// Return navigation agent radius.
	float GetAgentRadius() const { return agentRadius_; }

	/// Return navigation agent max vertical climb.
	float GetAgentMaxClimb() const { return agentMaxClimb_; }

	/// Return navigation agent max slope.
	float GetAgentMaxSlope() const { return agentMaxSlope_; }

	/// Return region minimum size.
	float GetRegionMinSize() const { return regionMinSize_; }

	/// Return region merge size.
	float GetRegionMergeSize() const { return regionMergeSize_; }

	/// Return edge max length.
	float GetEdgeMaxLength() const { return edgeMaxLength_; }

	/// Return edge max error.
	float GetEdgeMaxError() const { return edgeMaxError_; }

	/// Return detail sampling distance.
	float GetDetailSampleDistance() const { return detailSampleDistance_; }

	/// Return detail sampling maximum error.
	float GetDetailSampleMaxError() const { return detailSampleMaxError_; }

	/// Return navigation mesh bounding box padding.
	const Vector3f& GetPadding() const { return padding_; }

	/// Get the current cost of an area
	float GetAreaCost(unsigned areaID) const;

	/// Return whether has been initialized with valid navigation data.
	bool IsInitialized() const { return navMesh_ != 0; }

	/// Return local space bounding box of the navigation mesh.
	const BoundingBox& GetBoundingBox() const { return boundingBox_; }

	/// Return world space bounding box of the navigation mesh.
	BoundingBox GetWorldBoundingBox() const;

	/// Return number of tiles.
	std::array<int, 2> GetNumTiles() const { return std::array<int, 2>({ numTilesX_, numTilesZ_ }); }

	/// Set the partition type used for polygon generation.
	void SetPartitionType(NavmeshPartitionType aType);

	/// Return Partition Type.
	NavmeshPartitionType GetPartitionType() const { return partitionType_; }

	/// Set navigation data attribute.
	virtual void SetNavigationDataAttr(const unsigned char*& value);
	/// Return navigation data attribute.
	virtual unsigned char* GetNavigationDataAttr() const;

	/// Draw debug geometry for OffMeshConnection components.
	void SetDrawOffMeshConnections(bool enable) { drawOffMeshConnections_ = enable; }

	/// Return whether to draw OffMeshConnection components.
	bool GetDrawOffMeshConnections() const { return drawOffMeshConnections_; }

	/// Draw debug geometry for NavArea components.
	void SetDrawNavAreas(bool enable) { drawNavAreas_ = enable; }

	/// Return whether to draw NavArea components.
	bool GetDrawNavAreas() const { return drawNavAreas_; }
	std::vector<Navigable*> m_navigables;
	//private:
	/// Write tile data.
	void WriteTile(unsigned char*& dest, int x, int z) const;
	/// Read tile data to the navigation mesh.
	//bool ReadTile(Deserializer& source, bool silent);

//protected:
	/// Collect geometry from under Navigable components.
	void CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList);
	/// Visit nodes and collect navigable geometry.
	void CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList, SceneNodeLC* node, std::hash_set<SceneNodeLC*>& processedNodes, bool recursive);
	/// Get geometry data within a bounding box.
	void GetTileGeometry(NavBuildData* build, std::vector<NavigationGeometryInfo>& geometryList, BoundingBox& box);
	/// Add a triangle mesh to the geometry data.
	void AddTriMeshGeometry(NavBuildData* build, const Shape& shape, const Matrix4f& transform, unsigned int& vertexCount);
	/// Build one tile of the navigation mesh. Return true if successful.
	virtual bool BuildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z);
	/// Build tiles in the rectangular area. Return number of built tiles.
	unsigned BuildTiles(std::vector<NavigationGeometryInfo>& geometryList, const std::array<int, 2>& from, const std::array<int, 2>& to);
	/// Ensure that the navigation mesh query is initialized. Return true if successful.
	bool InitializeQuery();
	/// Release the navigation mesh and the query.
	virtual void ReleaseNavigationMesh();

	/// Identifying name for this navigation mesh.
	std::string meshName_;
	/// Detour navigation mesh.
	dtNavMesh* navMesh_;
	/// Detour navigation mesh query.
	dtNavMeshQuery* navMeshQuery_;
	/// Detour navigation mesh query filter.
	//UniquePtr<dtQueryFilter> queryFilter_;
	dtQueryFilter* queryFilter_;
	/// Temporary data for finding a path.
	//UniquePtr<FindPathData> pathData_;
	FindPathData*  pathData_;
	/// Tile size.
	int tileSize_;
	/// Cell size.
	float cellSize_;
	/// Cell height.
	float cellHeight_;
	/// Navigation agent height.
	float agentHeight_;
	/// Navigation agent radius.
	float agentRadius_;
	/// Navigation agent max vertical climb.
	float agentMaxClimb_;
	/// Navigation agent max slope.
	float agentMaxSlope_;
	/// Region minimum size.
	float regionMinSize_;
	/// Region merge size.
	float regionMergeSize_;
	/// Edge max length.
	float edgeMaxLength_;
	/// Edge max error.
	float edgeMaxError_;
	/// Detail sampling distance.
	float detailSampleDistance_;
	/// Detail sampling maximum error.
	float detailSampleMaxError_;
	/// Bounding box padding.
	Vector3f padding_;
	/// Number of tiles in X direction.
	int numTilesX_;
	/// Number of tiles in Z direction.
	int numTilesZ_;
	/// Whole navigation mesh bounding box.
	BoundingBox boundingBox_;
	/// Type of the heightfield partitioning.
	NavmeshPartitionType partitionType_;
	/// Keep internal build resources for debug draw modes.
	bool keepInterResults_;
	/// Debug draw OffMeshConnection components.
	bool drawOffMeshConnections_;
	/// Debug draw NavArea components.
	bool drawNavAreas_;
	/// NavAreas for this NavMesh
	std::vector<NavArea*> areas_;

	std::vector<BoundingBox> m_boxes;
	Vector3f randPoint(const Vector3f& center, float radius);

	static std::random_device RandomDevice;
	static std::mt19937 MersenTwist;
	static std::uniform_real_distribution<float> Dist;
	static float Scale;

	static float Random();
};