#include <DetourTileCacheBuilder.h>
#include <Recast.h>

#include "NavBuildData.h"

NavBuildData::NavBuildData() :
	ctx_(new rcContext(true)),
	heightField_(0),
	compactHeightField_(0)
{
}

NavBuildData::~NavBuildData()
{
	delete(ctx_);
	ctx_ = 0;
	rcFreeHeightField(heightField_);
	heightField_ = 0;
	rcFreeCompactHeightfield(compactHeightField_);
	compactHeightField_ = 0;
}

SimpleNavBuildData::SimpleNavBuildData() :
	NavBuildData(),
	contourSet_(0),
	polyMesh_(0),
	polyMeshDetail_(0)
{
}

SimpleNavBuildData::~SimpleNavBuildData()
{
	rcFreeContourSet(contourSet_);
	contourSet_ = 0;
	rcFreePolyMesh(polyMesh_);
	polyMesh_ = 0;
	rcFreePolyMeshDetail(polyMeshDetail_);
	polyMeshDetail_ = 0;
}