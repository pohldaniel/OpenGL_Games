#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>

#include "StateMachine.h"
#include "Background.h"

struct point {
	float x;
	float y;
};

class Plot : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Plot(StateMachine& machine);
	~Plot();

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

	Matrix4f viewportTransform(float x, float y, float width, float height, float *pixel_x, float *pixel_y);
	void renderUi();
	
	bool m_initUi = true;
	bool m_drawUi = true;
	
	float offset_x = 0;
	float scale_x = 1;

	unsigned int vbo[3];
	int border = 10;
	int ticksize = 10;
	Shader* m_shader;

	
};
