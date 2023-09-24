#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

constexpr float DEF_SPEED = 400.f;
constexpr int SCROLL_L = 1;
constexpr int SCROLL_R = -1;
constexpr int SCROLL_U = 1;
constexpr int SCROLL_D = -1;
constexpr int NO_SCROLL = 0;

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{ 
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	float pointX;
	float pointY;

	float pointXTrans;
	float pointYTrans;

	for (int i = 4; i >= 0; i--) {
		for (int j = 4; j >= 0; j--) {
			pointX = j * 0.5f;
			pointY = i * 0.5f;

			pointXTrans = pointX - pointY;
			pointYTrans = (pointX + pointY) * ((float)TILE_WIDTH / TILE_HEIGHT) * 0.5f;

			m_quads.push_back(new Quad(false, pointXTrans, pointXTrans + 1.0f, pointYTrans, pointYTrans + 1.0f, TILE_WIDTH, TILE_HEIGHT, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0));
		}
	}

	for (int i = 4; i >= 0; i--) {
		for (int j = 4; j >= 0; j--) {
			pointX = -j;
			pointY = i;
			m_quads2.push_back(new Quad(false, pointX, pointX + 1.0f, pointY, pointY + 1.0f, TILE_WIDTH * scale + dist, TILE_WIDTH * scale + dist, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0));
		}
	}

	m_shader = Globals::shaderManager.getAssetPointer("quad");
	m_shaderArray = Globals::shaderManager.getAssetPointer("quad_array");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("tiles");
	m_spriteSheet2 = Globals::spritesheetManager.getAssetPointer("tiles2");

	m_projection = Matrix4f::Orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);

	std::fstream fs("res/maps/40x40-01.map");
	if (!fs.is_open())
		return;

	std::string line;
	std::istringstream ss;

	// reading map size
	std::getline(fs, line);
	ss.str(line);

	unsigned int rows = 0;
	unsigned int cols = 0;
	ss >> rows >> cols;

	m_cols = cols;
	m_rows = rows;

	const int mapSize = rows * cols;
	tiles.reserve(mapSize);
	tiles.assign(mapSize, { Vector2f(),Vector2f(), 0 });

	// READ BASE MAP
	for (int r = 0; r < rows; ++r) {
		std::getline(fs, line);
		ss.clear();
		ss.str(line);

		const unsigned int row = r;
		const unsigned int ind0 = row * cols;

		for (int c = 0; c < cols; ++c) {
			unsigned int type;

			ss >> type;

			const unsigned int index = ind0 + c;
			pointX = c * (float)(TILE_WIDTH) * 0.5f;
			pointY = row * (float)(TILE_WIDTH) * 0.5f;
			pointXTrans = (pointX - pointY);
			pointYTrans = (pointX + pointY) * 0.5f;

			tiles[index] = { Vector2f(pointXTrans, -pointYTrans),Vector2f(TILE_WIDTH, TILE_HEIGHT), type };
		}
	}
	fs.close();

	m_vertices.clear();
	m_indexBuffer.clear();
	m_indexMap.clear();

	for (int i = 0; i < tiles.size(); i++) {
		addTile(tiles[i], m_vertices, m_indexBuffer, m_indexMap);
	}
	setOrigin(Application::Width * 0.5f - (float)(TILE_WIDTH / 2), Application::Height *  0.5f - (float)(TILE_HEIGHT / 0.5f));
	createBuffer();

	m_shaderLevel = Globals::shaderManager.getAssetPointer("level");

	setLimits(-1216, 1216, -1642, -430);
	CenterCameraOverCell(0, 39);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Game::fixedUpdate() {

}

void Game::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f directrion = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f, 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f, 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xPosRelative();
		dy = mouse.yPosRelative();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			//m_camera.rotate(dx, dy);

		}

		if (move) {
			//m_camera.move(directrion * 5.0f * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
	update2(m_dt);
}

void Game::render() {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//m_background.draw();

	/*glEnable(GL_BLEND);
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadInt("u_layer", 1);
	m_shaderArray->loadMatrix("u_transform", m_projection);
	for (int i = 0; i < m_quads.size(); i++) {
		m_quads[i]->draw(m_spriteSheet2->getAtlas(), true);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);

	if (m_drawUi)
		renderUi();*/

	glEnable(GL_BLEND);
	glUseProgram(m_shaderLevel->m_program);
	m_shaderLevel->loadMatrix("u_transform", m_projection * m_camera.getViewMatrix());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_spriteSheet2->getAtlas());
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glUseProgram(0);
	glDisable(GL_BLEND);
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//m_drawUi = false;
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Game::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}

	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyDown(Keyboard::KEY_A) || keyboard.keyDown(Keyboard::KEY_LEFT)) {
		if (!mMouseScrollX) {
			mDirX = SCROLL_R;
			mKeyScrollX = true;
		}
	}else if (keyboard.keyDown(Keyboard::KEY_D) || keyboard.keyDown(Keyboard::KEY_RIGHT)) {
		if (!mMouseScrollX) {
			mDirX = SCROLL_L;
			mKeyScrollX = true;
		}
	}else if (keyboard.keyDown(Keyboard::KEY_W) || keyboard.keyDown(Keyboard::KEY_UP)) {

		if (!mMouseScrollY) {
			mDirY = SCROLL_U;
			mKeyScrollY = true;
		}
	}else if (keyboard.keyDown(Keyboard::KEY_S) || keyboard.keyDown(Keyboard::KEY_DOWN)) {
		if (!mMouseScrollY) {
			mDirY = SCROLL_D;
			mKeyScrollY = true;
		}
	}
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {
	mDirX = NO_SCROLL;
	mDirY = NO_SCROLL;
	mKeyScrollX = false;
	mKeyScrollY = false;
}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Game::applyTransformation(TrackBall& arc) {
	m_transform.fromMatrix(arc.getTransform());
}

