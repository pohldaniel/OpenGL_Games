#pragma once
#include <memory>
#include <Physics/Physics.h>


class btPairCachingGhostObject;
class btKinematicCharacterController;
class btDiscreteDynamicsWorld;
//=============================================================================
//=============================================================================
class CharacterController {


public:
	CharacterController();
	virtual ~CharacterController();

	void AddKinematicToWorld();
	void ReleaseKinematic();
	void RemoveKinematicFromWorld();
	void ApplySettings(bool reapply = false);
	void DebugDrawContacts();

	const Vector3f& GetPosition();
	const Quaternion& GetRotation();
	void SetTransform(const Vector3f& position, const Quaternion& rotation);
	btTransform& GetTransform();
	void GetTransform(Vector3f& position, Quaternion& rotation);
	void moveAlongY(float step);
	void setUserPointer(void* userPointer);

	void SetWalkDirection(const Vector3f& walkDir);
	bool OnGround() const;
	void Jump(const Vector3f& jump = Vector3f::ZERO);


	int colFilter_;
	int colMask_;
	float stepHeight_;
	float maxJumpHeight_;
	float jumpSpeed_;
	float fallSpeed_;
	float maxSlope_;
	float linearDamping_;
	float angularDamping_;
	Vector3f gravity_;

	btDiscreteDynamicsWorld* physicsWorld_;
	std::unique_ptr<btPairCachingGhostObject> pairCachingGhostObject_;
	std::unique_ptr<btKinematicCharacterController> kinematicController_;
	btGhostPairCallback* m_ghostPairCallback;

	Vector3f position_;
	Quaternion rotation_;
	Vector3f colShapeOffset_;
	bool reapplyAttributes_;
};