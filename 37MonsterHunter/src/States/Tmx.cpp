#include <numeric>
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

	m_viewWidth = 1280.0f;
	m_viewHeight= 720.0f;
	m_movingSpeed = 250.0f;
	frameCount = 4;
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(m_movingSpeed);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClearDepth(1.0f);

	/*TextureAtlasCreator::Get().init(1536u, 768u);
	for (unsigned int x = 0; x < 24; x++) {
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 0u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 3u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 6u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 9u, 64u, 64u, false, false);

		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 1u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 4u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 7u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 10u, 64u, 64u, false, false);

		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 2u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 5u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 8u, 64u, 64u, false, false);
		TileSetManager::Get().getTileSet("coast").loadTileCpu("res/tmx/graphics/tilesets/coast.png", false, x * 64u, 64u * 11u, 64u, 64u, false, false);
	}
	TileSetManager::Get().getTileSet("coast").loadTileSetGpu();
	Spritesheet::Safe("res/tmx/graphics/tilesets/coast_ordered", TileSetManager::Get().getTileSet("coast").getAtlas());*/

	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/tilesets/world.png", true, 64.0f, 64.0f, true, false);
	
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/house_small_alt.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/ice_tree.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/palm.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/palm_alt.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/palm_small.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/ruin_pillar.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/ruin_pillar_broke.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/ruin_pillar_broke_alt.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/teal_tree.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/teal_tree_bushy.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/teal_tree_small.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/arean_fire.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/arena_plant.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/arena_water.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/green_tree.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/green_tree_bushy.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/green_tree_small.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/hospital.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/house_large.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/house_large_alt.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/house_small.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/gate_pillar.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/gate_top.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/grassrock1.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/grassrock2.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/icerock1.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/icerock2.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/sandrock1.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/sandrock2.png", false, true, false);

	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/grass.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/grass_ice.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/objects/sand.png", false, true, false);
	
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/tilesets/water/0.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/tilesets/water/1.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/tilesets/water/2.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/tilesets/water/3.png", false, true, false);

	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/tilesets/coast_ordered.png", false, 64.0f, 64.0f, true, false);

	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/player.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/blond.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/fire_boss.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/grass_boss.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/hat_girl.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/purple_girl.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/straw.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/water_boss.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/young_girl.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetCpu("res/tmx/graphics/characters/young_guy.png", false, 128.0f, 128.0f, true, false);
	TileSetManager::Get().getTileSet("world").loadTileSetGpu();
	m_atlasWorld = TileSetManager::Get().getTileSet("world").getAtlas();
	//Spritesheet::Safe("world", m_atlasWorld);
	
	loadMap("res/tmx/data/maps/world.tmx");
	m_camera.lookAt(Vector3f(4223.94f - m_viewWidth * 0.5f, m_mapHeight - 3661.26f - 0.5f * m_viewHeight, 0.0f), Vector3f(4223.94f - m_viewWidth * 0.5f, m_mapHeight - 3661.26f - 0.5f * m_viewHeight, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));


	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, sizeof(PointVertex) * MAX_POINTS, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(PointVertex), (void*)offsetof(PointVertex, position));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_POINT_SPRITE);

	pointBatch = new PointVertex[MAX_POINTS];
	pointBatchPtr = pointBatch;
}

Tmx::~Tmx() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	delete[] pointBatch;
	pointBatch = nullptr;
	pointBatchPtr = nullptr;
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
			Cell& cell = m_cellsMain[m_playerIndex];

			cell.posX += direction[0] * m_dt * m_movingSpeed;
			cell.posY -= direction[1] * m_dt * m_movingSpeed;

			cell.centerX = cell.posX + 64.0f;
			cell.centerY = cell.posY - 64.0f;
		}
	}


	culling();

	elapsedTime += 6.0f * m_dt;
	currentFrame = static_cast <int>(std::floor(elapsedTime));
	if (currentFrame > frameCount - 1) {
		currentFrame = 0;
		elapsedTime -= static_cast <float>(frameCount);
	}

	for (AnimatedCell& animatedCell : m_visibleCellsAni) {
		animatedCell.currentFrame = currentFrame;
	}
	
	std::sort(m_visibleCellsMain.begin(), m_visibleCellsMain.end(), [&](const Cell& cell1, const Cell& cell2) {return cell1.centerY < cell2.centerY; });
}

