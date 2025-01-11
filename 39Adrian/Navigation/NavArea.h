#pragma once

#include "../engine/DebugRenderer.h"
#include "../engine/BoundingBox.h"

class NavArea
{
	//URHO3D_OBJECT(NavArea, Component);

public:
	/// Construct.
	NavArea();
	/// Destruct.
	virtual ~NavArea();
	/// Register object factory and attributes.
	//static void RegisterObject(Context*);

	/// Render debug geometry for the bounds.
	virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);

	/// Get the area id for this volume.
	unsigned GetAreaID() const { return (unsigned)areaID_; }

	/// Set the area id for this volume.
	void SetAreaID(unsigned newID);

	/// Get the bounding box of this navigation area, in local space.
	BoundingBox GetBoundingBox() const { return boundingBox_; }

	/// Set the bounding box of this area, in local space.
	void SetBoundingBox(const BoundingBox& bnds) { boundingBox_ = bnds; }

	/// Get the bounds of this navigation area in world space.
	BoundingBox GetWorldBoundingBox() const;

private:
	/// Bounds of area to mark.
	BoundingBox boundingBox_;
	/// Area id to assign to the marked area.
	unsigned char areaID_;
};