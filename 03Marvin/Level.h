#pragma once

#include <iostream>
#include "engine\Vector.h"
#include "engine\Shader.h"
#include "engine\Spritesheet.h"
#include "engine\Quad.h"

#include "Constants.h"
#include "ViewEffect.h"

#include "MapLoader.h"

class Level {
	
public:
	Level(const float& dt, const float& fdt);
	~Level();

	void render();
	void fixedUpdate();
	void update();
	
	void remove();
	void reset();

	Vector2f m_playerPosition;
	MapLoader& m_mapLoader;
private:

	void createBuffer();

	Shader *m_shader;
	Quad *m_quadBackground;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;

	std::unordered_map<std::string, unsigned int> m_sprites;

	
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;

	const float& m_fdt;
	const float& m_dt;

	
};