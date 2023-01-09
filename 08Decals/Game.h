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
#include "engine/ObjModel.h"
#include "engine/AssimpModel.h"
#include "engine/animation/AnimatedModel/AnimatedModel.h"
#include "engine/animation/AssimpAnimatedModel.h"

#include "Constants.h"
#include "StateMachine.h"
#include "Skybox.h"

#define PROJECTOR_BACK_OFF_DISTANCE 10.0f;
#define ALBEDO_TEXTURE_SIZE 4096
#define DEPTH_TEXTURE_SIZE 512

enum RenderMode {
	TEXTURE,
	DEFFERED,
};

struct DecalInstance {
	// Last hit
	Vector3f m_hit_pos;
	Vector3f m_hit_normal;
	float m_hit_distance = INFINITY;

	// Projector
	Vector3f m_projector_pos;
	Vector3f m_projector_dir;
	Matrix4f m_projector_view;
	Matrix4f m_projector_view_inv;
	Matrix4f m_projector_view_proj;
	Matrix4f m_projector_proj;
	Matrix4f m_projector_proj_inv;
	Vector4f m_decal_overlay_color;
	Vector2f m_aspect_ratio;
};

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

	void initTexture();
	void initTexture(Texture& texture);
	void applyDecals();
	void renderDepthMap();
	void shdowPass();

	void renderGBuffer();
	void renderDecals();
	void combinerPass();
	void renderDeferred();

	Camera m_camera;
	SkyBox m_skybox;
	AssimpModel m_teapot;

	Vector3f m_projector_pos;
	Vector3f m_projector_dir;

	Matrix4f m_projector_view;
	Matrix4f m_projector_proj;

	Matrix4f m_projector_view_inv;
	Matrix4f m_projector_proj_inv;

	float m_projector_size = 10.0f;
	float m_projector_rotation = 0.0f;

	Framebuffer m_albedoFramebuffer;
	Framebuffer m_depthFramebuffer;
	Framebuffer m_copyFramebuffer;

	Framebuffer m_gBuffer;
	Framebuffer m_decalBuffer;
	Framebuffer m_finalBuffer;

	std::vector<DecalInstance> m_decal_instances;
	float m_projector_outer_depth = 30.0f;
	float m_projector_inner_depth = 30.0f;

	unsigned int offsetX;
	unsigned int offsetY;
	Vector2f size = Vector2f(0.75f, 0.75f);
	Shader *m_quadShader;
	Shader *m_quadShadow;
	Quad *m_leftBottom;
	Quad *m_rightBottom;
	Quad *m_leftTop;
	Quad *m_rightTop;
	Quad *m_quad;
	bool m_debug =false;

	std::vector<float> m_vertices;
	unsigned int m_vbo, m_vao;

	RenderMode renderMode = RenderMode::DEFFERED;
};