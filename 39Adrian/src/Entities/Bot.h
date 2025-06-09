#pragma once
#include <Entities/Md2Entity.h>
#include <Physics/Physics.h>

enum EnemyType {
	HERO = 0,
	CORPSE = 1,
	MUTANT_CHEETA = 2,
	MUTANT_LIZARD = 3,
	MUTANT_MAN = 4,
	RIPPER = 5,
	SKEL = 6
};

class Bot : public Md2Entity {

	struct TriggerCallback : public btCollisionWorld::ContactResultCallback {
		TriggerCallback() {}
		btScalar TriggerCallback::addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) override;
		std::function<void()> OnDeath = nullptr;
	};

public:

	Bot(const Md2Model& md2Model);
	~Bot();

	void update(const float dt) override;
	void fixedUpdate(float fdt) override;

	void init(const Shape& shape);
	void setStart(const Vector3f& start);
	void setEnd(const Vector3f& end);
	void setStart(const float x, const float y, const float z);
	void setEnd(const float x, const float y, const float z);
	void setMoveSpeed(const float moveSpeed);
	void handleCollision(btCollisionObject* collisionObject);
	btRigidBody* getRigidBody();
	btRigidBody* getSegmentBody();
	void setSegmentBody(btRigidBody* segmentBody);
	btRigidBody* getTriggerBody();
	void setRigidBody(btRigidBody* rigidBody);
	void setEnemyType(EnemyType enemyType);
	EnemyType getEnemyType();
	void setIsInRange(bool isInRange);
	bool isInRange();
	void death();
	bool isDeath();

private:
	
	void followPath(float dt);

	Vector3f m_start, m_end, m_currentWaypoint;
	float m_moveSpeed;
	btRigidBody *m_rigidBody, *m_segmentBody, *m_triggerBody;
	EnemyType m_enemyType;
	TriggerCallback m_triggerResult;
	bool m_isInRange, m_isDeath;
};