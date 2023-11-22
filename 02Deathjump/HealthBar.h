#pragma once
#include "engine/Spritesheet.h"
#include "engine/Shader.h"
#include "engine/Quad.h"

#include "Constants.h"

class HealthBar{
public:
	HealthBar();
	~HealthBar();

	void update(bool x); 
	void render();
	int getHealthState() const;
	void setPosition(const Vector2f &position);
	void setOrigin(const Vector2f &origin);
private:
	Quad *m_quad;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	unsigned int m_textureAtlas;
	unsigned int m_currentFrame = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;	
	Vector2f m_position = Vector2f(0.0f, 0.0f);
	Vector2f m_origin = Vector2f(0.0f, 0.0f);
	Vector2f m_size = Vector2f(96.0f, 32.0f);

	void initSprites();
};