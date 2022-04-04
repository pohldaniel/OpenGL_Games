#pragma once
#include "Shader.h"
#include "Quad.h"
#include "Text.h"

class TextField {
public:
	TextField() = default;
	TextField(TextField const& rhs);
	TextField& operator=(const TextField& rhs);
	TextField(size_t maxChar);
	TextField(size_t maxChar, const Vector4f& color);
	TextField(size_t maxChar, const Vector4f& color, const Vector2f &position);
	TextField(std::string label);
	TextField(std::string label, const Vector4f& color);
	TextField(std::string label, const Vector4f& color, const Vector2f &position);
	~TextField();

	void render();
	void render(std::string label);
	void setOutlineColor(const Vector4f &color);
	void setFillColor(const Vector4f &color);
	void setTextColor(const Vector4f &color);
	void setPosition(const Vector2f &position);
	void setOutlineThickness(float thickness);
	void setOrigin(const Vector2f &origin);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;

private:
	Shader *m_shader = nullptr;
	Quad *m_quad = nullptr;
	Text* m_text = nullptr;

	unsigned int m_vao = 0;
	unsigned int m_quadVBO = 0;

	Matrix4f m_transform = Matrix4f::IDENTITY;
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;

	Vector4f m_fillColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f m_outlineColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f m_textColor = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);

	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;

	float m_thickness = 0.0f;

	bool m_label = false;
};