#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>
#include "Background.h"
#include "LuaHelper.h"
#include "Scene.h"
#include "UserInterface.h"
#include "Input.h"

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

	void renderUi();
	void lua_openmetatables(lua_State* L);

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	Background m_background;

	Scene scene;
	Input input;
	UserInterface UI;
	lua_State* L;
};