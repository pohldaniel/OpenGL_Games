#pragma once

#include <engine/scene/Md2Node.h>
#include <Entities/Md2Entity.h>
#include "CrowdAgentEntity.h"

class Hero : public CrowdAgentEntity, public Md2Entity {

public:

	Hero(const Md2Model& md2Model, const CrowdAgent& crowdAgent);
	~Hero();

	void update(const float dt) override;
	void fixedUpdate(float fdt);

	void OnInactive() override;
	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	//void OnTarget(const Vector3f& targetPos) override;
	btRigidBody* getRigidBody();
	void setRigidBody(btRigidBody* rigidBody);

private:

	btRigidBody* m_rigidBody;
};