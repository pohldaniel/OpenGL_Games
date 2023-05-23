#pragma once
#include "engine/Camera.h"
#include "RenderableObject.h"

#define LEVITATION_SPEED 2.0f
#define BEACON_MIN_RADIUS 0.75f
#define BEACON_HEIGHT 140.0f

class Key : public RenderableObject {

public:
	Key();
	~Key();

	void draw(const Camera& camera) override;
	//void update(const float dt) override;
	void update(const float dt, const float dist);

private:

	float ang = 0.0f;
	bool deployed = false;
	std::vector<Matrix4f> m_mtxKey;

	std::vector<Matrix4f> m_mtxCylinder;
	std::vector<float> m_maxHeights;
};
