#pragma once

#include <vector>
#include "../engine/Vector.h"
#include "../engine/BoundingBox.h"

class rcContext;

struct dtTileCacheContourSet;
struct dtTileCachePolyMesh;
struct dtTileCacheAlloc;
struct rcCompactHeightfield;
struct rcContourSet;
struct rcHeightfield;
struct rcHeightfieldLayerSet;
struct rcPolyMesh;
struct rcPolyMeshDetail;


struct NavAreaStub{
	BoundingBox bounds;
	unsigned char areaID;
};

struct PolygonStub {
	float* verts;
	int numVerts;
	float minY;
	float maxY; 
	unsigned char areaID;
};

struct NavBuildData{

	NavBuildData();
	virtual ~NavBuildData();

	BoundingBox boundingBox;
	std::vector<Vector3f> vertices;
	std::vector<int> indices;
	std::vector<Vector3f> offMeshVertices;
	std::vector<float> offMeshRadii;
	std::vector<unsigned short> offMeshFlags;
	std::vector<unsigned char> offMeshAreas;
	std::vector<unsigned char> offMeshDir;
	rcContext* ctx;
	rcHeightfield* heightField;
	rcCompactHeightfield* compactHeightField;
	std::vector<NavAreaStub> navAreas;
	std::vector<PolygonStub> polygons;
};

struct SimpleNavBuildData : public NavBuildData{

	SimpleNavBuildData();
	virtual ~SimpleNavBuildData();

	rcContourSet* contourSet;
	rcPolyMesh* polyMesh;
	rcPolyMeshDetail* polyMeshDetail;
};

struct DynamicNavBuildData : public NavBuildData{

	DynamicNavBuildData(dtTileCacheAlloc* alloc);
	virtual ~DynamicNavBuildData();

	dtTileCacheContourSet* contourSet;
	dtTileCachePolyMesh* polyMesh;
	rcHeightfieldLayerSet* heightFieldLayers;
	dtTileCacheAlloc* alloc;
};