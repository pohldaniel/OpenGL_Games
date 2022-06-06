#pragma once

#include "engine/input/Mouse.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/MouseEventListener.h"

#include "Constants.h"
#include "StateMachine.h"

#include "MapLoader.h"
#include "CameraMapController.h"

class Game : public State, public MouseEventListener {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;

private:
	MapLoader m_mapLoader;
	CameraMapController * m_camController = nullptr;

	Spritesheet *m_spriteSheet;
	Shader *m_shaderLevel;

	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f m_projection = Matrix4f::IDENTITY;

	float m_offsetX = 0.0f;
	float m_offsetY = -20.0f;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;

	void OnMouseMotion(Event::MouseMoveEvent& event);
	void CenterCameraOverCell(int row, int col);
};