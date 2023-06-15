#pragma once

#include <functional>
#include "engine/input/Event.h"
#include "engine/CharacterSet.h"
#include "engine/Shader.h"
#include "Widget.h"

class TextField : public Widget {

public:

	TextField(const CharacterSet& charset);
	TextField(TextField const& rhs);
	TextField& operator=(const TextField& rhs);
	~TextField();

	void draw() override;
	virtual void processInput(const int mouseX, const int mouseY, const Event::MouseButtonEvent::MouseButton button = Event::MouseButtonEvent::MouseButton::NONE) override;

	void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f& position) override;
	void setSize(const float sx, const float sy) override;
	void setSize(const Vector2f& size) override;
	void setOffset(const float ox, const float oy);
	void setOffset(const Vector2f& offset);
	void setPadding(const float px, const float py);
	void setPadding(const Vector2f& padding);

	void setOutlineThickness(float thickness);
	void setOutlineColor(const Vector4f& color);
	void setText(std::string text);
	void setShader(Shader* shader);

protected:

	Shader *m_shader;

	Matrix4f m_transformOutline;
	Vector4f m_fillColor;
	Vector4f m_outlineColor;

	Vector4f m_outlineColorDefault;
	Vector4f m_outlineColorHover;	
	Vector2f m_offset;
	Vector2f m_padding;
	std::string m_text;

	float m_thickness = 0.0f;

	const CharacterSet& charset;

};