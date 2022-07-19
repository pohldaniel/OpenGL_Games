#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/MeshObject/MeshQuad.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/ObjModel.h"
#include "CharacterSetBmp.h"
#include "Text.h"

#include "Constants.h"
#include "StateMachine.h"
#include "Terrain.h"
#include "Water.h"
#include "Tree.h"
#include "Fern.h"
#include "Skybox.h"
#include "Barrel.h"
#include "Ray.h"


class RayResultCallback : public btCollisionWorld::ClosestRayResultCallback{
public:

	RayResultCallback() : btCollisionWorld::ClosestRayResultCallback(btVector3(0.0f, 0.0f, 0.0f), btVector3(0.0f, 0.0f, 0.0f)){
	}

	btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
	{
		//if (rayResult.m_collisionObject == mSelf) return 1.0f;
		//if (rayResult.m_collisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) return 1.0f;
		return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
	}

};

class Game : public State, public MouseEventListener {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void performCameraCollisionDetection();

	void renderOffscreen();
	void shadowPass();
	void mousePickPass();

	Camera m_camera;
	Terrain m_terrain;
	Water m_water;
	Quad *m_reflectionQuad;
	Quad *m_refractionQuad;
	Quad *m_perlinQuad;
	Quad *m_shadowQuad;

	Framebuffer m_copyFramebuffer;
	Framebuffer m_lightDepthFramebuffer;
	Framebuffer m_mousePickBuffer;

	Shader *m_quadShader;
	Shader *m_quadShadow;
	Shader *m_quadArrayShader;
	Shader *m_quadArrayShadowShader;

	MeshQuad *m_meshQuad;
	MeshCube *m_meshCube;
	std::vector<MeshCube*> m_entities;
	std::vector<Fern*> m_fernEntities;

	Tree *m_tree;
	SkyBox m_skybox;
	Barrel m_barrel;
	Ray m_ray;
	Vector3f m_cameraBoundsMax;
	Vector3f m_cameraBoundsMin;
	Vector2f size = Vector2f(0.75f, 0.75f);

	unsigned int offsetX;
	unsigned int offsetY;
	bool m_debug = false;
	bool m_debugNormal = false;
	bool m_flag;
	unsigned short m_debugCount = 0;
	CharacterSetBmp charachterSet;
	Text* m_text;
	//Model* m_tree;

	const int PBO_COUNT = 2;
	unsigned int pboIds[2];

	int pickedID = 0;
};