void Game::renderUi() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
		ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("InvisibleWindow", nullptr, windowFlags);
	ImGui::PopStyleVar(3);

	ImGuiID dockSpaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
	ImGui::End();

	if (m_initUi) {
		m_initUi = false;
		ImGuiID dock_id_left = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Left, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Debug Physic", &m_debugPhysic);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices) {
	Vector2f pos = tile.position;
	float w = tile.size[0];
	float h = tile.size[1];

	vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
	vertices.push_back(pos[0]); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

	mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid);

	unsigned int currentOffset = (vertices.size() / 5) - 4;

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

void Game::createBuffer() {
	short stride = 5, offset = 3;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboMap);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//map indices
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMap);
	glBufferData(GL_ARRAY_BUFFER, m_indexMap.size() * sizeof(m_indexMap[0]), &m_indexMap[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Game::update2(float delta) {

	const float movX = mDirX * mSpeed * delta;

	if (mDirX < 0) {
		const int newX = static_cast<int>(movX - 0.5f) + m_camera.getPositionX();

		if (newX < mLimitL) {
			m_camera.setPositionX(mLimitL);
		}else {
			m_camera.moveX(movX);
		}
	}
	else if (mDirX > 0) {
		const int newX = static_cast<int>(movX + 0.5f) + m_camera.getPositionX();

		if (newX > mLimitR) {
			m_camera.setPositionX(mLimitR);
		}else {
			m_camera.moveX(movX);
		}
	}

	// VERTICAL
	const float movY = mDirY * mSpeed * delta;

	if (mDirY < 0) {
		const int newY = static_cast<int>(movY - 0.5f) + m_camera.getPositionY();

		if (newY < mLimitT) {
			m_camera.setPositionY(mLimitT);
		}else {
			m_camera.moveY(movY);
		}
	}
	else if (mDirY > 0) {
		const int newY = static_cast<int>(movY + 0.5f) + m_camera.getPositionY();

		if (newY > mLimitB) {
			m_camera.setPositionY(mLimitB);
		}else {
			m_camera.moveY(movY);
		}
	}
}

void Game::setLimits(int l, int r, int t, int b) {
	mLimitL = l;
	mLimitR = r;
	mLimitT = t;
	mLimitB = b;
}

void Game::CenterCameraOverCell(int row, int col) {
	const Vector2f pos = GetCellPosition(row, col);
	const int cX = pos[0] + TILE_WIDTH * 0.5f;
	const int cY = pos[1] + TILE_HEIGHT * 0.5f;
	CenterCameraToPoint(cX, cY);
}

void Game::CenterCameraToPoint(int x, int y) {
	const int hw = Application::Width / 2;
	const int hh = Application::Height / 2;
	const int cameraX0 = x - hw;
	const int cameraY0 = y - hh;

	// clamp X
	if (cameraX0 < mLimitL)
		x = mLimitL + hw;
	else if (cameraX0 > mLimitR)
		x = mLimitR + hw;

	// clamp Y
	if (cameraY0 < mLimitT)
		y = hh - mLimitT;
	else if (cameraY0 > mLimitB)
		y = hh - mLimitB;

	CenterToPoint(x, y);
}

void Game::CenterToPoint(int x, int y) {
	float mXf = x - Application::Width / 2;
	float mYf = y - Application::Height / 2;

	int mXd = std::roundf(mXf);
	int mYd = std::roundf(mYf);

	m_camera.setPosition(mXf, mYf, 0.0f);
}

Vector2f Game::GetCellPosition(unsigned int r, unsigned int c) const {
	const unsigned int ind = r * m_cols + c;
	return GetCellPosition(ind);
}

Vector2f Game::GetCellPosition(unsigned int index) const {

	if (index < tiles.size()) {
		return tiles[index].position;
	}else {
		const Vector2f p(-1, -1);
		return p;
	}
}

void Game::setOrigin(const float x, const float y) {
	m_origin[0] = x;
	m_origin[1] = y;
	updateTilePositions();
}

void Game::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_origin[0] = origin[0];
	updateTilePositions();
}

void Game::updateTilePositions() {

	m_vertices.clear();
	m_indexBuffer.clear();
	m_indexMap.clear();

	for (int i = 0; i < tiles.size(); i++) {
		tiles[i].position[0] += m_origin[0];
		tiles[i].position[1] += m_origin[1];
		addTile(tiles[i], m_vertices, m_indexBuffer, m_indexMap);
	}
}