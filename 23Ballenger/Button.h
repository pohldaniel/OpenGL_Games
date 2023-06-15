#pragma once

#include <functional>
#include "engine/input/Event.h"
#include "engine/Shader.h"
#include "TextField.h"

class Button : public TextField {

public:

	Button(const CharacterSet& charset);
	Button(Button const& rhs);
	Button& operator=(const Button& rhs);
	~Button();

	//void draw() override;
	void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	/*void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f &position) override;
	void setSize(const float sx, const float sy) override;
	void setSize(const Vector2f &size) override;
	void setOffset(const float ox, const float oy);
	void setOffset(const Vector2f &offset);

	void setOutlineThickness(float thickness);
	void setOutlineColor(const Vector4f &color);
	void setText(std::string text);*/

	void setFunction(std::function<void()> fun);

private:

	/*Shader *m_shader = nullptr;

	Matrix4f m_transformOutline;
	Vector4f m_fillColor = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
	Vector4f m_outlineColor = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);

	Vector4f m_outlineColorDefault = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
	Vector4f m_outlineColorHover = Vector4f(1.0f, 0.0f, 1.0f, 1.0f);
	std::string m_text;
	float m_thickness = 0.0f;*/

	std::function<void()> m_fun;
};