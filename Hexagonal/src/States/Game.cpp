#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Game.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"

#include "_Game.h"
#include "CreatePrefabStrategies.h"

Game::Game(StateMachine& machine) : State(machine, CurrentState::GAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2), static_cast<float>(Application::Width / 2), -static_cast<float>(Application::Height / 2), static_cast<float>(Application::Height / 2), -1.0f, 1.0f);

	m_camera.lookAt(Vector3f(-800.0f, -800.0f, 0.0f), Vector3f(-800.0f, -800.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));


	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(400.0f);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	m_mainRT.create(Application::Width, Application::Height);
	m_mainRT.attachTexture2D(AttachmentTex::RGBA);
	m_mainRT.attachRenderbuffer(AttachmentRB::DEPTH24);

	TileSet::Get().init(1024u, 1024u);
	loadTileSet("res/tilesetFrames2.bimg");
	loadMap("res/EvilTown2.emap");

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);

	m_atlas = TileSet::Get().getAtlas();
	//Spritesheet::Safe("test", m_atlas);
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
	move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		directrion += Vector3f(0.0f, 1.0f * (1.0f / m_zoomFactor), 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f * (1.0f / m_zoomFactor), 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		directrion += Vector3f(-1.0f * (1.0f / m_zoomFactor), 0.0f, 0.0f);
		m_background.addOffset(-0.001f);
		m_background.setSpeed(-0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		directrion += Vector3f(1.0f * (1.0f / m_zoomFactor), 0.0f, 0.0f);
		m_background.addOffset(0.001f);
		m_background.setSpeed(0.005f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		directrion += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		directrion += Vector3f(0.0f, 0.0f, 1.0f);
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
			m_camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
}

void Game::render() {
	/*glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("quad_array");
	shader->use();
	
	const TextureRect& rect = TileSet::Get().getTextureRects()[11];
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * Matrix4f::Scale(rect.width, rect.height, 0.0f));
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Spritesheet::Bind(m_atlas);

	Globals::shapeManager.get("quad").drawRaw();

	Spritesheet::Unbind();
	shader->unuse();
	glDisable(GL_BLEND);*/

	m_mainRT.bindWrite();
	glClearBufferfv(GL_COLOR, 0, std::vector<float>{0.494f, 0.686f, 0.796f, 1.0f}.data());
	glClearBufferfv(GL_DEPTH, 0, &std::vector<float>{1.0f}[0]);
	//m_background.draw();
	culling();
	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());

	Spritesheet::Bind(m_atlas);
	for (auto cell : m_useCulling ? m_visibleCells : m_cells) {
		if(!cell.selected)
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX * m_zoomFactor + (m_camera.getPositionX() + m_focusPointX) * (1.0f - m_zoomFactor), cell.posY * m_zoomFactor + (m_camera.getPositionY() + m_focusPointY) * (1.0f - m_zoomFactor), cell.rect.width * m_zoomFactor, cell.rect.height * m_zoomFactor), Vector4f(cell.rect.textureOffsetX, cell.rect.textureOffsetY, cell.rect.textureWidth, cell.rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), cell.rect.frame);
	}

	Batchrenderer::Get().drawBufferRaw();
	Spritesheet::Unbind();
	shader->unuse();
	glDisable(GL_BLEND);

	if(m_drawCullingRect)
		drawCullingRect();

	drawMouseRect();
	m_mainRT.unbindWrite();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadBool("u_flip", false);
	m_mainRT.bindColorTexture();
	m_zoomableQuad.drawRaw();
	m_mainRT.unbindColorTexture();
	shader->unuse();

	if (m_drawUi)
		renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);

	if (m_mouseDown) {
		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		m_curMouseX = mouseViewX;
		m_curMouseY = mouseViewY;

	}
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);

		unselect();
		//clip
		//float mouseNDCX = (2.0f * event.x) / static_cast<float>(Application::Width) - 1.0f;
		//float mouseNDCY = 1.0f - (2.0f * event.y) / static_cast<float>(Application::Height);

		//view
		//float mouseViewX = 0.5f * (m_right * (mouseNDCX + 1.0f) + m_left   * (1.0f - mouseNDCX));
		//float mouseViewY = 0.5f * (m_top   * (mouseNDCY + 1.0f) + m_bottom * (1.0f - mouseNDCY));
		
		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		//world with zoom
		float mouseWorldX = mouseViewX + m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float mouseWorldY = mouseViewY + m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

		int row, col;
		isometricToCartesian(mouseWorldX, mouseWorldY, row, col, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
		for (int j = 0; j < m_layer.size(); j++) {
			if(isValid(row, col) && m_layer[j][col][row].first != -1) {
				m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
				m_selectedCells.back().get().selected = true;
			}
		}

		m_mouseDown = true;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;

	}else if (event.button == 2u) {
		unselect();
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Game::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);

		float offsetX = m_zoomFactor * (m_camera.getPositionX() + m_focusPointX) - m_focusPointX;
		float offsetY = m_zoomFactor * (m_camera.getPositionY() + m_focusPointY) - m_focusPointY;

		float mouseViewX = static_cast<float>(event.x);
		float mouseViewY = static_cast<float>(Application::Height - event.y);

		int left = static_cast<int>(floor(std::min(m_mouseX, mouseViewX) / 32.0f) * 32.0f);
		int right = static_cast<int>(ceil(std::max(m_mouseX, mouseViewX) / 32.0f) * 32.0f);
		int bottom = static_cast<int>(ceil(std::min(m_mouseY, mouseViewY) / 16.0f) * 16.0f);
		int top = static_cast<int>(floor(std::max(m_mouseY, mouseViewY) / 16.0f) * 16.0f);

		for (int x = left; x < right; x = x + 32) {
			for (int y = bottom; y < top; y = y + 16) {
				int row, col;
				isometricToCartesian(x + offsetX, y + offsetY, row, col, cellWidth  * m_zoomFactor, cellHeight  * m_zoomFactor);
				for (int j = 0; j < m_layer.size(); j++) {
					if(isValid(row, col) && m_layer[j][col][row].first != -1) {
						m_selectedCells.push_back(m_cells[m_layer[j][col][row].second]);
						m_selectedCells.back().get().selected = true;
					}
				}

			}
		}

		m_mouseDown = false;
		m_mouseX = mouseViewX;
		m_mouseY = mouseViewY;
		m_curMouseX = m_mouseX;
		m_curMouseY = m_mouseY;

	}else if (event.button == 2u) {
		//m_drawUi = true;
		Mouse::instance().detach();
	}
}

