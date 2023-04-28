#pragma once

#include "engine/Vector.h"

class Particle {
public:
	Particle();
	~Particle();

	bool Update(float deltaTime);

	void SetOrigin(const Vector3f& orig);

	Vector3f GetPos() { return m_position; }

private:
	Vector3f m_position;
	Vector3f m_origin;
	Vector3f m_velocity;
	float m_life;
};