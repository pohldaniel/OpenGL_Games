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
	//m_camera.orthographic(-static_cast<float>(Application::Width / 2), static_cast<float>(Application::Width / 2), -static_cast<float>(Application::Height / 2), static_cast<float>(Application::Height / 2), -1000.0f, 1000.0f);
	
	//m_camera.lookAt(Vector3f(0.0f, -800.0f, 0.0f), Vector3f(0.0f, -800.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 10.0f), Vector3f(0.0f, 0.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));	

	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(400.0f);

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

	game->InitSystem();

	auto windowSize = game->GetRenderer().ViewArea();
	camera.Init(eVec2((float)windowSize.w, (float)windowSize.h), vec2_zero);
	game->GetRenderer().RegisterCamera(&camera);
	game->GetEntityPrefabManager().SetCreatePrefabStrategy(std::make_shared<eCreateEntityPrefabUser>());

	map.SetViewCamera(&camera);
	//player.SetMap(&map);
	map.Init();


	data = (unsigned char*)malloc(Application::Width * Application::Height * 4);

	TileSet::Get().init("set_1", 1024u, 1024u);
	loadTileSet("res/tilesetFrames2.bimg");

	m_atlas = TileSet::Get().getAtlas();
	//Spritesheet::Safe("test", m_atlas);
}

Game::~Game() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	free(data);
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

	/*map.Draw();
	game->GetRenderer().Flush();

	game->GetRenderer().ReadPixels(data);
	m_pixelbuffer.mapData(m_texture, data);

	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", Matrix4f::IDENTITY);
	shader->loadMatrix("u_view", Matrix4f::IDENTITY);
	shader->loadInt("u_texture", 0);
	m_texture.bind();

	Globals::shapeManager.get("quad").drawRaw();

	m_texture.unbind();*/

	auto shader = Globals::shaderManager.getAssetPointer("quad_array");
	shader->use();
	
	const TextureRect& rect = TileSet::Get().getTextureRects().back();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * Matrix4f::Translate(800.0f, 450.0f, 0.0f) * Matrix4f::Scale(rect.width, rect.height, 0.0f));
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY,  rect.textureWidth,  rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Spritesheet::Bind(m_atlas);

	Globals::shapeManager.get("quad").drawRaw();

	Spritesheet::Unbind();

	//if (m_drawUi)
		//renderUi();	

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
		ImGui::DockBuilderDockWindow("Settings", dock_id_left);
	}

	// render widgets
	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Checkbox("Draw Wirframe", &StateMachine::GetEnableWireframe());
	
	
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
		unsigned char* bytes = Texture::LoadFromFile(textureFilepath, imageWidth, imageHeight);

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

		TileSet::Get().addTexture(bytes, imageWidth, imageHeight, m_textureRects);
		free(bytes);
	}
	readTileSet.close();
}