#pragma once
#include <filesystem>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>
#include <Objects/SceneNode.h>

#include "Mesh.h"
#include "Ant.h"
#include "ObjSequence.h"
#include "Player.h"
#include "StaticGO.h"
#include "HUD.h"
#include "PlayerNew.h"
#include "Entity.h"

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
	void destroyGameObject(int index);

	bool m_initUi = true;
	bool m_drawUi = false;

	ObjSequence m_objSequence;
	Camera m_camera;
	AssimpModel m_model;
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

	Player* m_player = nullptr;
	aw::StaticGO* m_muzzleGO = nullptr;
	aw::StaticGO* m_gunGO = nullptr;
	aw::StaticGO* m_handsGO = nullptr;
	aw::StaticGO* m_glovesGO = nullptr;
	aw::StaticGO* m_cpuGO = nullptr;
	aw::StaticGO* m_platformGO = nullptr;
	Ant *m_ant1 = nullptr, *m_ant2 = nullptr, *m_ant3 = nullptr, *m_ant4 = nullptr, *m_ant5 = nullptr, *m_ant6 = nullptr, *m_ant7 = nullptr, *m_ant8 = nullptr, *m_ant9 = nullptr;
	std::vector<aw::GameObject*> m_gameObjects;
	std::vector<aw::GameObject*> m_ants;

	aw::Status gameStatus;

	PlayerNew* playerNew;
	Entity* m_muzzleE = nullptr;
	Entity* m_gunE = nullptr;
	Entity* m_handsE = nullptr;
	Entity* m_glovesE = nullptr;

	std::vector<Entity*> m_entities;
};