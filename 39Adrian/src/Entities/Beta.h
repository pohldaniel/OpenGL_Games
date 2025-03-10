#pragma once

#include "CrowdAgentEntity.h"

class Beta : public CrowdAgentEntity {

public:

	Beta(const CrowdAgent& crowdAgent, AnimationNode* animationNode);
	~Beta();

	void update(const float dt) override;

	void OnInactive() override;
	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	void setUpdateSilent(bool value);

private:

	AnimationController *m_animationController;
};