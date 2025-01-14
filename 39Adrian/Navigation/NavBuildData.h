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



/// Navigation area stub.
struct NavAreaStub
{
	/// Area bounding box.
	BoundingBox bounds_;
	/// Area ID.
	unsigned char areaID_;
};

/// Navigation build data.
struct NavBuildData
{
	/// Constructor.
	NavBuildData();
	/// Destructor.
	virtual ~NavBuildData();

	/// World-space bounding box of the navigation mesh tile.
	BoundingBox worldBoundingBox_;
	/// Vertices from geometries.
	std::vector<Vector3f> vertices_;
	/// Triangle indices from geometries.
	std::vector<int> indices_;
	/// Offmesh connection vertices.
	std::vector<Vector3f> offMeshVertices_;
	/// Offmesh connection radii.
	std::vector<float> offMeshRadii_;
	/// Offmesh connection flags.
	std::vector<unsigned short> offMeshFlags_;
	/// Offmesh connection areas.
	std::vector<unsigned char> offMeshAreas_;
	/// Offmesh connection direction.
	std::vector<unsigned char> offMeshDir_;
	/// Recast context.
	rcContext* ctx_;
	/// Recast heightfield.
	rcHeightfield* heightField_;
	/// Recast compact heightfield.
	rcCompactHeightfield* compactHeightField_;
	/// Pretransformed navigation areas, no correlation to the geometry above.
	std::vector<NavAreaStub> navAreas_;
};

struct SimpleNavBuildData : public NavBuildData
{
	/// Constructor.
	SimpleNavBuildData();
	/// Descturctor.
	virtual ~SimpleNavBuildData();

	/// Recast contour set.
	rcContourSet* contourSet_;
	/// Recast poly mesh.
	rcPolyMesh* polyMesh_;
	/// Recast detail poly mesh.
	rcPolyMeshDetail* polyMeshDetail_;
};