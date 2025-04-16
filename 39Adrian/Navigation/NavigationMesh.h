#pragma once
#include <array>
#include <vector>
#include <string>
#include <hash_set>
#include <memory>
#include <random>
#include <unordered_map>

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
class OffMeshConnection;

struct FindPathData;
struct NavBuildData;

/// Description of a navigation mesh geometry component, with transform and bounds information.
struct NavigationGeometryInfo
{
	/// Component.
	ShapeNode* component_ = nullptr;
	/// Connection.
	OffMeshConnection* connection_ = nullptr;
	/// Connection.
	NavArea* area_ = nullptr;
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
	char* data;
	
};

class NavigationMesh{
	friend class CrowdManager;

public:

	NavigationMesh();
	virtual ~NavigationMesh();
	virtual void OnRenderDebug();

	void SetTileSize(int size);
	void SetCellSize(float size);
	void SetCellHeight(float height);
	void SetAgentHeight(float height);
	void SetAgentRadius(float radius);
	void SetAgentMaxClimb(float maxClimb);
	void SetAgentMaxSlope(float maxSlope);
	/// Set padding of the navigation mesh bounding box. Having enough padding allows to add geometry on the extremities of the navigation mesh when doing partial rebuilds.
	void SetPadding(const Vector3f& padding);
	/// Set the cost of an area.
	void SetAreaCost(unsigned areaID, float cost);

	virtual bool Allocate();
	virtual bool Allocate(const BoundingBox& boundingBox, unsigned tilesX, unsigned tilesZ);
	virtual bool Allocate(const BoundingBox& boundingBox, unsigned maxTiles);
	virtual bool Build();
	virtual bool Build(const BoundingBox& boundingBox);
	virtual bool Build(const std::array<int,2>& from, const std::array<int, 2>& to);
	virtual Buffer& GetTileData(Buffer& buffer, const std::array<int, 2>& tile) const;
	virtual Buffer& GetTileData(int x, int z);
	virtual bool AddTile(const Buffer& tileData);
	virtual bool AddTile(int x, int z);
	virtual void RemoveTile(const std::array<int, 2>& tile, unsigned int layersToRemove = 0u);
	virtual void RemoveAllTiles();
	void AddTiles();

	bool HasTile(const std::array<int, 2>& tile) const;
	bool HasTileData(int x, int z) const;
	std::array<int, 2> GetTileIndex(const Vector3f& position) const;

	Vector3f MoveAlongSurface(const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, int maxVisited = 3, const dtQueryFilter* filter = 0);
	Vector3f FindNearestPoint(const Vector3f& point, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, dtPolyRef* nearestRef = 0);
	/// Find a path between world space points. Return non-empty list of points if successful. Extents specifies how far off the navigation mesh the points can be.
	void FindPath(std::vector<Vector3f>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);
	/// Find a path between world space points. Return non-empty list of navigation path points if successful. Extents specifies how far off the navigation mesh the points can be.
	void FindPath(std::vector<NavigationPathPoint>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);
	
	

	Vector3f GetRandomPoint(const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	Vector3f GetRandomPointInCircle(const Vector3f& center, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	float GetDistanceToWall(const Vector3f& point, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, Vector3f* hitPos = 0, Vector3f* hitNormal = 0);
	Vector3f Raycast(const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, Vector3f* hitNormal = 0);
	int SetPolyFlag(const Vector3f& point, unsigned short polyMask, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);


	int GetTileSize() const { return tileSize_; }
	float GetCellSize() const { return cellSize_; }
	dtNavMesh* GetDetourNavMesh() { return navMesh_; }
	float GetCellHeight() const { return cellHeight_; }
	float GetAgentHeight() const { return agentHeight_; }
	float GetAgentRadius() const { return agentRadius_; }
	float GetAgentMaxClimb() const { return agentMaxClimb_; }
	float GetAgentMaxSlope() const { return agentMaxSlope_; }
	float GetRegionMinSize() const { return regionMinSize_; }
	float GetRegionMergeSize() const { return regionMergeSize_; }
	float GetEdgeMaxLength() const { return edgeMaxLength_; }
	float GetEdgeMaxError() const { return edgeMaxError_; }
	float GetDetailSampleDistance() const { return detailSampleDistance_; }
	float GetDetailSampleMaxError() const { return detailSampleMaxError_; }
	const Vector3f& GetPadding() const { return padding_; }
	float GetAreaCost(unsigned areaID) const;
	bool IsInitialized() const { return navMesh_ != 0; }
	const BoundingBox& GetBoundingBox() const { return boundingBox_; }


	std::array<int, 2> GetNumTiles() const { return std::array<int, 2>({ numTilesX_, numTilesZ_ }); }
	void SetPartitionType(NavmeshPartitionType aType);
	NavmeshPartitionType GetPartitionType() const { return partitionType_; }



	void SetDrawOffMeshConnections(bool enable) { drawOffMeshConnections_ = enable; }
	bool GetDrawOffMeshConnections() const { return drawOffMeshConnections_; }
	void SetDrawNavAreas(bool enable) { drawNavAreas_ = enable; }
	void WriteTile(Buffer& dest, int x, int z) const;
	bool ReadTile(const Buffer& source);
	void saveToTileData();
	void clearTileData();

	/// Return whether to draw NavArea components.
	bool GetDrawNavAreas() const { return drawNavAreas_; }
	std::vector<Navigable*> m_navigables;
	std::vector<OffMeshConnection*> m_offMeshConnections;
	std::vector<NavArea*> m_navAreas;
//private:
	BoundingBox GetTileBoudningBox(const std::array<int, 2>& tile) const;
	

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
	std::unordered_map<int, Buffer> m_tileData;

	std::vector<BoundingBox> m_boxes;
	Vector3f randPoint(const Vector3f& center, float radius);

	static std::random_device RandomDevice;
	static std::mt19937 MersenTwist;
	static std::uniform_real_distribution<float> Dist;
	static float Scale;

	static float Random();
};