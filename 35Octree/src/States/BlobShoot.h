#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <engine/Camera.h>
#include <engine/Background.h>

#include <States/StateMachine.h>

#include <Physics/Physics.h>

class BlobShoot : public State, public MouseEventListener, public KeyboardEventListener {

public:

	BlobShoot(StateMachine& machine);
	~BlobShoot();

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
	btRigidBody* addSphere(const Vector3f& pos, float rad, float mass, int collisionFilterGroup = 1, int collisionFilterMask = -1);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugCollision = true;

	Camera m_camera;
	Background m_background;
	Framebuffer m_sceneBuffer;
	const float maxDistance[1] = { 100000.0f };
	std::vector<btRigidBody*> m_bodies;
};