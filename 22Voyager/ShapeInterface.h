#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"
#include "engine/MeshObject/Shape.h"
#include "engine/Shader.h"
#include "StateMachine.h"

enum Model {
	TORUS,
	CAPSULE,
	SPHERE,
	SPIRAL,
	CYLINDER,
	QUAD,
	CUBE
};

enum RenderMode {
	TEXTURE,
	NORMAL,
	TANGENT,
	BITANGENT,
	GEOMETRY
};

class ShapeInterface : public State, public MouseEventListener {

public:

	ShapeInterface(StateMachine& machine);
	~ShapeInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void applyTransformation(TrackBall& arc);
	void renderUi();

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;
	bool m_initUi = true;
	Model model = Model::CYLINDER;
	RenderMode renderMode = RenderMode::TEXTURE;

	Shape m_currentShape;
	Shader* m_currentShader;
};
