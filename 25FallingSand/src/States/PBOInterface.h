#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/Mouse.h>
#include <engine/Transform.h>
#include <States/StateMachine.h>
#include <UI/Button.h>
#include <UI/TextField.h>

class PBOInterface : public State, public MouseEventListener, public KeyboardEventListener {

public:
	PBOInterface(StateMachine& machine);
	~PBOInterface();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	void updatePixels(GLubyte* dst, int size);

	GLuint pboIds[2];
	GLuint textureId;
	GLubyte* imageData = 0;
};