#pragma once
#include <memory>
#include "Physics.h"
#include "turso3d/Object/ObjectTu.h"
#include "turso3d/Math/Vector3.h"
#include "turso3d/Math/Quaternion.h"

class btPairCachingGhostObject;
class btKinematicCharacterController;
class btDiscreteDynamicsWorld;
//=============================================================================
//=============================================================================
class KinematicCharacterController : public ObjectTu
{
	OBJECT(KinematicCharacterController);

public:
	KinematicCharacterController();
	virtual ~KinematicCharacterController();

	void AddKinematicToWorld();
	void ReleaseKinematic();
	void RemoveKinematicFromWorld();
	void ApplySettings(bool reapply = false);
	void DebugDrawContacts();

	const Vector3& GetPosition();
	const QuaternionTu& GetRotation();
	void SetTransform(const Vector3& position, const QuaternionTu& rotation);
	void GetTransform(Vector3& position, QuaternionTu& rotation);
	void moveAlongY(float step);

	void SetWalkDirection(const Vector3& walkDir);
	bool OnGround() const;
	void Jump(const Vector3 &jump = Vector3::ZERO);

protected:
	int colFilter_;
	int colMask_;
	float stepHeight_;
	float maxJumpHeight_;
	float jumpSpeed_;
	float fallSpeed_;
	float maxSlope_;
	float linearDamping_;
	float angularDamping_;
	Vector3 gravity_;

	btDiscreteDynamicsWorld* physicsWorld_;
	std::unique_ptr<btPairCachingGhostObject> pairCachingGhostObject_;
	std::unique_ptr<btKinematicCharacterController> kinematicController_;
	btGhostPairCallback* m_ghostPairCallback;

	Vector3 position_;
	QuaternionTu rotation_;
	Vector3 colShapeOffset_;
	bool reapplyAttributes_;
};