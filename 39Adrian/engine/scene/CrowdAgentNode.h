#pragma once

#include "../Navigation/CrowdAgent.h"
#include "../BoundingBox.h"
#include "OctreeNode.h"

class CrowdAgentNode : public SceneNodeLC {

public:

	CrowdAgentNode(const CrowdAgent& crowdAgent);

	virtual void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) = 0;
	virtual void OnInactive() = 0;

	void setControlledNode(SceneNodeLC* controlledNode);

protected:

private:

	SceneNodeLC* m_controlledNode;
	const CrowdAgent& crowdAgent;
};