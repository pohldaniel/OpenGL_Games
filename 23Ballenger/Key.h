#pragma once
#include "engine/Camera.h"
#include "RenderableObject.h"

#define LEVITATION_SPEED 2.0f
#define BEACON_MIN_RADIUS 0.75f
#define BEACON_HEIGHT 140.0f

class cTerrain;

class Key : public RenderableObject {

	struct KeyState {
		Matrix4f model;
		bool deployed;
	};

public:

	Key(const Vector3f& playerPos);
	~Key();

	void init(const cTerrain& terrain);
	void draw(const Camera& camera) override;
	void update(const float dt, const float dist);
	void updateCylinderShape();
	void setPickedKeyId(int value);
	void deploy(int id, const Vector3f& pos, float yaw);

	void addInstances(const std::vector<Matrix4f>& values);
	const std::vector<Matrix4f>& getInstances();

private:

	const std::vector<Matrix4f>& fromKeyState(const std::vector<KeyState>& keyStates);
	const std::vector<KeyState>& fromInstances(const std::vector<Matrix4f>& instances);

	float ang = 0.0f;

	std::vector<KeyState> m_keyStates;
	std::vector<Matrix4f> m_instances;
	std::vector<Matrix4f> m_mtxKey;
	std::vector<Matrix4f> m_mtxCylinder;
	std::vector<Vector4f> m_colors;

	int m_pickedKeyId;
	const Vector3f& m_playerPos;
};