void Tmx::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Spritesheet::Bind(m_atlasWorld);
	
	for (auto& animatedCell : m_visibleCellsAni) {
		const TextureRect& rect = TileSetManager::Get().getTileSet("world").getTextureRects()[animatedCell.currentFrame + animatedCell.startFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(animatedCell.posX - m_camera.getPositionX(), m_mapHeight - 64.0f - animatedCell.posY - m_camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}
	
	for (auto& cell : m_visibleCellsBG) {
		const TextureRect& rect = TileSetManager::Get().getTileSet("world").getTextureRects()[cell.currentFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - m_camera.getPositionX(), m_mapHeight - 64.0f - cell.posY - m_camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	for (auto& cell : m_visibleCellsMain) {
		const TextureRect& rect = TileSetManager::Get().getTileSet("world").getTextureRects()[cell.currentFrame];
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - m_camera.getPositionX(), m_mapHeight - cell.posY - m_camera.getPositionY(), rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	}

	Batchrenderer::Get().drawBuffer();
	if(m_useCulling && m_drawScreenBorder)
		drawCullingRect();

	if (m_drawCenter) {
		updatePoints();
		glBindVertexArray(m_vao);
		GLsizeiptr size = (uint8_t*)pointBatchPtr - (uint8_t*)pointBatch;
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, pointBatch);

		auto shader = Globals::shaderManager.getAssetPointer("color");
		shader->use();
		shader->loadMatrix("u_projection", m_camera.getOrthographicMatrix());

		glDrawArrays(GL_POINTS, 0, m_pointCount);
		shader->unuse();

		pointBatchPtr = pointBatch;
		m_pointCount = 0;
	}

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
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);

	m_left = m_camera.getLeftOrthographic();
	m_right = m_camera.getRightOrthographic();
	m_bottom = m_camera.getBottomOrthographic();
	m_top = m_camera.getTopOrthographic();

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
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
	ImGui::Checkbox("Draw Center", &m_drawCenter);
	ImGui::SliderFloat("Screen Border", &m_screeBorder, -5.0f, 450.0f);
	ImGui::Checkbox("Use Culling", &m_useCulling);
	ImGui::Checkbox("Draw Screen Border", &m_drawScreenBorder);
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Tmx::loadMap(const std::string& path) {
	tmx::Map map;
	map.load(path);

	const tmx::Vector2u& mapSize = map.getTileCount();
	m_mapHeight = static_cast<float>(mapSize.y * map.getTileSize().y);
	m_tileHeight = static_cast<float>(map.getTileSize().y);
	m_tileWidth = static_cast<float>(map.getTileSize().x);

	const std::vector<std::unique_ptr<tmx::Layer>>& layers = map.getLayers();
	for (auto& layer : layers) {
		if (layer->getName() == "Terrain") {
			const tmx::TileLayer* tileLayer = dynamic_cast<const tmx::TileLayer*>(layer.get());

			if (!tileLayer)
				continue;
			m_layers.resize(m_layers.size() + 1);
			m_layers.back() = new std::pair<int, unsigned int>*[mapSize.y];
			for (int y = 0; y < mapSize.y; ++y)
				m_layers.back()[y] = new std::pair<int, unsigned int>[mapSize.x];


			const auto& tileIDs = tileLayer->getTiles();
			for (auto y = 0u; y < mapSize.y; ++y) {
				for (auto x = 0u; x < mapSize.x; ++x) {
					auto idx = y * mapSize.x + x;
					m_layers.back()[y][x].first = tileIDs[idx].ID - 1;
					if (m_layers.back()[y][x].first != -1) {
						m_cellsBackground.push_back({static_cast<float>(x) * m_tileWidth, static_cast<float>(y) * m_tileHeight, m_layers.back()[y][x].first, static_cast<float>(x) * m_tileWidth + 0.5f * m_tileWidth, static_cast<float>(y) * m_tileHeight + 0.5f * m_tileHeight, m_tileHeight });
						m_layers.back()[y][x].second = static_cast<unsigned int>(m_cellsBackground.size() - 1);
					}
				}
			}
		}

		unsigned int cutOff = m_cellsMain.size() - 1;
		if (layer->getName() == "Objects") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {	
				if (object.getName() == "top") {
					m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y - 0.5f * object.getAABB().height + m_mapHeight, object.getAABB().height });
				}else {
					m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u), object.getPosition().x + 0.5f * object.getAABB().width, object.getPosition().y - 0.5f * object.getAABB().height, object.getAABB().height });
				}
			}
		}

		if (layer->getName() == "Monsters") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getProperties()[0].getStringValue() == "sand") {
					m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u) , object.getPosition().x + 0.5f * object.getAABB().width, (object.getPosition().y - 0.5f * object.getAABB().height) - m_mapHeight, object.getAABB().height });
				}else {
					m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, static_cast<int>(object.getTileID() - 1u) , object.getPosition().x + 0.5f * object.getAABB().width, (object.getPosition().y - 0.5f * object.getAABB().height) - 40.0f, object.getAABB().height });
				}
			}
		}

		if (layer->getName() == "Entities") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				if (object.getName() == "Player" && object.getProperties()[1].getStringValue() == "house") {
					m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, 534, object.getPosition().x + 64.0f ,  object.getPosition().y - 64.0f, 128.0f });
					m_playerIndex = m_cellsMain.size() - 1;
				}

				if (object.getName() == "Character") {
					if(object.getProperties()[3].getStringValue() == "straw")
						m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, 630, object.getPosition().x + 64.0f ,  object.getPosition().y - 64.0f, 128.0f });
					else if(object.getProperties()[3].getStringValue() == "blond")
						m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, 550, object.getPosition().x + 64.0f ,  object.getPosition().y - 64.0f, 128.0f });
					else if (object.getProperties()[3].getStringValue() == "hat_girl")
						m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, 598, object.getPosition().x + 64.0f ,  object.getPosition().y - 64.0f, 128.0f });
					else if (object.getProperties()[3].getStringValue() == "young_guy")
						m_cellsMain.push_back({ object.getPosition().x, object.getPosition().y, 678, object.getPosition().x + 64.0f ,  object.getPosition().y - 64.0f, 128.0f });
				}
			}
		}
		
		if (layer->getName() == "Water") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {
				for (float x = object.getPosition().x; x < object.getPosition().x + object.getAABB().width; x = x + 64.0f) {
					for (float y = object.getPosition().y; y < object.getPosition().y + object.getAABB().height; y = y + 64.0f) {
						m_animatedCells.push_back({ x, y, 0, 242});
					}
				}
			}
		}

		if (layer->getName() == "Coast") {
			const tmx::ObjectGroup* objectLayer = dynamic_cast<const tmx::ObjectGroup*>(layer.get());
			for (auto& object : objectLayer->getObjects()) {

				for (auto& property : object.getProperties()) {

					if (property.getName() == "terrain" && property.getStringValue() == "grass") {
						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 246});
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 250});
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 254});
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 258});
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 266});
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 270});
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 274});
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 278});
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "grass_i") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 282});
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 286});
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 290});
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 294});
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 301});
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 306});
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 310});
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 314});
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "sand_i") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 318});
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 322});
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 326});
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 330});
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 338});
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 342});
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 346});
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 350});
						}
					}

					if (property.getName() == "terrain" && property.getStringValue() == "sand") {

						if (object.getProperties()[0].getStringValue() == "topleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 354});
						}

						if (object.getProperties()[0].getStringValue() == "left") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 358});
						}

						if (object.getProperties()[0].getStringValue() == "bottomleft") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 362});
						}

						if (object.getProperties()[0].getStringValue() == "top") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 366});
						}

						if (object.getProperties()[0].getStringValue() == "bottom") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 374});
						}

						if (object.getProperties()[0].getStringValue() == "topright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 378});
						}

						if (object.getProperties()[0].getStringValue() == "right") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 382});
						}

						if (object.getProperties()[0].getStringValue() == "bottomright") {
							m_animatedCells.push_back({ object.getPosition().x, object.getPosition().y, 0, 386});
						}
					}
				}				
			}
		}
	}
}