void Game::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_zoomFactor = m_zoomFactor - 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}

	if (event.direction == 0u) {
		m_zoomFactor = m_zoomFactor + 0.05f;
		m_zoomFactor = Math::Clamp(m_zoomFactor, 0.2f, 2.5f);
	}
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
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

}

void Game::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_mainRT.resize(Application::Width, Application::Height);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	m_focusPointX = static_cast<float>(Application::Width / 2);
	m_focusPointY = static_cast<float>(Application::Height / 2);
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
		ImGuiID dock_id_right = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Right, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGuiID dock_id_up = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Up, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::Checkbox("Use Culling", &m_useCulling);
	ImGui::Checkbox("Draw Culling Rect", &m_drawCullingRect);
	ImGui::SliderFloat("Screen Border", &m_screeBorder, 0.0f, 450.0f);
	ImGui::SliderFloat("Focus Point X", &m_focusPointX, -1600.0f, 1600.0f);
	ImGui::SliderFloat("Focus Point Y", &m_focusPointY, -1000.0f, 1000.0f);
	ImGui::SliderFloat("Enlarge", &m_enlargeBorder, 0.0f, 600.0f);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Game::unselect() {
	std::for_each(m_selectedCells.begin(), m_selectedCells.end(), [](std::reference_wrapper<Cell> const& cell) {
		cell.get().selected = false;
	});

	m_selectedCells.clear();

}

void Game::loadTileSet(std::string name) {
	std::ifstream readTileSet(name);

	char resourceFilename[MAX_ESTRING_LENGTH];
	while (!readTileSet.eof()) {
		readTileSet >> resourceFilename;

		std::ifstream readImageDef(resourceFilename);
		char textureFilepath[MAX_ESTRING_LENGTH];
		memset(textureFilepath, 0, sizeof(textureFilepath));
		readImageDef.getline(textureFilepath, sizeof(textureFilepath), '\n');

		int imageWidth, imageHeight;
		unsigned char* bytes = Texture::LoadFromFile(textureFilepath, imageWidth, imageHeight, false);

		int accessInt = 0, numFrames = 0;
		readImageDef >> accessInt >> numFrames;

		unsigned int posX = 0, posY = 0, width = 0, height = 0;

		if (numFrames == 0) {
			posX = posY = 0;
			width = imageWidth;
			height = imageHeight;

			m_textureRects.push_back({ static_cast<float>(posX) / static_cast<float>(imageWidth),
                                       static_cast<float>(posY) / static_cast<float>(imageHeight),
                                       static_cast<float>(width) / static_cast<float>(imageWidth),
                                       static_cast<float>(height) / static_cast<float>(imageHeight),
                                       width,
                                       height,
                                       0u });
		}else {

			while (!readImageDef.eof()) {
				readImageDef >> posX >> posY >> width >> height;

				m_textureRects.push_back({ static_cast<float>(posX) / static_cast<float>(imageWidth),
                                           static_cast<float>(posY) / static_cast<float>(imageHeight),
                                           static_cast<float>(width) / static_cast<float>(imageWidth),
                                           static_cast<float>(height) / static_cast<float>(imageHeight),
                                           width,
                                           height,
                                           0u });

				readImageDef.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
			}
		}
		readImageDef.close();

		TileSet::Get().addTexture(bytes, imageWidth, imageHeight, m_textureRects, true);
		free(bytes);
		m_textureRects.clear();
		m_textureRects.shrink_to_fit();
	}
	readTileSet.close();
}

