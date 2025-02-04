#pragma once

#include "CrowdAgentEntity.h"

class Woman : public CrowdAgentEntity {

public:

	Woman(const CrowdAgent& crowdAgent, AnimationNode* animationNode);
	~Woman();

	void update(const float dt) override;

	void OnInactive() override;
	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;

	void setUpdateSilent(bool value);

	AnimationController *m_animationController;
};