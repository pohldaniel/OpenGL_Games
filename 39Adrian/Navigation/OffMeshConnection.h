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
	OffMeshConnection(SceneNodeLC* node, SceneNodeLC* endPoint);
	OffMeshConnection(OffMeshConnection const& rhs);
	OffMeshConnection(OffMeshConnection&& rhs);
	OffMeshConnection& operator=(const OffMeshConnection& rhs);
	OffMeshConnection& operator=(OffMeshConnection&& rhs);
	virtual ~OffMeshConnection();

	void OnRenderDebug() const;

	void setEndPoint(SceneNodeLC* node);
	void setRadius(float radius);
	void setBidirectional(bool enabled);
	void setMask(unsigned int newMask);
	void setAreaID(unsigned int newAreaID);

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

	float m_radius;
	bool m_endPointDirty;
	unsigned int m_mask;
	unsigned int m_areaId;
	bool m_bidirectional;
	bool m_isEnabled;
};