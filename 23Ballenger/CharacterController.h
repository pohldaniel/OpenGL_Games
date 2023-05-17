#pragma once

#include <btBulletDynamicsCommon.h>

class CharacterController {
	public:

	CharacterController();
	~CharacterController();

	void create(btRigidBody* rigidBody, btDynamicsWorld* physicsWorld, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* rigidBodyUserPointer = NULL);
	void destroy();

	void setSlopeAngle(float degrees);
	void setJumpDistanceOffset(float value);
	void setOnGroundDistanceOffset(float value);

	void preStep(); // Call before the physics are stepped.
	void postStep(); // Call after the physics are stepped.

	bool onGround() const;
	void jump(const btVector3& direction, float force);
	void applyCentralImpulse(const btVector3& direction);

	const btVector3& getLinearVelocity();
	const float getLinearVelocityY();
	void getWorldTransform(btTransform& transform);
	btRigidBody* getRigidBody() const;
	btCollisionShape* getCollisionShape() const;

	void setLinearVelocity(const btVector3& vel);
	void setAngularVelocity(const btVector3& angVel);
	void setLinearVelocityXZ(const btVector3& vel);
	void setAngularFactor(const btVector3& angularFactor);
	void setSleepingThresholds(float linear, float angular);
	void setRollingFriction(float rollingFriction);
	void setDamping(float linear, float angular);
	void setLinearFactor(const btVector3& linearFactor);
	void setGravity(const btVector3& gravity);
	
private:

	void moveCharacterAlongY(float step);

	btCollisionWorld* m_collisionWorld;
	btRigidBody* m_rigidBody;
	btCollisionShape* m_shape;
		
	btVector3 m_slopeNormal;

	float m_maxClimbSlopeAngle;
	float m_slopeDot;
	float m_jumpDistanceOffset;
	float m_onGroundDistanceOffset;

	bool m_onGround;
	bool m_onSteepSlope;
	bool m_falling;
	bool m_canJump;

	bool movingUpward = true;
	unsigned short m_jumpTicks = 0;
	unsigned short m_groundTicks = 0;
	float getVelocityWeight(float sloapDot);
	unsigned short getJumpTicks(float sloapDot);
};