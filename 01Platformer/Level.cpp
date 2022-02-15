#include "Level.h"

Level::Level() {
	m_shader = new Shader("shader/quad_array.vs", "shader/quad_array.fs");
	m_spriteSheet = new Spritesheet("tileset.png", MAP_TILE_WIDTH, MAP_TILE_HEIGHT, true, true);
	m_quad = new Quad(xScale, yScale);

	loadLevel();
}

void Level::render() {

	glUseProgram(m_shader->m_program);
	for (int y = 0; y < LEVEL_HEIGHT; y++) {
		for (int x = 0; x < LEVEL_WIDTH; x++) {
			if (levelMatrix[y][x] != -1) {
				m_shader->loadMatrix("u_transform", Matrix4f::Translate((x + 0.5f) * xTrans - 1.0f, 1.0f - yTrans * (y + 0.5f), 0.0f));
				m_shader->loadInt("u_layer", levelMatrix[y][x]);
				m_quad->render(m_spriteSheet->getAtlas());
			}

		}
	}
	glUseProgram(0);		
}

Shader* Level::getShader() const {
	return m_shader;
}

void Level::loadLevel() {

	// Default nothing map
	for (int j = 0; j < LEVEL_HEIGHT; j++){
		for (int i = 0; i < LEVEL_WIDTH; i++){
			levelMatrix[j][i] = -1;

		}
	}

	// Load map from file
	std::ifstream myfile("level.txt");
	if (myfile.is_open()) {
		for (int j = 0; j < LEVEL_HEIGHT; j++) {
			for (int i = 0; i < LEVEL_WIDTH; i++) {
				int tmp;
				myfile >> tmp;
				levelMatrix[j][i] = tmp;
			}
		}
	}
}