void Game::SkipFileKey(std::ifstream & read) {
	read.ignore(std::numeric_limits<std::streamsize>::max(), ':');
	while (read.peek() == ' ' || read.peek() == '\t')
		read.ignore();
}


void Game::loadMap(std::string name) {
	std::ifstream read(name);
	char buffer[MAX_ESTRING_LENGTH];
	memset(buffer, 0, sizeof(buffer));

	numColumns = 0;
	numRows = 0;
	cellWidth = 0;
	cellHeight = 0;
	numLayers = 0;
	int row = 0;
	int column = 0;

	while (read.peek() == '#')
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

	for (int i = 0; i < 6; ++i) {
		SkipFileKey(read);
		switch (i) {
		case 0: read >> numColumns; break;
		case 1: read >> numRows;	break;
		case 2: read >> cellWidth;	break;
		case 3: read >> cellHeight; break;
		case 4: read >> numLayers;	break;
		case 5: read.getline(buffer, sizeof(buffer), '\n'); break;		// tileSet filename
		}

		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');		// skip the rest of the line (BUGFIX: and the # begin layer def comment line)
	}

	m_layer.resize(numLayers);

	Uint32 layer = 0;
	read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "Layers {"
	read.ignore(1, '\n');													// ignore the '\n' past '{'

	while (read.peek() != '}') {
		row = 0;
		column = 0;
		size_t tallestRenderBlock = 0;
		read.ignore(std::numeric_limits<std::streamsize>::max(), '{');			// ignore up past "layer_# {"
		read.ignore(1, '\n');													// ignore the '\n' past '{'
		m_layer[layer] = new std::pair<int, unsigned int>*[numColumns];
		for (int i = 0; i < numColumns; ++i)
			m_layer[layer][i] = new std::pair<int, unsigned int>[numRows];
																				// read one layer
		while (read.peek() != '}') {
			int tileType = INVALID_ID;
			read >> m_layer[layer][column][row].first;

			m_layer[layer][column][row].first--;

			if (m_layer[layer][column][row].first != -1) {

				const TextureRect& rect = TileSet::Get().getTextureRects()[m_layer[layer][column][row].first];
				float cartX = static_cast<float>(row);
				float cartY = static_cast<float>(column);
				cartesianToIsometric(cartX, cartY, cellWidth, cellHeight);
				m_cells.push_back({ rect, cartX, -cartY , false});
				m_layer[layer][column][row].second = m_cells.size() - 1;
			}

			if (read.peek() == '\n') {
				read.ignore(1, '\n');
				row = 0;
				column++;
			}else if (read.peek() == ',') {
				read.ignore(1, ',');
				row++;
				if (row >= numRows) {
					row = 0;
					column++;
				}
			}
			
			
		}
		read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		++layer;
		
	}
	read.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
void Game::cartesianToIsometric(float & x, float & y, float cellWidth, float cellHeight) {
	float cartX = x * cellWidth;
	float cartY = y * cellHeight;
	x = (cartX - cartY);
	y = (cartX + cartY) * 0.5f;
}

//iso -> cart
//isoX =  (cartX * CELL_WIDTH - cartY * CELL_HEIGHT) 
//isoY = -(cartX * CELL_WIDTH + cartY * CELL_HEIGHT)  * 0.5f
//cartX = (isoX  + cartY * CELL_HEIGHT) / CELL_WIDTH
//cartY = (-2.0 * isoY  - cartX * CELL_WIDTH) / CELL_HEIGHT
//
//cartX = (isoX - 2.0 * isoY - cartX * CELL_WIDTH) / CELL_WIDTH
//cartX =  (0.5f * isoX - isoY) /CELL_WIDTH 
//
//cartY = (-2.0 * isoY  - isoX  - cartY * CELL_HEIGHT) / CELL_HEIGHT
//cartY =-(0.5f * isoX + isoY) / CELL_HEIGHT
void Game::isometricToCartesian(float& x, float& y, float cellWidth, float cellHeight) {
	float isoX = x;
	float isoY = y;

	x =  (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;
}

void Game::isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;

	row = static_cast<int>(std::roundf(x));
	col = static_cast<int>(std::roundf(y )) + 1;
}

void Game::isometricToCartesian(float x, float y, int& row, int& col, float cellWidth, float cellHeight, int min, int max) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	y = -(0.5f * isoX + isoY) / cellHeight;

	row = static_cast<int>(std::roundf(x));
	col = static_cast<int>(std::roundf(y)) + 1;

	row = Math::Clamp(row, min, max);
	col = Math::Clamp(col, min, max);
}

