#pragma once
#include <functional>
#include <Physics/Physics.h>
#include <engine/scene/Md2Node.h>
#include <Entities/Md2Entity.h>
#include "CrowdAgentEntity.h"

class Hero : public CrowdAgentEntity, public Md2Entity {
	
	struct TriggerCallback : public btCollisionWorld::ContactResultCallback {
		TriggerCallback() {}
		btScalar TriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;	
		std::function<void()> OnDeath = nullptr;
		bool tick = false;
	};

public:

	Hero(const Md2Model& md2Model, const CrowdAgent& crowdAgent);
	~Hero();

	void update(const float dt) override;
	void fixedUpdate(float fdt) override;

	void init(const Shape& shape);
	void OnInactive() override;
	void OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) override;
	btRigidBody* getRigidBody();
	btRigidBody* getSegmentBody();
	btRigidBody* getTriggerBody();
	void setRigidBody(btRigidBody* rigidBody);
	bool handleCollision(btCollisionObject* collisionObject);
	void setIsDeath(bool isDeath);
	bool isDeath();
	void setOnDeath(std::function<void()> fun);

private:
	
	btRigidBody *m_rigidBody, *m_segmentBody, *m_triggerBody;
	TriggerCallback m_triggerResult;
	bool m_isDeath;
};