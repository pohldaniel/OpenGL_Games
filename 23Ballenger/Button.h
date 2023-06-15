#pragma once
#include <iostream>
#include <functional>

#include "engine/input/Event.h"
#include "engine/Shader.h"
#include "Widget.h"

class Button : public Widget{

public:
	Button();
	Button(Button const& rhs);
	Button& operator=(const Button& rhs);
	~Button();

	void draw() override;
	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	std::function<void()> update = 0;

	void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f &position) override;
	void setSize(const float sx, const float sy) override;
	void setSize(const Vector2f &size) override;
	void setOutlineColor(const Vector4f &color);
	void setOutlineThickness(float thickness);

	void setFunction(std::function<void()> fun);

private:

	Shader *m_shader = nullptr;

	Matrix4f m_transformOutline;
	Vector4f m_fillColor = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
	Vector4f m_outlineColor = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);

	Vector4f m_outlineColorDefault = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
	Vector4f m_outlineColorHover = Vector4f(1.0f, 0.0f, 1.0f, 1.0f);

	float m_thickness = 0.0f;
	std::function<void()> m_fun = 0;
};