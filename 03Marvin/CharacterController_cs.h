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
		m_position.SetZero();
		m_fraction = 0.0f;
		m_platformHor = false;
		m_platformVer = false;
		//m_body = nullptr;
	}

	void resetBody() {
		m_body = nullptr;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		uintptr_t index = fixture->GetUserData().pointer;
		if (index == 1) {
			return -1.0f;
		}

		if (index == 2) {
			m_platformHor = true;
			m_position = fixture->GetBody()->GetPosition();
			m_body = fixture->GetBody();
		}

		if (index == 3) {
			m_platformVer = true;
			m_position = fixture->GetBody()->GetPosition();
			m_body = fixture->GetBody();
		}

		m_hit = true;

		m_fraction = fraction;
		m_normal = normal;
		m_point = point;

		return fraction;
	}

	bool m_hit;
	b2Vec2 m_point;
	b2Vec2 m_normal;
	b2Vec2 m_position;
	float m_fraction;
	bool m_platformHor;
	bool m_platformVer;
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
		PlatformHor = 128,
		PlatformVer = 256,
		PlatformTop = 512,
		PlatformBottom= 1024,
		DIR_FORCE_32BIT = 0x7FFFFFFF
	};
	
	unsigned long flags;
	float slopeAngle, slopeAngleOld;

	bool wasSlight;
	bool wasSteep;
	bool wasJumping;
	bool applyCollisionResponse;
	bool wasPlatform;
	bool wasPlatformHor;
	bool wasPlatformVer;
	bool wasSlope;
	bool wasPlatformTop;
	bool wasPlatformBottom;
	bool moveUp;

	unsigned short guardSlight = 0;
	unsigned short guardSteep = 0;
	unsigned short guardPlatform = 0;

	short platformCount = 0;

	void reset() {	
		

		guardSlight = guardSlight ? guardSlight - 1 : 0;
		guardSteep = guardSteep ? guardSteep - 1 : 0;
		guardPlatform = guardPlatform ? guardPlatform - 1 : 0;

		wasJumping = flags & CollisionFlags::Jumping;
		wasSlope = (flags & CollisionFlags::SlightPoly) | (flags & CollisionFlags::SteepPoly);
		wasPlatformHor = flags & CollisionFlags::PlatformHor;
		wasPlatformVer = flags & CollisionFlags::PlatformVer;
		wasSlight = flags & CollisionFlags::SlightPoly;
		wasSteep = flags & CollisionFlags::SteepPoly;
		wasPlatformTop = flags & CollisionFlags::PlatformTop;
		wasPlatformBottom = flags & CollisionFlags::PlatformBottom;
		slopeAngle = 0.0f;

		flags |= CollisionFlags::None;
		flags &= ~CollisionFlags::Back;
		flags &= ~CollisionFlags::Top;
		flags &= ~CollisionFlags::Bottom;
		flags &= ~CollisionFlags::Front;
		flags &= ~CollisionFlags::SteepPoly;
		flags &= ~CollisionFlags::SlightPoly;
		flags &= ~CollisionFlags::Jumping;
		flags &= ~CollisionFlags::PlatformHor;
		flags &= ~CollisionFlags::PlatformTop;
		flags &= ~CollisionFlags::PlatformBottom;
	}

	void printFlags() {
		std::cout << "None: " << (flags & CollisionFlags::None) << std::endl;
		std::cout << "Back: " << (flags & CollisionFlags::Back) << std::endl;
		std::cout << "Top: " << (flags & CollisionFlags::Top) << std::endl;
		std::cout << "Bottom: " << (flags & CollisionFlags::Bottom) << std::endl;
		std::cout << "Front: " << (flags & CollisionFlags::Front) << std::endl;
		std::cout << "SteepPoly: " << (flags & CollisionFlags::SteepPoly) << std::endl;
		std::cout << "SlightPoly: " << (flags & CollisionFlags::SlightPoly) << std::endl;
		std::cout << "Jumping: " << (flags & CollisionFlags::Jumping) << std::endl;
		std::cout << "PlatformHor: " << (flags & CollisionFlags::PlatformHor) << std::endl;
		std::cout << "PlatformVer: " << (flags & CollisionFlags::PlatformVer) << std::endl;
		std::cout << "PlatformTop: " << (flags & CollisionFlags::PlatformTop) << std::endl;
		std::cout << "PlatformBottom: " << (flags & CollisionFlags::PlatformBottom) << std::endl;
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
	b2Vec2 moveVertical(b2Vec2 position, b2Vec2 direction, unsigned int maxIterations, bool reverse = false);
	b2Vec2 collisionResponse(b2Vec2 currentPosition, b2Vec2 initialTarget, b2Vec2 hitNormal, float friction, float bounciness);
	void updateRaycastOrigins(b2Vec2 position);

	float m_degrees = 0.0f;

	RayCastClosestCallbackCS m_callback;
	b2Body* m_body;

	b2Vec2 m_postion;
	b2Vec2 m_target;
	b2Vec2 m_velocity;
	b2Vec2 m_velocityParent;
	b2Body* m_parentBody = nullptr;
	b2Vec2 m_postionD;
	b2Vec2 m_targetD;

	const float m_jumpHeight = 10 * 30.0f;
	const float m_timeToJumpApex = 0.5f;
	const float m_movementSpeed = 300.0f;
	const float m_maxClimbAngle = 50.0f;

	float m_gravity = 0.0f;
	float m_jumpVelocity = 0.0;

	float m_skinWidth = 0.1f;
	int m_horizontalRayCount = 32;
	int m_verticalRayCount = 32;
	float m_horizontalRaySpacing = 0.0f;
	float m_verticalRaySpacing = 0.0f;
	bool _moveVertical = true;
	float m_parentVelocity = 0.0f;

	const float& m_fdt;
	const float& m_dt;

	RaycastOriginsCS raycastOrigins;
	CollisionInfoCS collisions;

	unsigned long transitions;
	enum CharacterTransition {
		NONE = 0,
		PLATFORM_TO_SLIGHT_SLOPE = 1,
		PLATFORM_TO_STEEP_SLOPE = 2,
		SLIGHT_SLOPE_TO_PLATFORM = 4,
		STEEP_SLOPE_TO_PLATFORM = 8,
		DIR_FORCE_32BIT = 0x7FFFFFFF
	};

	void printTransitions() {
		std::cout << "None: " << (transitions & CharacterTransition::NONE) << std::endl;
		std::cout << "PLATFORM_TO_SLIGHT_SLOPE: " << (transitions & CharacterTransition::PLATFORM_TO_SLIGHT_SLOPE) << std::endl;
		std::cout << "PLATFORM_TO_STEEP_SLOPE: " << (transitions & CharacterTransition::PLATFORM_TO_STEEP_SLOPE) << std::endl;
		std::cout << "SLIGHT_SLOPE_TO_PLATFORM: " << (transitions &CharacterTransition::SLIGHT_SLOPE_TO_PLATFORM) << std::endl;
		std::cout << "SLIGHT_SLOPE_TO_PLATFORM: " << (transitions &CharacterTransition::STEEP_SLOPE_TO_PLATFORM) << std::endl;
		std::cout << "################" << std::endl;
	}

	void resetTransitions() {
		transitions |= CharacterTransition::NONE;
		transitions &= ~CharacterTransition::PLATFORM_TO_SLIGHT_SLOPE;
		transitions &= ~CharacterTransition::PLATFORM_TO_STEEP_SLOPE;
		transitions &= ~CharacterTransition::SLIGHT_SLOPE_TO_PLATFORM;
		transitions &= ~CharacterTransition::STEEP_SLOPE_TO_PLATFORM;
	}

	inline int sgn(float x) {
		if (x == 0)
			return 0;
		else
			return (x>0) ? 1 : -1;
	}

	inline b2Vec2 reflect(b2Vec2 direction, b2Vec2 normal) {
		return direction - 2.0f * b2Dot(direction, normal) * normal;
	}

	bool isGrounded() {
		return (collisions.flags & CollisionInfoCS::CollisionFlags::Bottom);
	}
};