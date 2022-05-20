#pragma once

#include "engine/Shader.h"
#include "engine/Quad.h"
#include "engine/Spritesheet.h"

#include "Constants.h"

class Button {
public:
	Button();
	~Button();

	void render();
	void update();

	enum State {
		ACTIVE,
		SELECTED,
		DISABLED
	};

	void setState(State state);
	void setPosition(const Vector2f &position);
	void setLevel(const std::string &level);
	std::string getLevel();

private:
	
	Shader *m_shaderArray;
	Quad *m_quad;

	Spritesheet *m_spriteSheet;
	unsigned int m_textureAtlas;

	
	Matrix4f m_transform = Matrix4f::IDENTITY;
	Vector2f m_size = Vector2f(229, 49);
	Vector2f m_position;
	Vector2f m_origin;

	State m_state = State::ACTIVE;

	std::string m_level;
};