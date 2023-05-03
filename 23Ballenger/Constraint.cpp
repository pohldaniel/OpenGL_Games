#include "Constraint.h"

Constraint::Constraint(ClothParticle* p1, ClothParticle* p2) : m_pParticleOne(p1), m_pParticleTwo(p2) {
	Vector3f v = p1->GetPos() - p2->GetPos();
	m_restDistance = v.length();
}

Constraint::~Constraint(){

}

void Constraint::SatisfyConstraint() {
	Vector3f distance = m_pParticleTwo->GetPos() - m_pParticleOne->GetPos();
	float currDistance = distance.length();
	Vector3f offsetVec = distance * (1.0f - m_restDistance / currDistance);
	Vector3f offsetVecHalf = offsetVec * 0.5f;
	m_pParticleOne->OffsetPos(offsetVecHalf);
	m_pParticleTwo->OffsetPos(-offsetVecHalf);
}