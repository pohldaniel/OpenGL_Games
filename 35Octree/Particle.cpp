#include <random>
#include <time.h>
#include "Particle.h"

Particle::Particle(){
	m_lifetime = 200;
	m_velocity = Vector3f(0,0,0);
	m_position = Vector3f((((float) rand() / (float) RAND_MAX) - 0.5)/3,-0.5,(((float) rand() / (float) RAND_MAX) - 0.5)/3);
}

void Particle::update(std::vector<Particle*> &_particlesInRange, float dt){
	Vector3f acceleration;
	acceleration[1] += 0.6f;
	m_lifetime--;

	for (unsigned int i = 0; i < _particlesInRange.size(); i++){
		if (_particlesInRange[i] != this){
			if (_particlesInRange.size() > 20){
				acceleration += (m_position - _particlesInRange[i]->m_position) * 0.02f;
			}else{
				acceleration += (m_position - _particlesInRange[i]->m_position) * -2.0f;
			}
		}
	}

	m_velocity += acceleration * dt;
	m_position += m_velocity * dt;
}

const Vector3f& Particle::getPos() { 
	return m_position; 
}

int Particle::getLifetime() { 
	return m_lifetime; 
}