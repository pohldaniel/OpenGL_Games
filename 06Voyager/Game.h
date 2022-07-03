#pragma once

#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"

#include "Constants.h"
#include "StateMachine.h"

#include "Camera.h"
#include "Terrain.h"
#include "Water.h"

class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;
	void performCameraCollisionDetection();

	Camera m_camera;
	Terrain m_terrain;
	Water m_water;
	Quad *m_reflectionQuad;
	Quad *m_refractionQuad;

	Shader *m_quadShader;

	unsigned int m_fboCopy, m_frameTextureCopy;

	unsigned int m_fboReflection, m_frameTextureReflection, m_frameTextureReflectionDepth;
	unsigned int m_fboRefraction, m_frameTextureRefraction, m_frameTextureRefractionDepth;

	Vector3f m_cameraBoundsMax;
	Vector3f m_cameraBoundsMin;
	Vector2f size = Vector2f(0.75f, 0.75f);

	unsigned int offsetX;
	unsigned int offsetY;
	bool m_debug = false;
};