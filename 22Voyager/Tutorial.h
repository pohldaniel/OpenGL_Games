#pragma once
#include <glm/glm/glm.hpp>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/TrackBall.h"

#include "StateMachine.h"
#include "Weapon.h"
#include "SpotLight.h"
#include "Atmosphere.h"
#include "Terrain.h"

class Tutorial : public State, public MouseEventListener {

public:

	Tutorial(StateMachine& machine);
	~Tutorial();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void RenderScene();

	void renderUi();

	bool m_initUi = true;

	SpotLight* m_spotLight;
	Animation m_animationComponent;
	Weapon* m_currWeapon, *m_assaultRifle, *m_sniperRifle;
	bool m_walking, m_sprinting, m_jumping, m_isInAir, m_firing, m_reloading, m_muzzleFlash, m_usingAR, m_swapping, m_swapped;

	Terrain m_terrain;
	Atmosphere m_atmosphere;
	DirectionalLight m_dirLight;
	PointLight m_pointLight;
};