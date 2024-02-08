#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AssimpAnimatedModel.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>
#include <Entities/Entity.h>
#include <Entities/Ant.h>
#include <Entities/Player.h>
#include <Entities/Light.h>
#include "HUD.h"
#include "HUDNew.h"

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

	void renderUi();
	void deleteEntities();

	bool m_initUi = true;
	bool m_drawUi = false;


	Camera* m_camera;
	Player* m_player;
	

	std::vector<Entity*> m_entitiesAfterClear;
	std::vector<Entity*> m_entities;

	float m_offsetDistance = 0.0f;
	aw::Status gameStatus;
	HUDNew hud;
};