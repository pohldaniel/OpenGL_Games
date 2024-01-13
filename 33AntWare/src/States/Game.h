#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>

#include "Background.h"
#include "Mesh.h"
#include "Ant.h"
#include "ObjSequence.h"
#include "Player.h"
#include "StaticGO.h"
#include "CameraAW.h"

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

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	Background m_background;
	AssimpModel m_model;
	AssimpModel m_gun;

	Ant* m_ant;

	std::shared_ptr<aw::Mesh> m_muzzleMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_bulletMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_gunMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_handsMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_glovesMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_cpuMesh = nullptr;
	std::shared_ptr<aw::Mesh> m_platformMesh = nullptr;


	Player* m_player = nullptr;
	aw::StaticGO* m_muzzleGO = nullptr;
	aw::StaticGO* m_gunGO = nullptr;
	aw::StaticGO* m_handsGO = nullptr;
	aw::StaticGO* m_glovesGO = nullptr;
	aw::StaticGO* m_cpuGO = nullptr;
	aw::StaticGO* m_platformGO = nullptr;

	std::vector<std::shared_ptr<aw::Mesh>> m_meshes;
	aw::Camera* m_cameraAW;
};