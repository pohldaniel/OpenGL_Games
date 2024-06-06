#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include "BulletDebugDrawer.h"
#include "ObjModelNew.h"
#include "SimulationObject.h"
#include "VehicleObject.h"
#include "PhysicsChunkManager.h"

enum Control {
	VehicleAccelerate = 0,
	VehicleBrake,
	VehicleDown,
	VehicleUp,
	VehicleTurnLeft,
	VehicleTurnRight,
	Null, //No Input
	_MaxControls
};

class Kart : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Kart(StateMachine& machine);
	~Kart();

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
	void createBuffer(ObjModelNew* model);
	void drawObject(ObjModelNew* model);
private:

	void renderUi();
	void updateVehicleControls(Control accelerationControl, Control turnControl);
	void processInput();

	bool m_initUi = true;
	bool m_drawUi = true;
	float m_offsetDistance = 20.0f;
	float m_rotationSpeed = 0.1f;

	Camera m_camera;
	float lightCtr = 0.0f;
	bool bulletDebugDraw = false;
	BulletDebugDrawer* m_bulletDebugDrawer;

	ObjModelNew* m_model;

	SimulationObject simObject;
	VehicleObject vehicleObject;

	PhysicsChunkManager* physicsChunkManager;

	Control currentAcceleration = Control::Null;
	Control currentTurn = Control::Null;
};