#pragma once

#include <string>
#include <engine/Vector.h>
#include <engine/Rect.h>
#include <engine/input/Event.h>
#include <engine/Camera.h>

#define CELL_WIDTH 32.0f
#define CELL_HEIGHT 32.0f

class Prefab;

class Entity {

public:

	Entity(const Prefab& prefab, const Camera& camera, const float& zoomFactor, const float& focusPointX, const float& focusPointY);
	void update(float dt);
	void processInput();
	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE);
//private:

	void updateGridBounds();
	void drawClickBox();

	Vector2f m_directrion;
	Vector2f m_velocity;
	Vector2f m_oldFacingDirection;
	Vector2f m_position;

	const int xSpeedParameterHash = std::hash< std::string >()("xSpeed");
	const int ySpeedParameterHash = std::hash< std::string >()("ySpeed");
	const int magnitudeParameterHash = std::hash< std::string >()("magnitude");

	int m_minX, m_maxX, m_minY, m_maxY;
	bool m_isSelected;

	const Prefab& prefab;
	const Camera& camera;
	const float& zoomFactor;
	const float& focusPointX;
	const float& focusPointY;	
};