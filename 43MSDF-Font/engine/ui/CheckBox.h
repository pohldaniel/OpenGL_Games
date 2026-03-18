#pragma once

#include <functional>
#include "../input/Event.h"
#include "../Shader.h"
#include "Widget.h"

class CheckBox : public Widget {

public:

	CheckBox();
	CheckBox(CheckBox const& rhs);
	CheckBox& operator=(const CheckBox& rhs);
	~CheckBox();

	void draw() override;
	virtual void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f& position) override;
	void setSize(const float sx, const float sy) override;
	void setSize(const Vector2f& size) override;
	
	void setOutlineThickness(float thickness);
	void setOutlineColor(const Vector4f& color);
	void setShader(const Shader* shader);
	void setFunction(std::function<void()> fun);
	void setIsChecked(bool isChecked);

	float getTickness();
	const bool isChecked();

protected:

	const Shader *m_shader;

	Matrix4f m_transformOutline;
	Vector4f m_fillColor;
	Vector4f m_outlineColor;

	Vector4f m_outlineColorDefault;
	Vector4f m_outlineColorHover;
	
	float m_thickness = 0.0f;

	std::function<void()> m_fun;
	bool m_isChecked;
};