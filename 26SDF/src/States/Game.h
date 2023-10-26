#pragma once

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

#include <States/StateMachine.h>
#include "Background.h"

struct Instance {
	// Mesh
	ObjModel			mesh;
	Vector3f			color;

	// SDF
	Texture				sdf;
	std::array<int, 3>	volume_size;
	Vector3f			grid_origin;
	float				grid_step_size;
	Vector3f			min_extents;
	Vector3f			max_extents;

	// Transform
	bool	 animate = false;
	float	 rotation = 0.0f;
	Vector3f position = Vector3f(0.0f);
	Matrix4f transform = Matrix4f::IDENTITY;
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

	void applyTransformation(TrackBall& arc);
	void renderUi();
	void bake_sdf(Instance& instance, float grid_step_size, int padding);

	bool m_initUi = true;
	bool m_drawUi = true;
	
	Background m_background;
	TrackBall m_trackball;
	Transform m_transform;
	Camera m_camera;

	std::vector<Instance> m_instances;
	SlicedCube m_slicedCube;
};