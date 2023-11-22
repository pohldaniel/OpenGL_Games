#pragma once
#include "engine/animation/Animator2D.h"
#include "engine/Spritesheet.h"
#include "engine/Quad.h"
#include "engine/Shader.h"

#include "RenderableObject.h"

class Entity : public RenderableObject {
public:

	Entity(unsigned int category, const float& dt, const float& fdt);
	virtual ~Entity();

	virtual void fixedUpdate() = 0;
	virtual void update() = 0;
	
protected:
	
	const float& m_fdt;
	const float& m_dt;

	unsigned int *m_textureAtlas;
	unsigned int *m_currentFrame;
	std::unordered_map<std::string, Animator2D> m_Animations;

	Quad *m_quad;
	Shader *m_shaderArray;

private:
	virtual void initAnimations() = 0;	
};