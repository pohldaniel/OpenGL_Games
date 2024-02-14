#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/scene/Object.h>

#include <engine/Camera.h>
#include <engine/Rect.h>
#include <States/StateMachine.h>

class MainMenu : public State, public MouseEventListener, public KeyboardEventListener {

public:

	MainMenu(StateMachine& machine);
	~MainMenu();

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
	void processInput();

	bool m_initUi = true;
	bool m_drawUi = false;
	Object m_sop;

	Camera m_camera;
	std::vector<TextureRect> m_tileSet;
	unsigned int m_menuAtlas;
	short m_offset;
	Vector2f firstLevelPos = { -6.0f, 0.0f };
	Vector4f dimColor = { 0.5f, 0.5f, 0.5f, 1.0f };
	Vector2f labelSize = { 5.0f, 5.0f };

	Vector2f gameLabelSize = { 5.0f, 5.0f };
	Vector2f gameLabelPos = { -5.5f, 1.0f };
	Vector2f creditsSize = { 5.0f, 5.0f };
	Vector2f creditsPos = { 5.4f, -5.0f };

	float yMargain = -0.7f;
	int& currentPosition;
};