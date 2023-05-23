#pragma once
#include "engine/Camera.h"
#include "RenderableObject.h"

#define LEVITATION_SPEED 2.0f
#define BEACON_MIN_RADIUS 0.75f
#define BEACON_HEIGHT 140.0f

class Key : public RenderableObject {

public:

	Key(RenderableObject& player);
	~Key();

	void draw(const Camera& camera) override;
	void update(const float dt, const float dist);
	void addInstances(const std::vector<Matrix4f>& values);
	const std::vector<Matrix4f>& getInstances() const;
	//void replaceInstance(const Matrix4f& value, unsigned int index);
	//void updateShape();
	void updateCylinderShape();
	void setPickedKeyId(int value);

private:

	float ang = 0.0f;
	bool deployed = false;
	std::vector<Matrix4f> m_instances;
	std::vector<Matrix4f> m_mtxKey;
	std::vector<Matrix4f> m_mtxCylinder;

	int m_pickedKeyId;
	const Vector3f& m_playerPos;
};
