#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/scene/SceneNodeLC.h>
#include <engine/MeshObject/Shape.h>
#include <engine/MeshSequence.h>
#include <engine/Camera.h>
#include <Physics/BulletDebugDrawer.h>
#include <States/StateMachine.h>

#include <Entities/ShapeEntity.h>
#include <Entities/Vehicle.h>
#include <Entities/Skybox.h>

#include "PhysicsChunkManager.h"

enum Control {
	VehicleAccelerate = 0,
	VehicleBrake,
	VehicleDown,
	VehicleUp,
	VehicleTurnLeft,
	VehicleTurnRight,
	Null,
	_MaxControls
};

enum CameraMode {
	FOLLOW,
	FOLLOW_ROTATE,
	FREE
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

private:

	void renderUi();
	void updateVehicleControls(Control accelerationControl, Control turnControl);
	void processInput();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_drawBulletDebug = false;

	float m_offsetDistance = 15.0f;
	float m_rotationSpeed = 0.1f;

	Camera m_camera;
	float lightCtr = 0.0f;
	
	BulletDebugDrawer* m_bulletDebugDrawer;
	PhysicsChunkManager m_physicsChunkManager;
	Control currentAcceleration = Control::Null;
	Control currentTurn = Control::Null;
	SceneNodeLC* m_root;
	std::vector<Entity*> m_entities;
	Vehicle* m_vehicle;
	MeshSequence m_meshSequence;
	Shape m_shape;

	CameraMode cameraMode = CameraMode::FOLLOW;
	Skybox m_skybox;
};