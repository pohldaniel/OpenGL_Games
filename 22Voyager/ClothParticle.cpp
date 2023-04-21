#include "ClothParticle.h"

ClothParticle::ClothParticle(){

}

ClothParticle::ClothParticle(const Vector3f& pos) :
	m_pos(pos),
	m_oldPos(pos),
	m_mass(1.0f),
	m_acceleration(Vector3f(0.0f, 0.0f, 0.0f)),
	m_movable(true),
	m_accumulatedNormal(Vector3f(0.0f, 0.0f, 0.0f)),
	m_damping(0.1f),
	m_timeStep(0.25f) {

}

ClothParticle::~ClothParticle(){

}

void ClothParticle::VerletIntegration() {
	if (m_movable){
		Vector3f temp = m_pos;
		m_pos = m_pos + (m_pos - m_oldPos) * (1.0f - m_damping) + m_acceleration * m_timeStep;
		m_oldPos = temp;
		m_acceleration = Vector3f(0.0f, 0.0f, 0.0f);
	}
}