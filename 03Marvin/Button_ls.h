#pragma once

#include "engine/Shader.h"
#include "engine/Quad.h"
#include "engine/Spritesheet.h"

#include "Constants.h"

class ButtonLS {
public:
	ButtonLS();
	~ButtonLS();

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

	Shader *m_shader;
	Quad *m_quadPointer;

	std::unordered_map<std::string, unsigned int> m_sprites;
	Spritesheet *m_spriteSheet;
	unsigned int m_textureAtlas;


	Matrix4f m_transform = Matrix4f::IDENTITY;
	Vector2f m_size = Vector2f(725, 75);
	Vector2f m_position;
	Vector2f m_origin;

	Matrix4f m_transformPointer = Matrix4f::IDENTITY;
	Vector2f m_sizePointer = Vector2f(39, 31);
	Vector2f m_positionPointer;

	State m_state = State::ACTIVE;

	std::string m_level;
};