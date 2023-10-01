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
	//m_camera.lookAt(Vector3f(-713.0f,  -2155.0f, 0.0f), Vector3f(-713.0f, -2155.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));


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

	m_pixelbuffer.create(Application::Width * Application::Height * 4);
	m_texture.createEmptyTexture(Application::Width, Application::Height, GL_RGBA8, GL_RGBA);
	m_texture.setWrapMode(GL_CLAMP);
	m_texture.setFilter(GL_NEAREST);

	TileSet::Get().init(1024u, 1024u);
	loadTileSet("res/tilesetFrames2.bimg");
	loadMap("res/EvilTown2.emap");

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
		directrion += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		directrion += Vector3f(0.0f, -1.0f, 0.0f);
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
			m_camera.move(directrion * m_dt);
		}
	}
	m_trackball.idle();
	m_transform.fromMatrix(m_trackball.getTransform());

	m_background.update(m_dt);
}

void Game::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//m_background.draw();

	/*glEnable(GL_BLEND);
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

	glEnable(GL_BLEND);
	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());

	Spritesheet::Bind(m_atlas);

	for (auto cell : m_cells) {
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, cell.rect.width, cell.rect.height), Vector4f(cell.rect.textureOffsetX, cell.rect.textureOffsetY, cell.rect.textureWidth, cell.rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), cell.rect.frame);	
	}

	Batchrenderer::Get().drawBufferRaw();
	Spritesheet::Unbind();
	shader->unuse();
	glDisable(GL_BLEND);

	//culling();

	//if (m_drawUi)
	//	renderUi();
}

void Game::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Game::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {		
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);


		float cursorPosNDCX = (2.0f * event.x) / (float)Application::Width - 1.0f;
		float cursorPosNDCY = 1.0f - (2.0f * event.y) / (float)Application::Height;
		Vector4f m_cursorPosEye = Vector4f(cursorPosNDCX, cursorPosNDCY, -1.0f, 1.0f) ^ m_camera.getInvOrthographicMatrixNew();
		
		//world
		m_cursorPosEye[0] = m_cursorPosEye[0] + m_camera.getPositionX();
		m_cursorPosEye[1] = m_cursorPosEye[1] + m_camera.getPositionY();

		
		//iso -> cart
		//posX = (pointX - pointY) * CELL_WIDTH
		//posY = -(pointX + pointY) * CELL_HEIGHT * 0.5f
		//pointX = posX / CELL_WIDTH + pointY
		//pointY = - 2.0 * posY /CELL_HEIGHT - pointX
		//
		//pointX = posX / CELL_WIDTH - 2.0 * posY /CELL_HEIGHT - pointX
		//pointX = 0.5f * posX / CELL_WIDTH - posY /CELL_HEIGHT 
		//pointX = (0.5f * posX - posY) /CELL_WIDTH									(CELL_WIDTH = CELL_HEIGHT)
		//
		//pointY = - 2.0 * posY /CELL_HEIGHT - (0.5f * posX - posY) /CELL_WIDTH
		//pointY =-(0.5f * posX + posY)/CELL_HEIGHT
		//pointY =-(0.5f * posX + posY - CELL_HEIGHT)/CELL_HEIGHT					(shift one Cell matching OpenGL)
	
		const float w = 32.0f;
		const float h = 32.0f;
		
		float row = (0.5f * m_cursorPosEye[0]  - m_cursorPosEye[1]) / w;
		float col = -(0.5f * m_cursorPosEye[0] + m_cursorPosEye[1]) / h + 1.0f;

		std::cout << "Mouse X:" << std::round(row) << " Mouse Y: " << std::round(col)  << std::endl;

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
}

void Game::OnKeyUp(Event::KeyboardEvent& event) {

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
		ImGuiID dock_id_down = ImGui::DockBuilderSplitNode(dockSpaceId, ImGuiDir_Down, 0.2f, nullptr, &dockSpaceId);
		ImGui::DockBuilderDockWindow("Settings", dock_id_down);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	ImGui::SliderFloat("Screen Border", &m_screeBorder, 0.0f, 500.0f);
	
	ImGui::SliderFloat("Scale", &m_scale, 0.0f, 5.0f);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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
		m_layer[layer] = new unsigned int*[numColumns];
		for (int i = 0; i < numColumns; ++i)
			m_layer[layer][i] = new unsigned int[numRows];
																				// read one layer
		while (read.peek() != '}') {
			int tileType = INVALID_ID;
			read >> m_layer[layer][column][row];

			m_layer[layer][column][row]--;

			if (m_layer[layer][column][row] != -1 && layer == 3) {

				const TextureRect& rect = TileSet::Get().getTextureRects()[m_layer[layer][column][row]];
				eVec2 cellMins = eVec2((float)(row * cellWidth), (float)(column * cellHeight));
				eBounds bounds = eBounds(cellMins, cellMins + eVec2((float)cellWidth, (float)cellHeight));
				eVec2& origin = bounds[0];

				eMath::CartesianToIsometric(origin.x, origin.y);
				m_cells.push_back({ rect, origin.x, -origin.y });

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

void IsometricToCartesian(float & x, float & y) {
	float isoX = x;
	float isoY = y;
	x = (2.0f * isoY + isoX) * 0.5f;
	y = (2.0f * isoY - isoX) * 0.5f;
}

void Game::culling() {

	std::array<Vector2f, 4> corners;
	Vector3f m_position = m_camera.getPosition();

	corners[0] = Vector2f( m_left + m_screeBorder,  m_bottom + m_screeBorder);
	corners[1] = Vector2f(m_left + m_screeBorder, m_top - m_screeBorder);
	corners[2] = Vector2f( m_right - m_screeBorder,  m_top - m_screeBorder);
	corners[3] = Vector2f( m_right - m_screeBorder, m_bottom + m_screeBorder);
	
	IsometricToCartesian(corners[0][0], corners[0][1]);
	IsometricToCartesian(corners[1][0], corners[1][1]);
	IsometricToCartesian(corners[2][0], corners[2][1]);
	IsometricToCartesian(corners[3][0], corners[3][1]);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);
	glColor3f(1.0f, 0.0f, 0.0f);

	float xpos = corners[0][0];
	float ypos = corners[1][1];
	float w = corners[2][0];
	float h = corners[3][1];


	/*glVertex3f(xpos, ypos, 0.0f);
	glVertex3f(xpos, (ypos + h), 0.0f);
	glVertex3f(xpos + w, (ypos + h), 0.0f);
	glVertex3f(xpos + w, ypos, 0.0f);

	glVertex3f(corners[0][0], corners[0][1], 0.0f);
	glVertex3f(corners[1][0], corners[1][1], 0.0f);
	glVertex3f(corners[2][0], corners[2][1], 0.0f);
	glVertex3f(corners[3][0], corners[3][1], 0.0f);*/

	float left = m_left * m_scale;
	float right = m_right * m_scale;
	float top = m_top * m_scale;
	float bottom = m_bottom * m_scale;

	glVertex3f(-left, (top - bottom) * 0.5f, 0.0f);
	glVertex3f((right - left) * 0.5f, top, 0.0f);
	glVertex3f(right, (top - bottom) * 0.5f, 0.0f);
	glVertex3f((right - left) * 0.5f, bottom, 0.0f);

	glEnd();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}