#pragma once
#include <numeric>

#include "engine/Camera.h"
#include "RenderableObject.h"

#define LEVITATION_SPEED 2.0f
#define BEACON_MIN_RADIUS 0.75f
#define BEACON_HEIGHT 140.0f

class Terrain;

class KeySet : public RenderableObject {

	struct State {
		bool deployed;
		Vector3f position;
	};

public:

	KeySet(const Vector3f& playerPos);
	~KeySet();

	void init(const Terrain& terrain);
	void draw(const Camera& camera) override;
	void update(const float dt) override;
	void updateCylinderShape();
	void pickKey(int id);
	void deploy(const Vector3f& pos, float yaw);
	void restorePrevState();

	void addInstances(const std::vector<Matrix4f>& values);
	unsigned short getNumDeployed();
	bool isDeployed(unsigned short index);
	const Vector3f& getPosition(unsigned short index);
	const std::vector<State>& getKeyStates();
	int& getPickedKeyId();

private:

	const std::vector<State>& fromInstances(const std::vector<Matrix4f>& instances);

	float ang = 0.0f;

	std::vector<State> m_keyStates;
	std::vector<Matrix4f> m_mtxKey;
	std::vector<Matrix4f> m_mtxCylinder;
	std::vector<Vector4f> m_colors;
	Vector4f m_pickedColor;
	//Vector3f m_pickedPos;

	unsigned short m_numDeployed;
	int m_pickedKeyId;
	const Vector3f& m_playerPos;

	std::vector<short> m_idCache;
};
