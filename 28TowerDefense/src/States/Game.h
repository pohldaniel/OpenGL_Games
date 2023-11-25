#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>
#include <engine/ZoomableQuad.h>
#include <engine/ObjModel.h>
#include <engine/SlicedCube.h>
#include <engine/Transform.h>

#include <States/StateMachine.h>
#include "Background.h"
#include "Level.h"
#include "Unit.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void applyTransformation(TrackBall& arc);
	void renderUi();
	void addUnit(const Vector2f& posMouse);
	void removeUnitsAtMousePosition(const Vector2f& posMouse);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_mouseDownLeft = false;
	bool m_mouseDownRight = false;
	bool m_mouseMove = false;
	int m_mouseX;
	int m_mouseY;

	Background m_background;
	TrackBall m_trackball;
	Transform m_transform;
	Camera m_camera;
	Level m_level;
	const int tileSize = 48;

	std::vector<Unit> listUnits;
};