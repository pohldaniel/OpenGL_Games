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

enum LightType{
	DIRECTIONAL,
	POINTAW,
	SPOT
};

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

	ObjSequence m_objSequence;
	Camera* m_camera;
	Player* m_player;
	Entity *m_muzzleE = nullptr, *m_gunE = nullptr, *m_handsE = nullptr, *m_glovesE = nullptr, *m_cpuE = nullptr, *m_platformE = nullptr;
	Ant *m_ant1 = nullptr, *m_ant2 = nullptr, *m_ant3 = nullptr, *m_ant4 = nullptr, *m_ant5 = nullptr, *m_ant6 = nullptr, *m_ant7 = nullptr, *m_ant8 = nullptr, *m_ant9 = nullptr;
	
	std::vector<Ant*> m_ants;

	std::vector<Entity*> m_entitiesAfterClear;
	std::vector<Entity*> m_entities;

	aw::Status gameStatus;

	std::vector<AssimpModel*> m_meshes;
	float m_offsetDistance = 0.0f;
};