#pragma once

#include "../Navigation/CrowdAgent.h"
#include "../BoundingBox.h"
#include "../Object.h"

class SceneNodeLC;

class CrowdAgentObject : public Object {

public:

	CrowdAgentObject(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode);

	virtual void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) = 0;
	virtual void OnInactive() = 0;
	virtual void OnTarget(const Vector3f& pos) = 0;

	void setControlledNode(SceneNodeLC* controlledNode);
	void setPosition(const Vector3f& position) override;
	void setOrientation(const Vector3f &directionXZ) override;

protected:

private:

	SceneNodeLC* m_controlledNode;
	const CrowdAgent& crowdAgent;
};