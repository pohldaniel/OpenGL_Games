#pragma once

#include <engine/animationNew/AnimationController.h>
#include <engine/scene/CrowdAgentNode.h>
#include <Entities/Entity.h>

class CrowdAgentEntity : public CrowdAgentNode, public Entity  {

public:

	CrowdAgentEntity(const CrowdAgent& crowdAgent, AnimationNode* animationNode);
	

	void draw() override;
	void update(const float dt) override;
	void OnInactive() override;

	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	AnimationController *m_animationController;
	//AnimationNode* m_animationNode;
};