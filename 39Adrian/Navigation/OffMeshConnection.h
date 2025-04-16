#pragma once

#include <memory>
#include "../engine/scene/SceneNodeLC.h"
#include "../engine/DebugRenderer.h"


class OffMeshConnection{

	friend class NavigationMesh;
	friend class DynamicNavigationMesh;
	friend struct MeshProcess;

public:

	OffMeshConnection(SceneNodeLC* node);
	virtual ~OffMeshConnection();

	void OnRenderDebug();

	void setEndPoint(SceneNodeLC* node);
	void setRadius(float radius);
	void setBidirectional(bool enabled);
	void setMask(unsigned newMask);
	void setAreaID(unsigned newAreaID);

	SceneNodeLC* getEndPoint() const;
	SceneNodeLC* getNode() const;

	float getRadius() const;
	unsigned getMask() const;
	unsigned getAreaID() const;
	bool isBidirectional() const;
	bool isEnabled() const;

private:

	SceneNodeLC* m_node;
	SceneNodeLC* m_endPoint;

	unsigned m_endPointID;
	float m_radius;
	bool m_endPointDirty;
	unsigned m_mask;
	unsigned m_areaId;
	bool m_bidirectional;
	bool m_isEnabled;
};