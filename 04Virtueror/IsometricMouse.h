#pragma once

#include "engine/Vector.h"
#include "engine/Quad.h"
#include "engine/Shader.h"
#include "engine/Spritesheet.h"

#include "Constants.h"

class IsometricMouse {

public:
	IsometricMouse();
	virtual ~IsometricMouse();

	Quad *m_quad;
	Shader *m_shaderArray;
	Matrix4f m_transform = Matrix4f::IDENTITY;
	Spritesheet *m_spriteSheet;

	Vector2f m_size = Vector2f(96.0f, 48.0f);
	Vector2f m_position = Vector2f(0.0f, 0.0f);

	void Render(float x, float y);
};