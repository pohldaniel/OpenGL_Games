#pragma once
#include "engine/animation/Animator.h"
#include "engine/Spritesheet.h"
#include "engine/Quad.h"
#include "engine/Shader.h"

#include "Constants.h"
#include "CharacterController_cs.h"

class Marvin  {
public:
	Marvin(const float& dt, const float& fdt);
	~Marvin();

	void fixedUpdate();
	void update();
	void render();

	

private:
	void initAnimations();

	unsigned int *m_textureAtlas;
	unsigned int *m_currentFrame;
	std::unordered_map<std::string, Animator> m_Animations;

	const float& m_fdt;
	const float& m_dt;

	Quad *m_quad;
	Shader *m_shaderArray;

	Vector2f m_playerSize = Vector2f(26.0f, 26.0f);

	Matrix4f m_transform = Matrix4f::IDENTITY;
	Vector2f m_position;
	Vector2f m_origin;
	CharacterControllerCS* m_characterControllerCS;
	void setPosition(const Vector2f &position);
	void setOrigin(const Vector2f &origin);
};