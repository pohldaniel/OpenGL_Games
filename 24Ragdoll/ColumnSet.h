#pragma once

#include "engine/Camera.h"
#include "RenderableObject.h"

#define COLUMN_HEIGHT      7.0f
#define ENERGY_BALL_RADIUS 1.0f
#define GATHERNG_AREA_SIDE 4.0f

class Terrain;

class ColumnSet : public RenderableObject {

public:

	struct State {
		Vector3f position;
		float yaw;
	};


	ColumnSet(const Vector3f& playerPos);
	~ColumnSet();

	void init(const Terrain& terrain);
	void draw(const Camera& camera) override;

	bool insideGatheringArea(unsigned short index);
	const Vector3f& getPosition(unsigned short index) const;
	float getYaw(unsigned short index) const;
	const Vector3f getHole(unsigned short index) const;
	const std::vector<State>& getStates();

private:

	const std::vector<ColumnSet::State>& fromInstances(const std::vector<Matrix4f>& instances);

	const Vector3f& m_playerPos;
	std::vector<State> m_states;
};