#include <DetourTileCacheBuilder.h>
#include <Recast.h>

#include "NavBuildData.h"

NavBuildData::NavBuildData() :
	ctx(new rcContext(true)),
	heightField(0),
	compactHeightField(0)
{
}

NavBuildData::~NavBuildData(){
	delete(ctx);
	ctx = nullptr;
	rcFreeHeightField(heightField);
	heightField = nullptr;
	rcFreeCompactHeightfield(compactHeightField);
	compactHeightField = nullptr;
}

SimpleNavBuildData::SimpleNavBuildData() :
	NavBuildData(),
	contourSet(nullptr),
	polyMesh(nullptr),
	polyMeshDetail(nullptr){
}

SimpleNavBuildData::~SimpleNavBuildData(){
	rcFreeContourSet(contourSet);
	contourSet = nullptr;
	rcFreePolyMesh(polyMesh);
	polyMesh = nullptr;
	rcFreePolyMeshDetail(polyMeshDetail);
	polyMeshDetail = nullptr;
}

DynamicNavBuildData::DynamicNavBuildData(dtTileCacheAlloc* allocator) :
	NavBuildData(),
	contourSet(nullptr),
	polyMesh(nullptr),
	heightFieldLayers(nullptr),
	alloc(allocator){
}

DynamicNavBuildData::~DynamicNavBuildData(){
	dtFreeTileCacheContourSet(alloc, contourSet);
	contourSet = nullptr;
	dtFreeTileCachePolyMesh(alloc, polyMesh);
	polyMesh = nullptr;
	rcFreeHeightfieldLayerSet(heightFieldLayers);
	heightFieldLayers = nullptr;
}