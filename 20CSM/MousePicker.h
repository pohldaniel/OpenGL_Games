#pragma once

#include "engine/MeshObject/MeshQuad.h"
#include "engine/Camera.h"
#include "engine/Framebuffer.h"
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
	void updateObjectId(unsigned int posX, unsigned int posY);
	void updatePosition(unsigned int posX, unsigned int posY, const Camera& camera);

	void click();
	unsigned int getPickedId() const;

	Framebuffer getBuffer() const;

private:

	MeshQuad *m_cursor;
	Framebuffer m_mousePickBuffer;

	const float m_transitionSpeed = 1.5f;
	bool m_transitionEnd = false;
	bool m_fadeIn = false;
	bool m_fadeOut = true;
	float m_radius = 0.0f;

	unsigned int m_pickedId = 0;
	const int PBO_COUNT = 2;
	unsigned int pboIds[2];	
	int index = 0;
	int nextIndex = 0;
};