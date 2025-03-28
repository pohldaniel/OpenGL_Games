#pragma once

#include "CrowdAgentEntity.h"

class Jack : public CrowdAgentEntity {

public:

	Jack(const CrowdAgent& crowdAgent, AnimationNode* animationNode);
	~Jack();

	void update(const float dt) override;

	void OnInactive() override;
	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	
	void playAnimation();
	const Vector3f& getPosition();
	
private:

	AnimationController *m_animationController;
};