#pragma once

#include <fstream>
#include <sstream>

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

enum RenderMode {
	ISOTILE,
	ISOHEX,
	HEX,
	HEXFLIP
};

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

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

	void applyTransformation(TrackBall& arc);
	void renderUi();
	

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;
	
	Background m_background;
	RenderMode renderMode = RenderMode::ISOTILE;

	int m_cols;
	int m_rows;

	const int ISO_WIDTH;
	const int ISO_HEIGHT;

	const int HEX_WIDTH;
	const int HEX_HEIGHT;

	const int HEX_OFFSET_X;
	const int HEX_OFFSET_Y;

	unsigned int** m_tileId;
};
