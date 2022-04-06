#pragma once
#include <iostream>
#include <functional>
#include "engine/Shader.h"
#include "engine/Quad.h"
#include "engine/ui/Text.h"
#include "Constants.h"

class Button {

public:
	Button();
	Button(Button const& rhs);
	Button& operator=(const Button& rhs);
	Button(std::string label, const Vector4f& color, const bool clickSafe = true);
	Button(std::string label, const Vector2f &position, const Vector4f& color, const bool clickSafe = true);
	~Button();

	void render();
	std::function<void()> update = 0;

	void setOutlineColor(const Vector4f &color);
	void setPosition(const Vector2f &position);
	void setOutlineThickness(float thickness);
	void setOrigin(const Vector2f &origin);

	const Vector2f &getPosition() const;
	const Vector2f &getSize() const;

	const bool isPressed();

	void setFunction(std::function<void()> fun);

	SoundEffectsPlayer m_effectsPlayer;
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
	Vector4f m_outlineColorDefault = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
	Vector4f m_outlineColorHover = Vector4f(100.0f / 255.0f, 100.0f / 255.0f, 100.0f / 255.0f, 1.0f);

	Vector2f m_position;
	Vector2f m_size;
	Vector2f m_origin;

	float m_thickness = 0.0f;

	bool m_isPressed = false;
	bool m_clickSafe = false;
	bool m_guard = true;

	std::function<void()> m_fun = 0;
	
	void click();
	void clickSafe();		
};