#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <States/StateMachine.h>
#include <Entities/Player.h>

#include "Zone.h"
#include "Overworld.h"
#include "Level.h"

enum Scene {
	OVERWORLD,
	OMNI,
	LEVEL1,
	LEVEL2
};

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnReEnter(unsigned int prevState) override;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useCulling = true;
	bool m_debugCollision = false;
	bool m_drawCenter = false;

	Camera m_camera;
	//Overworld m_overworld;
	//Level m_level;
	Zone* m_zone;
	Player* m_player;

	float m_movingSpeed;
	float m_viewWidth;
	float m_viewHeight;
	float m_mapHeight;
	float m_screeBorder;

	Scene m_scene = Scene::OMNI;
};