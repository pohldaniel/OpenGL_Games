#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <engine/Camera.h>
#include <engine/Background.h>

#include <States/StateMachine.h>

#include <Physics/Physics.h>

class BlobShoot : public State, public MouseEventListener, public KeyboardEventListener {
	static const int MAX_SPHERE_COUNT = 24;
	static const int SPHERE_RADIUS = 6;

	static const int BOX_SIZE = 100;
	static const int TINY_SIZE = 1;

public:

	struct SphereStruct {
		Vector3f position;
		float padding0;
		Quaternion orientaion;
		Vector3f color;
		float radius;
	};

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
	btCollisionObject* addBox(const Vector3f& pos, const Vector3f& size);
	void addCharacter(const Vector3f& pos, const Vector2f& size);
	const Vector3f getPosition();
	const Vector3f getPosition(btRigidBody* body);
	const Quaternion getOrientation(btRigidBody* body);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debugCollision = true;

	Camera m_camera;
	Background m_background;
	Framebuffer m_sceneBuffer, m_depthBuffer;
	const float maxDistance[1] = { 100000.0f };
	std::vector<btRigidBody*> m_bodies;
	std::vector<SphereStruct> m_spheres;

	btPairCachingGhostObject* m_pairCachingGhostObject;
	btKinematicCharacterController* m_kinematicController;
	btGhostPairCallback* m_ghostPairCallback;

	Vector3f m_colShapeOffset = Vector3f(0.0f, 0.84f, 0.0f);
	float m_offsetDistance = 10.0f;
	float m_accTime = 0.0f;
};