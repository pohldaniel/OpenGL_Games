#ifndef _BASIS3STEPCHARACTERCONTROLLER_H_INCLUDED_
#define _BASIS3STEPCHARACTERCONTROLLER_H_INCLUDED_

#include <btBulletDynamicsCommon.h>
#include "engine/Vector.h"

// An implementation of a "3 step" character controller for Bullet, based on ideas described here: http://dev-room.blogspot.fi/2015/03/some-example-works-like.html

// The idea is that the rigid body goes through 3 steps every update:
// 1. Move the character up a certain amount.
// 2. Move the character based on input and let Bullet resolve collisions normally (ie. step the simulation).
// 3. Do a ray or convex test downwards to find any objects that the character can stand on (ie. the ground) and move the character back down.

class DynamicCharacterController {
	public:

		DynamicCharacterController();
		~DynamicCharacterController();

		void create(btMotionState* motionState, btDynamicsWorld* physicsWorld, float mass, float radius, float height, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* rigidBodyUserPointer = NULL);
		void create(btRigidBody* rigidBody, btDynamicsWorld* physicsWorld, int collisionFilterGroup = 1, int collisionFilterMask = -1, void* rigidBodyUserPointer = NULL);
		void destroy();

		void setParameters(float maxClimbSlopeAngle);
		void setSlopeAngle(float degrees);
		void setDistanceOffset(float value);
		void setStepHeight(float value);

		void preStep(); // Call before the physics are stepped.
		void postStep(); // Call after the physics are stepped.
		bool isStepping() const;

		bool onGround() const;
		void jump(const btVector3& direction, float force);

		void setMovementXZ(const Vector2f& movementVector);
		void setMovementXYZ(const btVector3& movementVector);

		void setLinearVelocity(const btVector3& vel);
		const btVector3& getLinearVelocity();

		btRigidBody* getRigidBody() const { return mRigidBody; }
		btCollisionShape* getCollisionShape() const { return mShape; }


		void moveCharacterAlongY(float step);

		btCollisionWorld* mCollisionWorld;
		btRigidBody* mRigidBody;
		btCollisionShape* mShape;
		bool mOnGround;
		bool mOnSteepSlope;
		btVector3 mSlopeNormal;
		float mStepHeight;
		float mMaxClimbSlopeAngle;
		float mDistanceOffset;
		bool mIsStepping;
		

		float mCharacterMovementX;
		float mCharacterMovementZ;

		float mCharacterMovementY;


		int mCollisionFilterGroup;
		int mCollisionFilterMask;

		bool movingUpward = true;
};
#endif // _BASIS3STEPCHARACTERCONTROLLER_H_INCLUDED_