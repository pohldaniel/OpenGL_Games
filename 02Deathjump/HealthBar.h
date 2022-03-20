#pragma once

#include "Constants.h"
#include "Spritesheet.h"
#include "Shader.h"
#include "Quad.h"
class HealthBar{
public:
	HealthBar();
	~HealthBar();

	void update(bool x); 
	void render();
	//int getHealthState() const;
	void setPosition(const Vector2f &position);

private:
	Quad *m_quad;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	unsigned int m_textureAtlas;
	unsigned int m_currentFrame = 0;
	Matrix4f m_transform = Matrix4f::IDENTITY;	
	Vector2f m_position;
	Vector2f m_origin;
	Vector2f m_size = Vector2f(96.0f, 32.0f);

	void initSprites();



};