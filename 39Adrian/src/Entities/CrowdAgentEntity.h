#pragma once

#include <engine/animationNew/AnimationController.h>
#include <engine/objects/CrowdAgentObject.h>
#include <Entities/Entity.h>

class CrowdAgentEntity : public CrowdAgentObject, public Entity  {

public:

	CrowdAgentEntity(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode);
	
	virtual void draw() override = 0;
	void update(const float dt) override;

	virtual void OnInactive() override;
	virtual void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	virtual void OnTarget(const Vector3f& targetPos) override;
};