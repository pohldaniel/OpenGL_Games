#pragma once

#include "engine/Camera.h"
#include "RenderableObject.h"

#define CIRCLE_RADIUS 2.0f
#define AURA_HEIGHT 3.0f
#define HEIGHT_OFFSET 0.05f
#define RADIUS   0.5f

class Terrain;

class RespawnPointSet : public RenderableObject {

	struct State {
		Vector3f position;
	};

public:

	RespawnPointSet(const Vector3f& playerPos);
	~RespawnPointSet();
	void init(const Terrain& terrain);
	void draw(const Camera& camera) override;
	void update(const float dt) override;
	unsigned short& getRespawnId();
	const Vector3f& getActivePoistion() const;
	void addInstances(const std::vector<Matrix4f>& values);
	void deploy();

private:

	const std::vector<RespawnPointSet::State>& fromInstances(const std::vector<Matrix4f>& instances);
	void updateUbo();

	unsigned short m_respawnId;
	std::vector<Vector4f> m_colors;
	std::vector<std::array<bool, 16>> m_activate;
	std::vector<State> m_states;
	const Vector3f& m_playerPos;
};