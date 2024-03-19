#pragma once

#include <vector>
#include <engine/Vector.h>

class Particle{

public:
	Particle();
	void update(std::vector<Particle*> &_particlesInRange, float dt);

	const Vector3f& getPos();
	int getLifetime();

private:
	Vector3f m_position;	
	Vector3f m_velocity;
	int m_lifetime;
};
