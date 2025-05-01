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
class NavPolygon;
class ShapeNode;
class Shape;
class OffMeshConnection;

struct FindPathData;
struct NavBuildData;

struct NavigationGeometryInfo{
	ShapeNode* component = nullptr;
	OffMeshConnection* connection = nullptr;
	NavArea* area = nullptr;
	NavPolygon* polygon = nullptr;
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
	void addNavPolygon(const NavPolygon& navPolygon);
	
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
	void setDrawNavAreas(bool enable);
	void setDrawNavPolygons(bool enable);
	
	float getAreaCost(unsigned areaID) const;
	Vector3f getRandomPoint(const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	Vector3f getRandomPointInCircle(const Vector3f& center, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, dtPolyRef* randomRef = 0);
	float getDistanceToWall(const Vector3f& point, float radius, const Vector3f& extents = Vector3f::ONE, const dtQueryFilter* filter = 0, Vector3f* hitPos = 0, Vector3f* hitNormal = 0);
	std::array<int, 2> getTileIndex(const Vector3f& position) const;
	BoundingBox getTileBoudningBox(const std::array<int, 2>& tile) const;
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
	const std::unordered_map<int, Buffer>& getTileData() const;
	std::unordered_map<int, Buffer>& tileData();	
	const std::vector<NavArea>& getNavAreas() const;
	std::vector<NavArea>& navAreas();
	bool getDrawOffMeshConnections() const;
	void setDrawOffMeshConnections(bool enable);

protected:
	
	virtual void releaseNavigationMesh();

	void collectGeometries(std::vector<NavigationGeometryInfo>& geometryList);
	void collectGeometries(std::vector<NavigationGeometryInfo>& geometryList, SceneNodeLC* node, std::hash_set<SceneNodeLC*>& processedNodes, bool recursive);
	void getTileGeometry(NavBuildData* build, std::vector<NavigationGeometryInfo>& geometryList, BoundingBox& box);
	void addTriMeshGeometry(NavBuildData* build, const Shape& shape, const Matrix4f& transform, unsigned int& vertexCount);	
	unsigned int buildTiles(std::vector<NavigationGeometryInfo>& geometryList, const std::array<int, 2>& from, const std::array<int, 2>& to);
	Vector3f randPoint(const Vector3f& center, float radius);
	bool buildTile(std::vector<NavigationGeometryInfo>& geometryList, int x, int z);
	bool initializeQuery();
	
	std::vector<Navigable> m_navigables;
	std::vector<OffMeshConnection> m_offMeshConnections;
	std::vector<NavArea> m_navAreas;
	std::vector<NavPolygon> m_navPolygons;

	dtNavMesh* m_navMesh;
	dtNavMeshQuery* m_navMeshQuery;
	dtQueryFilter* m_queryFilter;
	FindPathData*  m_pathData;
	BoundingBox m_boundingBox;
	std::unordered_map<int, Buffer> m_tileData;

	int m_tileSize;
	float m_cellSize;
	float m_cellHeight;
	float m_agentHeight;
	float m_agentRadius;
	float m_agentMaxClimb;
	float m_agentMaxSlope;
	float m_regionMinSize;
	float m_regionMergeSize;
	float m_edgeMaxLength;
	float m_edgeMaxError;
	float m_detailSampleDistance;
	float m_detailSampleMaxError;
	int m_numTilesX;
	int m_numTilesZ;
	Vector3f m_padding;
	NavmeshPartitionType m_partitionType;
	bool m_drawNavAreas;
	bool m_drawNavPolygons;
	bool m_drawOffMeshConnections;
	std::vector<BoundingBox> m_boxes;
	
	static std::random_device RandomDevice;
	static std::mt19937 MersenTwist;
	static std::uniform_real_distribution<float> Dist;
	static float Scale;

	static float Random();
};