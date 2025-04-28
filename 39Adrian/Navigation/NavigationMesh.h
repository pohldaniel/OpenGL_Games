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

enum NavmeshPartitionType{
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

struct NavigationGeometryInfo{
	ShapeNode* component = nullptr;
	OffMeshConnection* connection = nullptr;
	NavArea* area = nullptr;
	unsigned int lodLevel;
	Matrix4f transform;
	BoundingBox boundingBox;
};

/// A flag representing the type of path point- none, the start of a path segment, the end of one, or an off-mesh connection.
enum NavigationPathPointFlag{
	NAVPATHFLAG_NONE = 0,
	NAVPATHFLAG_START = 0x01,
	NAVPATHFLAG_END = 0x02,
	NAVPATHFLAG_OFF_MESH = 0x04
};

struct NavigationPathPoint{
	Vector3f m_position;
	NavigationPathPointFlag m_flag;
	unsigned char m_areaID;
};


struct Buffer {	
	Buffer() : size(0), data(nullptr){}
	void resize(size_t size);
	size_t size;
	char* data;	
};

class NavigationMesh{
	friend class CrowdManager;
	friend struct MeshProcess;

public:

	NavigationMesh();
	virtual ~NavigationMesh();
	virtual void OnRenderDebug();

	virtual bool allocate();
	virtual bool allocate(const BoundingBox& boundingBox, unsigned int tilesX, unsigned int tilesZ);
	virtual bool allocate(const BoundingBox& boundingBox, unsigned int maxTiles);
	virtual bool build();
	virtual Buffer& getTileData(Buffer& buffer, const std::array<int, 2>& tile) const;
	virtual Buffer& getTileData(int x, int z);
	virtual bool addTile(const Buffer& tileData);
	virtual bool addTile(int x, int z);
	virtual void removeTile(const std::array<int, 2>& tile, unsigned int layersToRemove = 0u);
	virtual void removeAllTiles();
	
	void addTiles();
	bool hasTile(const std::array<int, 2>& tile) const;
	bool hasTileData(int x, int z) const;
	void writeTile(Buffer& dest, int x, int z) const;
	bool readTile(const Buffer& source);
	void saveToTileData();
	void clearTileData();
	void addNavigable(const Navigable& navigable);
	void addOffMeshConnection(const OffMeshConnection& offMeshConnection);
	void addNavArea(const NavArea& navArea);
	
	Vector3f moveAlongSurface(const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, int maxVisited = 3, const dtQueryFilter* filter = 0);
	Vector3f findNearestPoint(const Vector3f& point, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, dtPolyRef* nearestRef = 0);
	void findPath(std::vector<Vector3f>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);
	void findPath(std::vector<NavigationPathPoint>& dest, const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);	
	Vector3f raycast(const Vector3f& start, const Vector3f& end, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, Vector3f* hitNormal = 0);
	bool isInitialized() const;

	void setTileSize(int size);
	void setCellSize(float size);
	void setCellHeight(float height);
	void setAgentHeight(float height);
	void setAgentRadius(float radius);
	void setAgentMaxClimb(float maxClimb);
	void setAgentMaxSlope(float maxSlope);
	void setPadding(const Vector3f& padding);
	void setAreaCost(unsigned int areaID, float  cost);
	int setPolyFlag(const Vector3f& point, unsigned short polyMask, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0);
	void setPartitionType(NavmeshPartitionType aType);
	void setDrawOffMeshConnections(bool enable);
	void setDrawNavAreas(bool enable);
	
	float getAreaCost(unsigned areaID) const;
	Vector3f getRandomPoint(const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	Vector3f getRandomPointInCircle(const Vector3f& center, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	float getDistanceToWall(const Vector3f& point, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, Vector3f* hitPos = 0, Vector3f* hitNormal = 0);
	std::array<int, 2> getTileIndex(const Vector3f& position) const;
	BoundingBox getTileBoudningBox(const std::array<int, 2>& tile) const;
	bool getDrawOffMeshConnections() const;
	int getTileSize() const;
	float getCellSize() const;
	dtNavMesh* getDetourNavMesh();
	float getCellHeight() const;
	float getAgentHeight() const;
	float getAgentRadius() const;
	float getAgentMaxClimb() const;
	float getAgentMaxSlope() const;
	float getRegionMinSize() const;
	float getRegionMergeSize() const;
	float getEdgeMaxLength() const;
	float getEdgeMaxError() const;
	float getDetailSampleDistance() const;
	float getDetailSampleMaxError() const;
	const Vector3f& getPadding() const;
	const BoundingBox& getBoundingBox() const;
	BoundingBox& boundingBox();
	std::array<int, 2> getNumTiles() const;
	const int getNumTilesX() const;
	const int getNumTilesZ() const;
	int& numTilesX();
	int& numTilesZ();
	NavmeshPartitionType getPartitionType() const;
	bool getDrawNavAreas() const;
	std::unordered_map<int, Buffer>& tileData();
	const std::vector<NavArea>& getNavAreas() const;
	std::vector<NavArea>& navAreas();
//private:
	
	

protected:

	virtual bool BuildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z);
	virtual void releaseNavigationMesh();

	void CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList);
	void CollectGeometries(std::vector<NavigationGeometryInfo>& geometryList, SceneNodeLC* node, std::hash_set<SceneNodeLC*>& processedNodes, bool recursive);
	void GetTileGeometry(NavBuildData* build, std::vector<NavigationGeometryInfo>& geometryList, BoundingBox& box);
	void AddTriMeshGeometry(NavBuildData* build, const Shape& shape, const Matrix4f& transform, unsigned int& vertexCount);	
	unsigned int BuildTiles(std::vector<NavigationGeometryInfo>& geometryList, const std::array<int, 2>& from, const std::array<int, 2>& to);
	bool InitializeQuery();
	

	std::vector<Navigable> m_navigables;
	std::vector<OffMeshConnection> m_offMeshConnections;
	std::vector<NavArea> m_navAreas;

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