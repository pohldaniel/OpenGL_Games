#pragma once
#include <iostream>
#include "Constants.h"
#include "Shader.h"
#include "Text.h"

class Button {
public:
	Button(std::string label, float sizeX = 1.0f, float sizeY = 1.0f, const Vector4f& color = Vector4f(1.0, 1.0, 1.0, 1.0));
	~Button();

	void render();
	void update();

	void setOutlineColor(const Vector4f &color);
	void setPosition(const Vector2f &position);
	void setOutlineThickness(float thickness);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;

	const bool pressed();

private:
	
	Shader *m_shader;
	Shader *m_shaderSingle;

	unsigned int m_vao = 0;
	unsigned int m_quadVBO = 0;

	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;

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

	Text* m_text;
};