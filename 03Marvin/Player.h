#pragma once


#include "Constants.h"
#include "Entity.h"
#include "ViewEffect.h"

class CharacterControllerCS;

class Player : public Entity{
public:
	Player(const float& dt, const float& fdt);
	~Player();

	void fixedUpdate();
	void update();
	void render();

	void setPosition(const Vector2f &position) override;
	void setPosition(const float x, const float y) override;

private:
	void initAnimations();

	unsigned int *m_textureAtlas;
	unsigned int *m_currentFrame;
	std::unordered_map<std::string, Animator> m_Animations;

	

	Quad *m_quad;
	Shader *m_shaderArray;

	
	
	CharacterControllerCS* m_characterControllerCS;
	void updatePosition(const Vector2f &position);	
};