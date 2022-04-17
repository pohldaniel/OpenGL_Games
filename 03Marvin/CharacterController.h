#pragma once
#include <Box2D\Box2D.h>
#include <cmath>
#include "Constants.h"

class RayCastClosestCallback : public b2RayCastCallback{

public:
	RayCastClosestCallback(){
		m_hit = false;
	}

	void reset() {
		m_hit = false;
		m_point.SetZero();
		m_normal.SetZero();
		m_fraction = 0.0f;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override{
		uintptr_t index = fixture->GetUserData().pointer;
		if (index == 1){			
			return -1.0f;
		}

		m_hit = true;
		m_point = point;
		m_normal = normal;
		m_fraction = fraction;
		return fraction;
	}

	bool m_hit;
	b2Vec2 m_point;
	b2Vec2 m_normal;
	float m_fraction;
};

struct RaycastOrigins {
	b2Vec2 topLeft, topRight;
	b2Vec2 bottomLeft, bottomRight;
};

struct CollisionInfo {
	bool above, below;
	bool left, right;
	bool climbingSlope, descendingSlope;
	float slopeAngle, slopeAngleOld;
	b2Vec2 velocityOld;

	void reset() {
		above = false;
		below = false;
		left = false;
		right = false;
		climbingSlope = false;
		descendingSlope = false;
		slopeAngleOld = slopeAngle;
		slopeAngle = 0.0f;
		velocityOld.SetZero();
	}
};

//https://revolutionpoetry.com/collision-detection/
class CharacterController{

public:
	CharacterController(const float& dt, const float& fdt);
	~CharacterController();

	void render();
	void update();
	void fixedUpdate();

	void move(b2Vec2 velocity);

	void updateVelocity();
	void verticalCollision(b2Vec2& velocity);
	void horizontalCollision(b2Vec2& velocity);

	float m_degrees = 0.0f;

	RayCastClosestCallback m_callback;
	b2Body* m_body;

	b2Vec2 m_postion;
	b2Vec2 m_target;
	b2Vec2 m_velocity;

	b2Vec2 m_postionD;
	b2Vec2 m_targetD;
	
	const float m_jumpHeight = 6 * 30.0f;
	const float m_timeToJumpApex = 0.5f;
	const float m_movementSpeed = 300.0f;
	const float m_maxClimbAngle = 80.0f;
	const float m_maxDescendAngle = 75.0f;

	float m_gravity = 0.0f;
	float m_jumpVelocity = 0.0;

	bool m_grounded = false;

	float m_skinWidth = 0.2f;
	int m_horizontalRayCount = 2;
	int m_verticalRayCount = 2;
	float m_horizontalRaySpacing = 0.0f;
	float m_verticalRaySpacing = 0.0f;

	const float& m_fdt;
	const float& m_dt;

	RaycastOrigins raycastOrigins;
	CollisionInfo collisions;

	void updateRaycastOrigins();
	void climbSlope(b2Vec2& velocity, float slopeAngle);
	void descendSlope(b2Vec2& velocity);

	inline int sgn(float x){
		if (x == 0)
			return 0;
		else
			return (x>0) ? 1 : -1;
	}
};