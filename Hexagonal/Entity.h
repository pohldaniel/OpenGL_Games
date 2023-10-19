#pragma once

#include <string>
#include <engine/Vector.h>

#define CELL_WIDTH 32.0f
#define CELL_HEIGHT 32.0f

class Prefab;

class Entity {

public:

	Entity(const Prefab& prefab);
	void update(float dt);
	void processInput();

//private:

	void updateGridBounds();

	Vector2f m_directrion;
	Vector2f m_velocity;
	Vector2f m_oldFacingDirection;
	Vector2f m_position;

	const Prefab& m_prefab;

	const int xSpeedParameterHash = std::hash< std::string >()("xSpeed");
	const int ySpeedParameterHash = std::hash< std::string >()("ySpeed");
	const int magnitudeParameterHash = std::hash< std::string >()("magnitude");

	int minX, maxX, minY, maxY;
};