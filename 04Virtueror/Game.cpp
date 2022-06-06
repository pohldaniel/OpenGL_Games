#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	
	m_mapLoader.loadLevel("res/maps/40x40-01.map");
	short stride = 5, offset = 3;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboMap);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_mapLoader.m_vertices.size() * sizeof(float), &m_mapLoader.m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//map indices
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMap);
	glBufferData(GL_ARRAY_BUFFER, m_mapLoader.m_indexMap.size() * sizeof(m_mapLoader.m_indexMap[0]), &m_mapLoader.m_indexMap[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_mapLoader.m_indexBuffer.size() * sizeof(unsigned int), &m_mapLoader.m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("tiles");
	m_shaderLevel = Globals::shaderManager.getAssetPointer("level");

	m_projection = Matrix4f::GetOrthographic(m_projection, -static_cast<float>(WIDTH) * 0.5f, static_cast<float>(WIDTH) * 0.5f, -static_cast<float>(HEIGHT) * 0.5f, static_cast<float>(HEIGHT) * 0.5f, -1000.0f, 1000.0f);

	Matrix4f trans;
	trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
	m_transform = trans;
}

Game::~Game() {
}

void Game::fixedUpdate() {
}

void Game::update() {

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyDown(Keyboard::KEY_A)) {
		m_offsetX = m_offsetX + 0.1f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		m_offsetX = m_offsetX - 0.1f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		m_offsetY = m_offsetY - 0.2f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		m_offsetY = m_offsetY + 0.2f;

		Matrix4f trans;
		trans.translate(m_offsetX * TILE_WIDTH, m_offsetY * TILE_HEIGHT, 0.0f);
		m_transform = trans;
	}
}

void Game::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClearColor(0.06f, 0.06f, 0.06f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);
	glUseProgram(m_shaderLevel->m_program);
	m_shaderLevel->loadMatrix("u_transform", m_transform * m_projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_spriteSheet->getAtlas());
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_mapLoader.m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glUseProgram(0);
	glEnable(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	std::cout << "Game: " << event.x << "  " << event.y << std::endl;
}