int Tmx::posYToRow(float y, float cellHeight, int min, int max, int shift) {
	return Math::Clamp(static_cast<int>(std::roundf(y / cellHeight)) + shift, min, max);
}

int Tmx::posXToCol(float x, float cellWidth, int min, int max, int shift) {
	return Math::Clamp(static_cast<int>(std::roundf(x / cellWidth)) + shift, min, max);
}

bool Tmx::isRectOnScreen(float posX, float posY, float width, float height) {
	if (posX + width < m_cullingVertices[0][0] || posX > m_cullingVertices[2][0] || m_mapHeight - posY < m_cullingVertices[0][1] || m_mapHeight - (posY + height) > m_cullingVertices[2][1]) {
		return false;
	}
	return true;
}

void Tmx::updatePoints() {
	for (auto& cell : m_cellsMain) {
		pointBatchPtr->position = { cell.centerX - m_camera.getPositionX(), m_mapHeight - cell.centerY - m_camera.getPositionY(), 0.0f};
		pointBatchPtr++;
		m_pointCount++;
	}
}

void Tmx::culling() {
	const Vector3f& position = m_camera.getPosition();

	m_cullingVertices[0] = Vector2f(m_left + m_screeBorder + position[0],  m_bottom + m_screeBorder + position[1]);
	m_cullingVertices[1] = Vector2f(m_left + m_screeBorder + position[0],  m_top    - m_screeBorder + position[1]);
	m_cullingVertices[2] = Vector2f(m_right - m_screeBorder + position[0], m_top    - m_screeBorder + position[1]);
	m_cullingVertices[3] = Vector2f(m_right - m_screeBorder + position[0], m_bottom + m_screeBorder + position[1]);

	int colMin = m_useCulling ? posXToCol(m_cullingVertices[0][0], m_tileWidth, 0, 86, -1)       :  0;
	int colMax = m_useCulling ? posXToCol(m_cullingVertices[2][0], m_tileWidth, 0, 86, 1)        : 86;
	int rowMin = m_useCulling ? 86 - posYToRow(m_cullingVertices[2][1], m_tileHeight, 0, 86, 1)  :  0;
	int rowMax = m_useCulling ? 86 - posYToRow(m_cullingVertices[0][1], m_tileHeight, 0, 86, -1) : 86;
		
	m_visibleCellsBG.clear();

	for (int j = 0; j < m_layers.size(); j++) {
		for (int y = rowMin; y < rowMax; y++) {
			for (int x = colMin; x < colMax; x++) {
				if (m_layers[j][y][x].first != -1) {
					m_visibleCellsBG.push_back(m_cellsBackground[m_layers[j][y][x].second]);
				}

			}
		}
	}

	m_visibleCellsAni.clear();
	for (AnimatedCell& animatedCell : m_animatedCells) {
		if (isRectOnScreen(animatedCell.posX, animatedCell.posY, 64.0f, 64.0f) || !m_useCulling) {
			m_visibleCellsAni.push_back(animatedCell);			
		}
	}

	m_visibleCellsMain.clear();
	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("world").getTextureRects();
	for (const Cell& cell : m_cellsMain) {
		const TextureRect& rect = rects[cell.currentFrame];
		if (isRectOnScreen(cell.posX, cell.posY - rect.height, rect.width,  rect.height) || !m_useCulling) {
			m_visibleCellsMain.push_back(cell);
		}		
	}
	
}

void Tmx::drawCullingRect() {
	const Vector3f& position = m_camera.getPosition();

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixf(&m_camera.getOrthographicMatrix()[0][0]);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glTranslatef(-position[0], -position[1], 0.0f);

	glBegin(GL_QUADS);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(m_cullingVertices[0][0], m_cullingVertices[0][1], 0.0f);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(m_cullingVertices[1][0], m_cullingVertices[1][1], 0.0f);
	glVertex3f(m_cullingVertices[2][0], m_cullingVertices[2][1], 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(m_cullingVertices[3][0], m_cullingVertices[3][1], 0.0f);

	glEnd();
	glPopMatrix();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}