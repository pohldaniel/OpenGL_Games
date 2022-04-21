#pragma once
#include <Box2D\Box2D.h>
#include <algorithm>
#include "Constants.h"

class RayCastClosestCallbackCS : public b2RayCastCallback {

public:
	RayCastClosestCallbackCS() {
		m_hit = false;
	}

	void reset() {
		m_hit = false;
		m_point.SetZero();
		m_normal.SetZero();
		m_velocity.SetZero();
		m_position.SetZero();
		m_fraction = 0.0f;
		m_platform = false;
		m_body = nullptr;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		uintptr_t index = fixture->GetUserData().pointer;
		if (index == 1) {
			return -1.0f;
		}

		if (index == 2) {
			m_platform = true;
			m_velocity = fixture->GetBody()->GetLinearVelocity();
			m_position = fixture->GetBody()->GetPosition();
			m_body = fixture->GetBody();
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
	b2Vec2 m_velocity;
	b2Vec2 m_position;
	float m_fraction;
	bool m_platform;
	b2Body* m_body = nullptr;
};

struct RaycastOriginsCS {
	b2Vec2 topLeft, topRight;
	b2Vec2 bottomLeft, bottomRight;
};

struct CastResultCS {
	b2Vec2 normal;
	float distance;

	void reset() {
		normal.SetZero();
		distance = 0.0f;
	}
};

struct CollisionInfoCS{
	enum CollisionFlags {
		None = 0,
		Front = 1,
		Bottom = 2,
		Back = 4,
		Top = 8,
		SteepPoly = 16,
		SlightPoly = 32,
		Jumping = 64,
		Platform = 128,
		DIR_FORCE_32BIT = 0x7FFFFFFF
	};
	
	unsigned long flags;
	float slopeAngle, slopeAngleOld;
	bool wasSlight;
	bool wasJumping;
	bool applyCollisionResponse;
	bool wasPlatform;

	void reset() {	
		wasSlight = flags & CollisionFlags::SlightPoly;
		wasJumping = flags & CollisionFlags::Jumping;
		wasPlatform = flags & CollisionFlags::Platform;
		
		slopeAngleOld = slopeAngle;
		slopeAngle = 0.0f;

		flags |= CollisionFlags::None;
		flags &= ~CollisionFlags::Back;
		flags &= ~CollisionFlags::Top;
		flags &= ~CollisionFlags::Bottom;
		flags &= ~CollisionFlags::Front;
		flags &= ~CollisionFlags::SteepPoly;
		flags &= ~CollisionFlags::SlightPoly;
		flags &= ~CollisionFlags::Jumping;
		flags &= ~CollisionFlags::Platform;
	}

	void printFlags() {
		std::cout << "None: " << (flags & CollisionFlags::None) << std::endl;
		std::cout << "Back: " << (flags & CollisionFlags::Back) << std::endl;
		std::cout << "Top: " << (flags & CollisionFlags::Top) << std::endl;
		std::cout << "Bottom: " << (flags & CollisionFlags::Bottom) << std::endl;
		std::cout << "Front: " << (flags & CollisionFlags::Front) << std::endl;
		std::cout << "SteepPoly: " << (flags & CollisionFlags::SteepPoly) << std::endl;
		std::cout << "Jumping: " << (flags & CollisionFlags::Jumping) << std::endl;
		std::cout << "Platform: " << (flags & CollisionFlags::Platform) << std::endl;
		std::cout << "################" << std::endl;
	}
};


//https://revolutionpoetry.com/collision-detection/
class CharacterControllerCS {

public:
	
	CharacterControllerCS(const float& dt, const float& fdt);
	~CharacterControllerCS();

	void render();
	void update();
	void fixedUpdate();

	void move(b2Vec2 velocity);

	void updateVelocity();
	
	b2Vec2 moveHorizontal(b2Vec2 position, b2Vec2 direction, unsigned int maxIterations);
	b2Vec2 moveVertical(b2Vec2 position, b2Vec2 direction, unsigned int maxIterations);
	b2Vec2 collisionResponse(b2Vec2 currentPosition, b2Vec2 initialTarget, b2Vec2 hitNormal, float friction, float bounciness);
	void updateRaycastOrigins(b2Vec2 position);

	float m_degrees = 0.0f;

	RayCastClosestCallbackCS m_callback;
	b2Body* m_body;

	b2Vec2 m_postion;
	b2Vec2 m_target;
	b2Vec2 m_velocity;
	b2Body* m_parentBody = nullptr;
	b2Vec2 m_postionD;
	b2Vec2 m_targetD;

	const float m_jumpHeight = 10 * 30.0f;
	const float m_timeToJumpApex = 0.5f;
	const float m_movementSpeed = 300.0f;
	const float m_maxClimbAngle = 60.0f;

	float m_gravity = 0.0f;
	float m_jumpVelocity = 0.0;

	float m_skinWidth = 0.1f;
	int m_horizontalRayCount = 32;
	int m_verticalRayCount = 32;
	float m_horizontalRaySpacing = 0.0f;
	float m_verticalRaySpacing = 0.0f;

	const float& m_fdt;
	const float& m_dt;

	RaycastOriginsCS raycastOrigins;
	CollisionInfoCS collisions;
	
	inline int sgn(float x) {
		if (x == 0)
			return 0;
		else
			return (x>0) ? 1 : -1;
	}

	inline b2Vec2 reflect(b2Vec2 direction, b2Vec2 normal) {
		return direction - 2.0f * b2Dot(direction, normal) * normal;
	}

	bool isGrounded(){
		return (collisions.flags & CollisionInfoCS::CollisionFlags::Bottom);
	}
};