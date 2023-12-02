#pragma once

#include <memory>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>
#include <engine/ZoomableQuad.h>
#include <engine/ObjModel.h>
#include <engine/SlicedCube.h>
#include <engine/Transform.h>

#include <States/StateMachine.h>
#include "Background.h"
#include "Bridge/UnitBridge.h"
#include "Bridge/Pin.h"
#include "Bridge/Beam.h"

enum class GameMode {
	PLAYING,
	PAUSED,
	VICTORY,
	DEFEAT
};


class Bridge : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Bridge(StateMachine& machine);
	~Bridge();

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
	void setupBridge1();
	void setupBridge2();
	
	bool m_initUi = true;
	bool m_drawUi = true;
	
	Camera m_camera;
	Background m_background;
	unsigned int m_sprites;

	std::vector<std::shared_ptr<Beam>> listBeams;
	std::vector<std::shared_ptr<Pin>> listPins;
	UnitBridge unitRedCreature;

	GameMode gameModeCurrent = GameMode::PLAYING;
};