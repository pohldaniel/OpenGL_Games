#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/scene/SceneNodeLC.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>

#include "ParticleSystem.h"

class SoftParticle : public State, public MouseEventListener, public KeyboardEventListener {

	struct NormalAttributes {
		Vector3f pos;
		Vector2f uv;
		Vector3f color;
		float size;
		float alpha;
	};

public:

	SoftParticle(StateMachine& machine);
	~SoftParticle();

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
	void draw();
	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_soft = true;

	Camera m_camera;
	ObjModel m_jesus, m_rock1, m_rock2;
	Framebuffer m_depthBuffer;
	ParticleSystem m_particleSystem;
	std::vector<NormalAttributes> normalVertex;
};
