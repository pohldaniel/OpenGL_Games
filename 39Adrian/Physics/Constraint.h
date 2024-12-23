#pragma once
#include <vector>
#include <memory>
#include <engine/Vector.h>
#include "Physics.h"

class btTypedConstraint;

enum ConstraintType{
		CONSTRAINT_POINT = 0,
		CONSTRAINT_HINGE,
		CONSTRAINT_SLIDER,
		CONSTRAINT_CONETWIST,
		CONSTRAINT_GENERIC6DOFSPRING2
};

class Constraint {
	

public:

	Constraint();
	~Constraint();

	virtual void OnSetEnabled();
	/// Return the depended on nodes to order network updates.
	//virtual void GetDependencyNodes(std::vector<Node*>& dest);
	/// Visualize the component as debug geometry.
	//virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);

	/// Set constraint type and recreate the constraint.
	void SetConstraintType(ConstraintType type);
	/// Set other body to connect to. Set to null to connect to the static world.
	void SetOtherBody(btRigidBody* body);
	/// Set constraint position relative to own body.
	void SetPosition(const Vector3f& position);
	/// Set constraint rotation relative to own body.
	void SetRotation(const Quaternion& rotation);
	/// Set constraint rotation relative to own body by specifying the axis.
	void SetAxis(const Vector3f& axis);
	/// Set constraint position relative to the other body. If connected to the static world, is a world space position.
	void SetOtherPosition(const Vector3f& position);
	/// Set constraint rotation relative to the other body. If connected to the static world, is a world space rotation.
	void SetOtherRotation(const Quaternion& rotation);
	/// Set constraint rotation relative to the other body by specifying the axis.
	void SetOtherAxis(const Vector3f& axis);
	/// Set constraint world space position. Resets both own and other body relative position, ie. zeroes the constraint error.
	void SetWorldPosition(const Vector3f& position);
	/// Set high limit. Interpretation is constraint type specific.
	void SetHighLimit(const Vector2f& limit);
	/// Set low limit. Interpretation is constraint type specific.
	void SetLowLimit(const Vector2f& limit);
	/// Set constraint error reduction parameter. Zero = leave to default.
	void SetERP(float erp);
	/// Set constraint force mixing parameter. Zero = leave to default.
	void SetCFM(float cfm);
	/// Set whether to disable collisions between connected bodies.
	void SetDisableCollision(bool disable);

	/// Return physics world.
	btDiscreteDynamicsWorld* GetPhysicsWorld() const { return physicsWorld_; }

	/// Return Bullet constraint.
	btTypedConstraint* GetConstraint() const { return constraint_; }

	/// Return constraint type.
	ConstraintType GetConstraintType() const { return constraintType_; }

	/// Return rigid body in own scene node.
	btRigidBody* GetOwnBody() const { return ownBody_; }

	/// Return the other rigid body. May be null if connected to the static world.
	btRigidBody* GetOtherBody() const { return otherBody_; }

	/// Return constraint position relative to own body.
	const Vector3f& GetPosition() const { return position_; }

	/// Return constraint rotation relative to own body.
	const Quaternion& GetRotation() const { return rotation_; }

	/// Return constraint position relative to other body.
	const Vector3f& GetOtherPosition() const { return otherPosition_; }

	/// Return constraint rotation relative to other body.
	const Quaternion& GetOtherRotation() const { return otherRotation_; }

	/// Return constraint world position, calculated from own body.
	Vector3f GetWorldPosition() const;

	/// Return high limit.
	const Vector2f& GetHighLimit() const { return highLimit_; }

	/// Return low limit.
	const Vector2f& GetLowLimit() const { return lowLimit_; }

	/// Return constraint error reduction parameter.
	float GetERP() const { return erp_; }

	/// Return constraint force mixing parameter.
	float GetCFM() const { return cfm_; }

	/// Return whether collisions between connected bodies are disabled.
	bool GetDisableCollision() const { return disableCollision_; }

	/// Release the constraint.
	void ReleaseConstraint();
	/// Apply constraint frames.
	void ApplyFrames();

protected:
	/// Handle node being assigned.
	//virtual void OnNodeSet(Node* node);
	/// Handle scene being assigned.
	//virtual void OnSceneSet(Scene* scene);
	/// Handle node transform being dirtied.
	//virtual void OnMarkedDirty(Node* node);

protected:
	/// Create the constraint.
	virtual void CreateConstraint();
	/// Apply high and low constraint limits.
	virtual void ApplyLimits();

	/// Physics world.
	btDiscreteDynamicsWorld* physicsWorld_;
	/// Own rigid body.
	btRigidBody* ownBody_;
	/// Other rigid body.
	btRigidBody* otherBody_;
	/// Bullet constraint.
	btTypedConstraint* constraint_;
	/// Constraint type.
	ConstraintType constraintType_;
	/// Constraint position.
	Vector3f position_;
	/// Constraint rotation.
	Quaternion rotation_;
	/// Constraint other body position.
	Vector3f otherPosition_;
	/// Constraint other body axis.
	Quaternion otherRotation_;
	/// Cached world scale for determining if the constraint position needs update.
	Vector3f cachedWorldScale_;
	/// High limit.
	Vector2f highLimit_;
	/// Low limit.
	Vector2f lowLimit_;
	/// Error reduction parameter.
	float erp_;
	/// Constraint force mixing parameter.
	float cfm_;
	/// Other body node ID for pending constraint recreation.
	unsigned otherBodyNodeID_;
	/// Disable collision between connected bodies flag.
	bool disableCollision_;
	/// Recreate constraint flag.
	bool recreateConstraint_;
	/// Coordinate frames dirty flag.
	bool framesDirty_;
	/// Constraint creation retry flag if attributes initially set without scene.
	bool retryCreation_;
};


