#pragma once
#include <iostream>
#include <functional>
#include "Constants.h"
#include "Shader.h"
#include "Quad.h"
#include  <algorithm>
#include "Text.h"

class Button {
public:
	Button() = default;
	Button(Button const& other);
	Button& operator=(const Button& rhs);
	Button(std::string label, const Vector4f& color);
	Button(std::string label, const Vector2f &position, const Vector4f& color);
	~Button();

	void render();
	void update();

	void setOutlineColor(const Vector4f &color);
	void setPosition(const Vector2f &position);
	void setOutlineThickness(float thickness);
	void setOrigin(const Vector2f &origin);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;

	const bool pressed();

	void setFunction(std::function<void()> fun);

private:

	Shader *m_shader;
	Quad *m_quad;
	Text* m_text;

	unsigned int m_vao = 0;
	unsigned int m_quadVBO = 0;

	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;

	Vector4f m_fillColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f m_outlineColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f m_outlineColorDefault = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f m_outlineColorHover = Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 255.0f / 255.0f);

	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;

	float m_thickness = 0.0f;

	float xScaleOutline;
	float yScaleOutline;

	bool m_isPressed = false;


	std::function<void()> m_fun = 0;
};