void Game::isometricToRow(float x, float y, int& row,  float cellWidth) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	row = static_cast<int>(std::roundf(x));
}

void Game::isometricToRow(float x, float y, int& row, float cellWidth, int min, int max) {
	float isoX = x;
	float isoY = y;

	x = (0.5f * isoX - isoY) / cellWidth;
	row = static_cast<int>(std::roundf(x));

	row = Math::Clamp(row, min, max);
}

void Game::isometricToCol(float x, float y, int& col, float cellHeight) {
	float isoX = x;
	float isoY = y;

	y = -(0.5f * isoX + isoY) / cellHeight;
	col = static_cast<int>(std::roundf(y)) + 1;
}

void Game::isometricToCol(float x, float y, int& col, float cellHeight, int min, int max) {
	float isoX = x;
	float isoY = y;

	y = -(0.5f * isoX + isoY) / cellHeight;
	col = static_cast<int>(std::roundf(y)) + 1;

	col = Math::Clamp(col, min, max);
}

bool Game::isValid(const int row, const int column) const {
	return (row >= 0 && row < 128 && column >= 0 && column < 128);
}

void Game::culling() {

	Vector3f m_position = m_camera.getPosition();

	corners[0] = Vector2f(m_left  + m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	corners[1] = Vector2f(m_left  + m_screeBorder, m_top - m_screeBorder)    + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	corners[2] = Vector2f(m_right - m_screeBorder, m_top - m_screeBorder)    + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);
	corners[3] = Vector2f(m_right - m_screeBorder, m_bottom + m_screeBorder) + Vector2f(m_zoomFactor * (m_position[0] + m_focusPointX) - m_focusPointX, m_zoomFactor * (m_position[1] + m_focusPointY) - m_focusPointY);

	int rowMin, rowMax, colMin, colMax;
	isometricToCol(corners[0][0] - m_enlargeBorder, corners[0][1] - m_enlargeBorder, colMax, cellHeight * m_zoomFactor, 0, 128);
	isometricToRow(corners[3][0] + m_enlargeBorder, corners[3][1] - m_enlargeBorder, rowMax, cellWidth  * m_zoomFactor, 0, 128);
	isometricToRow(corners[1][0], corners[1][1], rowMin, cellWidth  * m_zoomFactor, 0, 128);
	isometricToCol(corners[2][0], corners[2][1], colMin, cellHeight * m_zoomFactor, 0, 128);
 
	m_visibleCells.clear();
	

	for (int j = 0; j < m_layer.size(); j++) {
		for (int y = colMin; y < colMax; y++) {
			for (int x = rowMin; x < rowMax; x++) {
				if (m_layer[j][y][x].first != -1) {					
					m_visibleCells.push_back(m_cells[m_layer[j][y][x].second]);
				}

			}
		}
	}
}

void Game::drawCullingRect() {
	Vector3f m_position = m_camera.getPosition();
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(m_focusPointX - m_zoomFactor * (m_position[0] + m_focusPointX), m_focusPointY - m_zoomFactor * (m_position[1] + m_focusPointY), 0.0f);

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);

	glVertex3f(corners[0][0], corners[0][1], 0.0f);
	glVertex3f(corners[1][0], corners[1][1], 0.0f);
	glVertex3f(corners[2][0], corners[2][1], 0.0f);
	glVertex3f(corners[3][0], corners[3][1], 0.0f);

	glEnd();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void  Game::drawMouseRect() {
	if (!m_mouseDown) return;
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3f(0.0f, 1.0f, 0.0f);

	glVertex3f(m_mouseX, m_mouseY, 0.0f);
	glVertex3f(m_mouseX, m_curMouseY, 0.0f);
	glVertex3f(m_curMouseX, m_curMouseY, 0.0f);
	glVertex3f(m_curMouseX, m_mouseY, 0.0f);

	glEnd();
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}