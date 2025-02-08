#pragma once
#include "../engine/DebugRenderer.h"
#include "../engine/scene/SceneNodeLC.h"

class DynamicNavigationMesh;

/// Obstacle for dynamic navigation mesh.
class Obstacle {
	//URHO3D_OBJECT(Obstacle, Component)

	friend class DynamicNavigationMesh;

public:
	/// Construct.
	Obstacle(SceneNodeLC* node);
	/// Destruct.
	virtual ~Obstacle();

	/// Register Obstacle with engine context.
	//static void RegisterObject(Context*);

	/// Update the owning mesh when enabled status has changed.
	virtual void OnSetEnabled();

	/// Get the height of this obstacle.
	float GetHeight() const { return height_; }

	/// Set the height of this obstacle.
	void SetHeight(float newHeight);

	/// Get the blocking radius of this obstacle.
	float GetRadius() const { return radius_; }

	/// Set the blocking radius of this obstacle.
	void SetRadius(float newRadius);

	/// Get the internal obstacle ID.
	unsigned GetObstacleID() const { return obstacleId_; }

	/// Render debug information.
	void OnRenderDebug();

	SceneNodeLC* m_node;

	DynamicNavigationMesh* ownerMesh_;

protected:
	/// Handle node being assigned.
	virtual void OnNodeSet(SceneNodeLC* node);
	/// Handle scene being assigned, identify our DynamicNavigationMesh.
	//virtual void OnSceneSet(Scene* scene);
	/// Handle node transform being dirtied.
	virtual void OnMarkedDirty(SceneNodeLC* node);
	/// Handle navigation mesh tile added.
	//void HandleNavigationTileAdded(StringHash eventType, VariantMap& eventData);

private:
	/// Radius of this obstacle.
	float radius_;
	/// Height of this obstacle, extends 1/2 height below and 1/2 height above the owning node's position.
	float height_;

	/// Id received from tile cache.
	unsigned obstacleId_;
	/// Pointer to the navigation mesh we belong to.
	

	bool isEnabled_;
};