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

#include "Mesh.h"
#include "HUD.h"

enum LightType{
	DIRECTIONAL,
	POINTAW,
	SPOT
};

struct LightStruct{
	int enabled;
	LightType type;
	float padding0, padding1;
	glm::vec4 ambient, diffuse, specular;
	glm::vec3 direction;
	float angle;
	glm::vec3 position;
	float padding2;
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
	void destroyAnt(int index);

	bool m_initUi = true;
	bool m_drawUi = false;

	ObjSequence m_objSequence;
	Camera m_camera;
	AssimpModel* m_model;
	AssimpModel m_gun;

	std::shared_ptr<aw::Mesh> m_muzzleMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_bulletMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_gunMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_handsMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_glovesMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_cpuMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_platformMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_antMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_cubeMesh = nullptr;
	std::vector<std::shared_ptr<aw::Mesh>> m_meshes;

	aw::Status gameStatus;

	Player* m_player;
	Entity* m_muzzleE = nullptr;
	Entity* m_gunE = nullptr;
	Entity* m_handsE = nullptr;
	Entity* m_glovesE = nullptr;
	Entity* m_cpuE = nullptr;
	Entity* m_platformE = nullptr;

	Ant *m_ant1 = nullptr, *m_ant2 = nullptr, *m_ant3 = nullptr, *m_ant4 = nullptr, *m_ant5 = nullptr, *m_ant6 = nullptr, *m_ant7 = nullptr, *m_ant8 = nullptr, *m_ant9 = nullptr;
	std::vector<Entity*> m_entities;
	std::vector<Ant*> m_ants;
};