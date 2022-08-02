#pragma once

#include "engine/MeshObject/MeshQuad.h"
#include "engine/Camera.h"
#include "Constants.h"

class MousePickCallback : public btCollisionWorld::ClosestRayResultCallback {
public:

	MousePickCallback(const btVector3& origin, const btVector3& target, int collisionFilterGroup, int collisionFilterMask) : btCollisionWorld::ClosestRayResultCallback(origin, target) {
		m_collisionFilterGroup = collisionFilterGroup;
		m_collisionFilterMask = collisionFilterMask;
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) {
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

};


class MousePicker {

public:
	MousePicker();

	void draw(const Camera& camera);
	void update(float dt);
	void updatePosition(unsigned int posX, unsigned int posY, const Camera& camera);
	void click();
private:
	MeshQuad *m_cursor;

	const float m_transitionSpeed = 1.5f;
	bool m_transitionEnd = false;
	bool m_fadeIn = false;
	bool m_fadeOut = true;
	float m_radius = 0.0f;
};