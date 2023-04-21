#pragma once
#include "engine/Vector.h"

class ClothParticle {

public:
	ClothParticle();
	ClothParticle(const Vector3f& pos);
	~ClothParticle();

	void VerletIntegration();
	void AddForce(const Vector3f& force)			{ m_acceleration += force / m_mass; }
	Vector3f& GetPos()						{ return m_pos; }
	Vector3f& GetNormal()					{ return m_accumulatedNormal; }
	void ZeroAcceleration()					{ m_acceleration = Vector3f(0.0f, 0.0f, 0.0f); }
	void ZeroNormal()						{ m_accumulatedNormal = Vector3f(0.0f, 0.0f, 0.0f); }
	void OffsetPos(const Vector3f& v)		{ if (m_movable) m_pos += v; }
	void AddToNormal(Vector3f& normal)		{ m_accumulatedNormal += Vector3f::Normalize(normal); }
	void Pin()								{ m_movable = false; }

private:
	float m_damping, m_timeStep;
	bool m_movable;
	float m_mass;
	Vector3f m_pos, m_oldPos, m_acceleration, m_accumulatedNormal;
};