#pragma once

#include <functional>
#include "engine/input/Event.h"
#include "engine/Shader.h"
#include "TextField.h"

class Button : public TextField {

public:

	Button();
	Button(Button const& rhs);
	Button& operator=(const Button& rhs);
	~Button();

	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;
	void setFunction(std::function<void()> fun);

private:

	std::function<void()> m_fun;
};