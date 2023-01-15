#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/MeshObject/MeshQuad.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"

#include "Constants.h"
#include "StateMachine.h"
#include "Skybox.h"
#include "Bloom.h"
#include "QuadTree.h"
#include "Terrain2.h"

#define DEPTH_TEXTURE_SIZE 2048

class Game : public State, public MouseEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;	
	
	Camera m_camera;

	Quad *m_quad;
	QuadTree* m_quadTree;
	TerrainClass* m_terrain;
};

