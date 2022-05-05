#pragma once
#include <Box2D\Box2D.h>
#include <cmath>
#include <vector>
#include <iterator>
#include "Constants.h"

#define SHAPE_BOX 1
#define SHAPE_CIRCLE 0

class RayCastClosestCallbackCS : public b2RayCastCallback {

public:

	RayCastClosestCallbackCS() {
		m_hit = false;
		m_count = 0;
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
		m_count = 0;
	}

	void resetBody() {
		m_body = nullptr;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		uintptr_t index = fixture->GetUserData().pointer;
		if (index == 1) {
			return -1.0f;
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
	int32 m_count;
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

struct CollisionInfoCS {
	enum CollisionFlags {
		None = 0,
		Front = 1,
		Bottom = 2,
		Back = 4,
		Top = 8,
		SteepPoly = 16,
		SlightPoly = 32,
		Platform = 64,
		PlatformTop = 128,
		PlatformBottom = 512,
		DIR_FORCE_32BIT = 0x7FFFFFFF
	};

	unsigned long flags;
	float slopeAngle;

	bool wasSlight;
	bool wasSteep;
	bool wasSlope;

	unsigned short guardPlatform = 0;
	unsigned short platformToSlight = 0;

	void reset() {		
		guardPlatform = guardPlatform ? guardPlatform - 1 : 0;
		platformToSlight = platformToSlight ? platformToSlight - 1 : 0;
		
		wasSlope = (flags & CollisionFlags::SlightPoly) | (flags & CollisionFlags::SteepPoly);
		wasSlight = flags & CollisionFlags::SlightPoly;
		wasSteep = flags & CollisionFlags::SteepPoly;
		slopeAngle = 0.0f;

		flags |= CollisionFlags::None;
		flags &= ~CollisionFlags::Back;
		flags &= ~CollisionFlags::Top;
		flags &= ~CollisionFlags::Bottom;
		flags &= ~CollisionFlags::Front;
		flags &= ~CollisionFlags::SteepPoly;
		flags &= ~CollisionFlags::SlightPoly;
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
		std::cout << "PlatformVer: " << (flags & CollisionFlags::Platform) << std::endl;
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
	b2Vec2 moveVertical(b2Vec2 position, b2Vec2 direction, unsigned int maxIterations, bool reset = true);
	b2Vec2 collisionResponse(b2Vec2 currentPosition, b2Vec2 initialTarget, b2Vec2 hitNormal, float friction, float bounciness);
	

	RayCastClosestCallbackCS m_callback;
	b2Body* m_body;

	b2Vec2 m_position;
	b2Vec2 m_size = b2Vec2(30.0f, 60.0f);
	b2Vec2 m_target;
	b2Vec2 m_velocity;
	b2Vec2 m_velocityParent;
	b2Body* m_parentBody = nullptr;
	b2Vec2 m_postionD;
	b2Vec2 m_targetD;

	std::vector<b2Vec2> positionsLeft;
	std::vector<b2Vec2> positionsRight;
	std::vector<b2Vec2> positionsBottom;
	std::vector<b2Vec2> positionsTop;

	const float m_jumpHeight = 10 * 30.0f;
	const float m_timeToJumpApex = 0.5f;
	const float m_movementSpeed = 300.0f;
	const float m_maxClimbAngle = 80.0f;

	float m_gravity = 0.0f;
	float m_jumpVelocity = 0.0;

	float m_skinWidth = 0.1f;
	int m_horizontalRayCount = static_cast<int>(m_size.y) + 2;
	int m_verticalRayCount = static_cast<int>(m_size.x) + 10;
	//int m_verticalRayCount = 64;
	float m_horizontalRaySpacing = 0.0f;
	float m_verticalRaySpacing = 0.0f;

	const float& m_fdt;
	const float& m_dt;

	RaycastOriginsCS raycastOrigins;
	CollisionInfoCS collisions;

	unsigned long transitions;
	enum CharacterTransition {
		NONE = 0,
		PLATFORM_TO_SLIGHT_SLOPE = 1,
		PLATFORM_TO_STEEP_SLOPE = 2,
		PLATFORM_UP_TO_STEEP_SLOPE = 4,
		SLIGHT_SLOPE_TO_PLATFORM = 8,
		STEEP_SLOPE_TO_PLATFORM = 16,
		JUMP_FROM_PLATFORM = 32,
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
		transitions &= ~CharacterTransition::PLATFORM_UP_TO_STEEP_SLOPE;
		transitions &= ~CharacterTransition::SLIGHT_SLOPE_TO_PLATFORM;
		transitions &= ~CharacterTransition::STEEP_SLOPE_TO_PLATFORM;
		transitions &= ~CharacterTransition::JUMP_FROM_PLATFORM;
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