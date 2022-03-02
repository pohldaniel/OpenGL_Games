#pragma once
#include <iostream>
#include "Constants.h"
#include "Shader.h"

class Button {
public:
	Button(float sizeX = 1.0f, float sizeY = 1.0f, const Vector4f& color = Vector4f(1.0, 1.0, 1.0, 1.0));
	~Button();

	void render();
	
	void setOutlineColor(const Vector4f &color);
	void setPosition(const Vector2f &position);
	void setOutlineThickness(float thickness);
	void setOrigin(const Vector2f &origin);

private:
	
	Shader *m_shader;
	Shader *m_shaderSingle;

	unsigned int m_vao = 0;
	unsigned int m_quadVBO = 0;

	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f projection = Matrix4f::IDENTITY;

	Matrix4f m_scaleOutline = Matrix4f::IDENTITY;
	Vector4f m_outlineColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;

	float m_thickness = 1.0f;

	float xScaleOutline;
	float yScaleOutline;

	const float xTrans = 2.0f / (float)(WIDTH);
	const float yTrans = 2.0f / (float)(HEIGHT);

};