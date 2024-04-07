#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>
#include <engine/TrackBall.h>
#include <States/StateMachine.h>

class SSSOGLP : public State, public MouseEventListener, public KeyboardEventListener {

public:

	static const int DEPTH_RES = 4096;

	enum Model {
		BUNNY,
		DRAGON,
		BUDDHA
	};

	SSSOGLP(StateMachine& machine);
	~SSSOGLP();

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
	void applyTransformation(TrackBall& arc);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useDiffuse = false;
	float m_thikness = 1.0f;

	Camera m_camera;
	AssimpModel m_bunny, m_dragon;
	ObjModel m_buddha;
	Framebuffer m_depth;
	const float maxDistance[1] = { 0.0f };
	Model model = Model::DRAGON;
	TrackBall m_trackball;
	Transform m_transform;

	Matrix4f m_lightProj, m_lightView, m_proj, m_view, m_model;
	Vector3f m_lightPos, m_camPos;

	const Matrix4f& Orbiting(Matrix4f& mtx, const Vector3f& target, float radius, float azinuthRad, float elevationRad);
	const Vector3f& getPosition(Matrix4f& mtx, Vector3f& pos);
	float bs_rad = 1.48632f;
	Vector3f m_center;
	float radius = 0.0f;
	float m_color[3] = { 0.2f, 0.9f, 0.7f };
};