#include "Game.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {
	const int rendW = 1600;
	const int rendH = 900;

	Camera::CreateDefaultCamera();
	Camera::CreateDummyCamera();

	auto cam = Camera::GetDefaultCamera();
	cam->SetSize(rendW, rendH);
	m_camController = new CameraMapController(cam);


	m_mapLoader.loadLevel("res/maps/40x40-01.map");
	const int mapH = m_mapLoader.GetHeight();
	m_mapLoader.setOrigin(rendW * 0.5, (rendH - mapH) * 0.5);

	const Vector2f p0 = m_mapLoader.GetCellPosition(0, 0);
	const Vector2f p1 = m_mapLoader.GetCellPosition(m_mapLoader.GetNumRows() - 1, 0);
	const Vector2f p2 = m_mapLoader.GetCellPosition(m_mapLoader.GetNumRows() - 1, m_mapLoader.GetNumCols() - 1);
	const Vector2f p3 = m_mapLoader.GetCellPosition(0, m_mapLoader.GetNumCols() - 1);
	const int tileW = TILE_WIDTH;
	const int tileH = TILE_HEIGHT;
	const int marginCameraH = tileW;
	const int marginCameraV = tileH * 2;

	const int cameraL = p1[0] - marginCameraH;
	const int cameraR = p3[0] + tileW + marginCameraH - rendW;
	const int cameraT = p0[1] - marginCameraV;
	const int cameraB = p2[1] + tileH + marginCameraV - rendH;

	m_camController->SetLimits(cameraL, cameraR, cameraT, cameraB);

	CenterCameraOverCell(19, 19);

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

	m_projection = Matrix4f::GetOrthographic(m_projection, 0.0f, static_cast<float>(WIDTH) * 1.0f, 0.0f, static_cast<float>(HEIGHT) * 1.0f, -1000.0f, 1000.0f);
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

	m_camController->Update(m_dt);
}

void Game::render(unsigned int &frameBuffer) {
	const Camera * cam = m_camController->GetCamera();
	m_transform.translate(cam->GetX(), cam->GetY(), 0.0f);

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
	//std::cout << "Game: " << event.x << "  " << event.y << std::endl;

	m_camController->HandleMouseMotion(event);

	const Camera * cam = m_camController->GetCamera();
	const int worldX = cam->GetScreenToWorldX(event.x);
	const int worldY = cam->GetScreenToWorldY(event.y);
}

void Game::CenterCameraOverCell(int row, int col) {
	const Vector2f pos = m_mapLoader.GetCellPosition(row, col);

	const int cX = pos[0] + m_mapLoader.GetTileWidth() * 0.5f;
	const int cY = pos[1] + m_mapLoader.GetTileHeight() * 0.5f;

	m_camController->CenterCameraToPoint(cX, cY);

	const Camera * cam = m_camController->GetCamera();

	m_transform.translate(cam->GetX(), cam->GetY(), 0.0f);
}
