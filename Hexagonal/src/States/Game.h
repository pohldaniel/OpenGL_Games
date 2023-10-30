#pragma once

#include <fstream>
#include <sstream>
#include <memory>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>
#include <engine/ZoomableQuad.h>

#include "StateMachine.h"
#include "Background.h"
#include "TileSet.h"
#include "Enums.h"
#include "Animation.h"
#include "Map.h"

#include "Entity.h"
#include "AnimationController.h"
#include "Prefab.h"
#include "Movement.h"

class Game : public State, public MouseEventListener, public KeyboardEventListener {

	friend eMovementPlanner;

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

	TrackBall m_trackball;
	Transform m_transform;
	Camera m_camera;
	Map m_map;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_autoRedraw = false;
	bool m_redrawMap = true;

	Background m_background;
	ZoomableQuad m_zoomableQuad;
	Framebuffer m_mainRT;

	unsigned int m_atlas;

	std::vector<Entity*> m_entities;

	bool move;
	bool m_mouseDown = false;
	bool m_mouseMove = false;
	float m_mouseX, m_mouseY;
	float m_curMouseX, m_curMouseY;

	float m_zoomFactor = 1.0f;
	float m_focusPointY;
	float m_focusPointX;

	Enums::SelectionMode m_selectionMode = Enums::SelectionMode::ENTITY;
};