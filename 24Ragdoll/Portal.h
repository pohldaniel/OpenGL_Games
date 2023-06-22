#pragma once

#include <btBulletDynamicsCommon.h>
#include "engine/Camera.h"
#include "RenderableObject.h"
#include "StaticTrigger.h"

#define PORTAL_SIDE  3.0f
#define PORTAL_SPEED 2.0f

class Terrain;

class Portal : public RenderableObject, public StaticTrigger {

public:

	Portal();
	~Portal();

	void init(const Terrain& terrain);
	void draw(const Camera& camera) override;
	void update(const float dt) override;
	void setDisabled(bool disabled);

	const Vector3f getReceptor(int index) const;
	float getZ();

private:

	RenderableObject m_vortex;
};