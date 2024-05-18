#pragma once
#include <memory>
#include <Physics/Physics.h>

class KinematicCharacterController {

public:

	KinematicCharacterController(btConvexShape* shape = nullptr, int collisionFilterGroup = 1, int collisionFilterMask = -1);
	virtual ~KinematicCharacterController();
	
	void debugDrawContacts();
	bool onGround() const;
	void jump(const Vector3f& jump = Vector3f::ZERO);
	void setLinearVelocity(const Vector3f& velocity);
	void setWalkDirection(const Vector3f& walkDir);
	void setUserPointer(void* userPointer);
	void setPosition(const Vector3f& position);

	const Vector3f& getPosition() const;
	const btVector3 getBtPosition() const;
	const Quaternion& getRotation() const;
	const btQuaternion getBtRotation() const;
	const Vector3f getLinearVelocity() const;
	void setTransform(const Vector3f& position, const Quaternion& rotation);
	const btTransform& getTransform() const;
	void getTransform(Vector3f& position, Quaternion& rotation);
	btPairCachingGhostObject* getPairCachingGhostObject() const;

private:

	void addKinematicToWorld();
	void releaseKinematic();
	void removeKinematicFromWorld();
	void applySettings(bool reapply = false);


	int m_colFilter;
	int m_colMask;
	float m_stepHeight;
	float m_maxJumpHeight;
	float m_jumpSpeed;
	float m_fallSpeed;
	float m_maxSlope;
	float m_linearDamping;
	float m_angularDamping;
	Vector3f m_gravity;

	btDiscreteDynamicsWorld* physicsWorld_;

	std::unique_ptr<btPairCachingGhostObject> m_pairCachingGhostObject;
	std::unique_ptr<btKinematicCharacterController> m_kinematicCharacterController;

	btGhostPairCallback* m_ghostPairCallback;
	btConvexShape* m_collisionShape;
	mutable Vector3f m_position;
	mutable Quaternion m_rotation;
	Vector3f m_colShapeOffset;
	bool m_reapplyAttributes;
};