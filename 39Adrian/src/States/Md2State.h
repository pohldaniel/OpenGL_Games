#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/MeshObject/Shape.h>
#include <engine/Camera.h>
#include <engine/Background.h>
#include <engine/MeshSequence.h>
#include <States/StateMachine.h>

#include "Utils/BinaryIO.h"
#include "Md2Model.h"
#include "Md2.h"

class Md2State : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Md2State(StateMachine& machine);
	~Md2State();

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

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;

	CMD2Model md2Models[4];
	animState_t animationStateMain;
	int iCurrentModel = 0;

	//MD2 md2;
	Utils::MD2IO md2Converter;
	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	Shape m_shape;
	MeshSequence m_sequence;

	int index = 0;
};