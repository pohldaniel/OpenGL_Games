#pragma once

#include <engine/scene/Md2Node.h>
#include "CrowdAgentEntity.h"

class Hero : public CrowdAgentEntity {

public:

	Hero(const CrowdAgent& crowdAgent, Md2Node* md2Node);
	~Hero();

	void update(const float dt) override;

	void OnInactive() override;
	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	//void OnTarget(const Vector3f& targetPos) override;

private:

	Md2Node *m_md2Node;
};