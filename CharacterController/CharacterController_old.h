#pragma once
#include <Box2D\Box2D.h>
#include "engine\Extension.h"
#include "Constants.h"

class RayCastClosestCallbackOld : public b2RayCastCallback {

public:
	RayCastClosestCallbackOld() {
		m_hit = false;
	}

	void reset() {
		m_hit = false;
		m_point.SetZero();
		m_normal.SetZero();
		m_fraction = 0.0f;
	}

	float ReportFixture(b2Fixture* fixture, const b2Vec2& point, const b2Vec2& normal, float fraction) override {
		uintptr_t index = fixture->GetUserData().pointer;
		if (index == 1) {
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

//https://revolutionpoetry.com/collision-detection/
class CharacterControllerOld {

public:
	CharacterControllerOld(const float& dt, const float& fdt);
	~CharacterControllerOld();

	void render();
	void update();
	void fixedUpdate();

	void move();

	float m_degrees = 0.0f;

	RayCastClosestCallbackOld m_callback;
	b2Body* m_body;

	b2Vec2 m_postion;
	b2Vec2 m_target;
	b2Vec2 m_velocity;

	float m_distance = 15.0f;
	float m_skinWidth = 15.0f;

	const float m_movementSpeed = 300.0f;
	const float m_jumpHeight = 800;
	const float m_gravity = 1600.0f;
	bool m_grounded = false;

	const float& m_fdt;
	const float& m_dt;
};