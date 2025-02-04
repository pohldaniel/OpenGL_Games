#pragma once

#include <engine/animationNew/AnimationController.h>
#include <engine/objects/CrowdAgentObject.h>
#include <Entities/Entity.h>

class EmptyAgentEntity : public CrowdAgentObject, public Entity {

public:

	EmptyAgentEntity(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode);
	virtual ~EmptyAgentEntity();

	virtual void OnInactive() override;
	virtual void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	virtual void OnTarget(const Vector3f& targetPos) override;
	virtual void OnAddAgent(const Vector3f& targetPos) override;
};