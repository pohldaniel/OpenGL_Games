#pragma once

#include "CrowdAgentEntity.h"

class Beta : public CrowdAgentEntity {

public:

	Beta(const CrowdAgent& crowdAgent, AnimationNode* animationNode);


	void draw() override;
	void update(const float dt) override;

	void OnInactive() override;
	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;

	AnimationController *m_animationController;
};