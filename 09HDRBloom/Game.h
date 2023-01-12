#define NOMINMAX
#pragma once
#include <algorithm>
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/MeshObject/MeshQuad.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/Cube.h"
#include "engine/ObjModel.h"
#include "engine/AssimpModel.h"
#include "engine/animation/AnimatedModel/AnimatedModel.h"
#include "engine/animation/AssimpAnimatedModel.h"

#include "Constants.h"
#include "StateMachine.h"
#include "Skybox.h"
#include "Bloom.h"

#define PROJECTOR_BACK_OFF_DISTANCE 10.0f;
#define ALBEDO_TEXTURE_SIZE 4096
#define DEPTH_TEXTURE_SIZE 512

class RayResultCallback : public btCollisionWorld::ClosestRayResultCallback {
public:

	RayResultCallback(const btVector3& origin, const btVector3& target, int collisionFilterGroup, int collisionFilterMask) : btCollisionWorld::ClosestRayResultCallback(origin, target) {
		m_collisionFilterGroup = collisionFilterGroup;
		m_collisionFilterMask = collisionFilterMask;
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace) {
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

};

class Game : public State, public MouseEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;	

	void renderOffscreen();

	Camera m_camera;

	Framebuffer m_copyFramebuffer;
	Framebuffer m_hdrBuffer;
	Framebuffer m_pingPongBuffer[2];

	unsigned int offsetX;
	unsigned int offsetY;
	Vector2f size = Vector2f(0.75f, 0.75f);
	Shader *m_quadShader;
	Quad *m_leftBottom;
	Quad *m_rightBottom;
	Quad *m_leftTop;
	Quad *m_rightTop;
	Quad *m_quad;
	Cube *m_cube;

	bool m_debug = false;
	bool m_useFilter = false;

	std::vector<float> m_vertices;
	unsigned int m_vbo, m_vao;

	BloomRenderer bloomRenderer;
	
	std::vector<Vector3f> lightPositions;
	std::vector<Vector3f> lightColors;
	Transform m_model;

	bool bloom = true;
	float exposure = 1.0f;
	int programChoice = 3;
	float bloomFilterRadius = 0.005f;
};

