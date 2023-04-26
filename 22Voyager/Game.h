#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/TrackBall.h"
#include "engine/Cube.h"

#include "StateMachine.h"
#include "Weapon.h"
#include "SpotLight.h"
#include "Atmosphere.h"
#include "Terrain.h"
#include "Cloth.h"
#include "Enemy.h"

class Game : public State, public MouseEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void renderUi();

	bool m_initUi = true;

	Terrain m_terrain;
	Atmosphere m_atmosphere;
	DirectionalLight m_dirLight;
	PointLight m_pointLight;

	Cloth m_flag;
	AssimpModel m_flagPoleModel, m_rockModel;

	RenderableObject m_skybox;
	RenderableObject m_saturn;
	RenderableObject m_saturnRings;
	RenderableObject m_rock;
	RenderableObject m_flagPole;
	RenderableObject m_sniperScope;
	RenderableObject m_crossHaire;
	RenderableObject m_health;
	RenderableObject m_ammo;

	std::vector<Enemy*> m_enemies;
	float m_dataTransmitTimer = 0.0f, m_enemySpawnTimer = 0.0f, m_gameStateTimer = 0.0f;
	int m_enemyCount = 1;
};
