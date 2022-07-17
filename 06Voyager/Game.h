#pragma once

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

class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;
	void performCameraCollisionDetection();

	void renderOffscreen();
	void shadowPass();

	Camera m_camera;
	Terrain m_terrain;
	Water m_water;
	Quad *m_reflectionQuad;
	Quad *m_refractionQuad;
	Quad *m_perlinQuad;
	Quad *m_shadowQuad;

	Framebuffer m_copyFramebuffer;
	Framebuffer m_lightDepthFramebuffer;

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
};