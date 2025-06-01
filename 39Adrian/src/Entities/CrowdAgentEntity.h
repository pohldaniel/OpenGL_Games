#pragma once

#include <engine/animationNew/AnimationController.h>
#include <engine/objects/CrowdAgentObject.h>
#include <Entities/Entity.h>
#include "EmptyAgentEntity.h"

class CrowdAgentEntity : public EmptyAgentEntity {

public:

	CrowdAgentEntity(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode);
	virtual ~CrowdAgentEntity();

	virtual void update(const float dt) = 0;

	virtual void OnInactive() override;
	virtual void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	virtual void OnTarget(const Vector3f& targetPos) override;
	virtual void OnAddAgent(const Vector3f& pos) override;
	void resetAgent();
};