#pragma once
#include <math.h>
#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyboardEventListener.h"
#include "engine/input/Mouse.h"
#include "engine/Transform.h"
#include "StateMachine.h"
#include <UI/Button.h>
#include <UI/SeekerBar.h>
#include <UI/CheckBox.h>

class Settings : public State, public MouseEventListener, public KeyboardEventListener {

public:
	Settings(StateMachine& machine);
	~Settings();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	Button m_button;
	TextField m_headline;
	std::unordered_map<std::string, SeekerBar> m_seekerBars;
	CheckBox m_checkBox;

	float soundVolume;
	float musicVolume;
	bool useSkybox;
};