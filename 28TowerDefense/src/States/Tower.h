#pragma once

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
#include <engine/ObjModel.h>
#include <engine/SlicedCube.h>
#include <engine/Transform.h>

#include <States/StateMachine.h>
#include "Background.h"
#include "Level.h"
#include "Unit.h"
#include "Timer.h"
#include "Turret.h"
#include "Projectile.h"

class Tower : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Tower(StateMachine& machine);
	~Tower();

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
	void updateSpawnUnitsIfRequired(float dt);
	void addUnit(const Vector2f& posMouse);
	void addTurret(const Vector2f& posMouse);
	void removeTurretsAtMousePosition(const Vector2f& posMouse);
	void updateUnits(float dt);
	void updateProjectiles(float dt);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_mouseDownLeft = false;
	bool m_mouseDownRight = false;
	bool m_mouseMove = false;
	int m_mouseX;
	int m_mouseY;

	Camera m_camera;
	Level m_level;
	

	std::vector<std::shared_ptr<Unit>> listUnits;
	std::vector<Turret> listTurrets;
	std::vector<Projectile> listProjectiles;

	unsigned int m_sprites;

	Timer spawnTimer, roundTimer;
	int spawnUnitCount = 0;

	static float TileSize;
};