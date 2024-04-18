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

class SSSGems : public State, public MouseEventListener, public KeyboardEventListener {
	static const int SHADOW_RES = 4096;

	enum Model {
		BUNNY,
		DRAGON,
		BUDDHA
	};

	struct DirectionnalLightInfo {
		Vector3f dir;
		Matrix4f biasProjView;
		Matrix4f proj;
		Matrix4f view;
		Matrix4f projView;
		float size;
	};

public:

	SSSGems(StateMachine& machine);
	~SSSGems();

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
	Matrix4f m_view;
	AssimpModel m_bunny, m_dragon;
	ObjModel m_buddha;
	DirectionnalLightInfo m_lightInfo;
	Framebuffer m_distance;
	const float maxDistance[1] = { 0.0f };
	Model model = Model::BUNNY;
	TrackBall m_trackball;
	Transform m_transform;
};