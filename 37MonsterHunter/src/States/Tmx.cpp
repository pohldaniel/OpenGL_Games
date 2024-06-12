#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/TileSet.h>
#include <States/Menu.h>
#include <tmxlite/Map.hpp>
#include <tmxlite/TileLayer.hpp>

#include "Tmx.h"
#include "Application.h"
#include "Globals.h"

Tmx::Tmx(StateMachine& machine) : State(machine, States::TMX) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, 960.0f, 0.0f, 512.0f, -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(2500.0f);

	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClearDepth(1.0f);
	
	TileSetManager::Get().getTileSet("demo").loadTileSetCpu("res/tmx/images/tilemap/tileset.png", true, 64.0f, 64.0f, true, false);
	TileSetManager::Get().getTileSet("demo").loadTileSetCpu("res/tmx/images/tilemap/tileset02.png", false, 32.0f, 32.0f, true, false);
	TileSetManager::Get().getTileSet("demo").loadTileSetGpu();
	m_atlas = TileSetManager::Get().getTileSet("demo").getAtlas();
	//Spritesheet::Safe("tileset", m_atlas);

	loadMap("res/tmx/platform.tmx");

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, 960.0f, 0.0f, 512.0f, -1.0f, 1.0f));
	shader->unuse();
}

Tmx::~Tmx() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Tmx::fixedUpdate() {

}

void Tmx::update() {
	Keyboard &keyboard = Keyboard::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 0.0f, 1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, 0.0f, -1.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_A)) {
		direction += Vector3f(-1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_D)) {
		direction += Vector3f(1.0f, 0.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_W)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_S)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	Mouse &mouse = Mouse::instance();

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(direction * m_dt);
		}
	}
}

void Tmx::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Spritesheet::Bind(m_atlas);
	for (auto cell :  m_cells) {
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - m_camera.getPositionX(), (m_mapHeight - m_tileHeight) - cell.posY - m_camera.getPositionY(), cell.rect.width, cell.rect.height), Vector4f(cell.rect.textureOffsetX, cell.rect.textureOffsetY, cell.rect.textureWidth, cell.rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), cell.rect.frame);
	}
	Batchrenderer::Get().drawBuffer();

	if (m_drawUi)
		renderUi();
}

void Tmx::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Tmx::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void Tmx::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void Tmx::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void Tmx::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Tmx::OnKeyUp(Event::KeyboardEvent& event) {

}

void Tmx::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, 960.0f, 0.0f, 512.0f, -1.0f, 1.0f);

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::Orthographic(0.0f, 960.0f, 0.0f, 512.0f, -1.0f, 1.0f));
	shader->unuse();
}

void Tmx::renderUi() {
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
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Tmx::loadMap(const std::string& path) {
	tmx::Map map;
	map.load(path);

	const tmx::Vector2u& mapSize  = map.getTileCount();
	m_mapHeight = static_cast<float>(mapSize.y * map.getTileSize().y);
	m_tileHeight = static_cast<float>(map.getTileSize().y);

	const std::vector<std::unique_ptr<tmx::Layer>>& layers = map.getLayers();
	int layerId = 0;
	for (auto& layer : layers) {

		const auto layer = dynamic_cast<const tmx::TileLayer*>(layers[layerId].get());
		if (!layer)
			continue;

		m_layer.resize(layers.size() + 1);

		const auto& tileIDs = layer->getTiles();

		
		m_layer[layerId] = new std::pair<int, unsigned int>*[mapSize.y];
		for (int y = 0; y < mapSize.y; ++y)
			m_layer[layerId][y] = new std::pair<int, unsigned int>[mapSize.x];

		for (auto y = 0u; y < mapSize.y; ++y) {
			for (auto x = 0u; x < mapSize.x; ++x) {
				auto idx = y * mapSize.x + x;
				m_layer[layerId][y][x].first = tileIDs[idx].ID - 1;
				if (m_layer[layerId][y][x].first != -1) {

					const TextureRect& rect = TileSetManager::Get().getTileSet("demo").getTextureRects()[m_layer[layerId][y][x].first];

					//std::cout << "Size: " << rect.width << "  " << rect.height << std::endl;

					
					m_cells.push_back({ rect, static_cast<float>(x) * rect.width, static_cast<float>(y) * rect.height });
				}
			}
		}
		layerId++;
	}
}