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

#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>

#include "MonsterHunter.h"
#include "Application.h"
#include "Globals.h"

MonsterHunter::MonsterHunter(StateMachine& machine) : State(machine, States::MONSTER_HUNTER), m_zone(m_camera){

	m_viewWidth = 1280.0f;
	m_viewHeight= 720.0f;
	m_movingSpeed = 250.0f;
	m_screeBorder = 0.0f;

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(m_movingSpeed);

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
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/other/shadow.png", false, true, false);
	TileSetManager::Get().getTileSet("world").loadTileCpu("res/tmx/graphics/other/empty.png", false, true, false);

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
	
	m_mapHeight = m_zone.getMapHeight();

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	m_zone.setDebugCollision(m_debugCollision);

	m_zone.getPlayer()->setMovingSpeed(m_movingSpeed);
	m_zone.getPlayer()->setViewWidth(m_viewWidth);
	m_zone.getPlayer()->setViewHeight(m_viewHeight);
	m_zone.getPlayer()->setMapHeight(m_mapHeight);

	std::ifstream file("res/trainer.json", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << "res/trainer.json" << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);
	
	for (rapidjson::Value::ConstMemberIterator trainer = doc.MemberBegin(); trainer != doc.MemberEnd(); ++trainer) {
		if (std::string(trainer->name.GetString()) != "Nurse") {			
			m_trainers[trainer->name.GetString()].binom = "sand";
			m_trainers[trainer->name.GetString()].defeated = false;
			m_trainers[trainer->name.GetString()].lookAround = true;
			m_trainers[trainer->name.GetString()].viewDirections.push_back(ViewDirection::DOWN);
			m_trainers[trainer->name.GetString()].viewDirections.push_back(ViewDirection::LEFT);
			m_trainers[trainer->name.GetString()].monsters.push_back({"tmp", 5});
			m_trainers[trainer->name.GetString()].dialog.undefeated.push_back("Hello");
			m_trainers[trainer->name.GetString()].dialog.defeated.push_back("Hello");
			std::cout << "Name: " << trainer->name.GetString() << std::endl;
		}		
	}
	file.close();
}

MonsterHunter::~MonsterHunter() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void MonsterHunter::fixedUpdate() {

}

void MonsterHunter::update() {
	for (auto&& spriteEntity : m_zone.getSpriteEntities()) {
		spriteEntity->update(m_dt);
	}
	m_zone.update(m_dt);
}

void MonsterHunter::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Spritesheet::Bind(m_atlasWorld);
	m_zone.draw();

	if (m_drawUi)
		renderUi();
}

void MonsterHunter::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void MonsterHunter::OnMouseWheel(Event::MouseWheelEvent& event) {
	
}

void MonsterHunter::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void MonsterHunter::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void MonsterHunter::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void MonsterHunter::OnKeyUp(Event::KeyboardEvent& event) {

}

void MonsterHunter::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	m_zone.resize();

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();
}

void MonsterHunter::renderUi() {
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
	if(ImGui::Checkbox("Draw Center", &m_drawCenter)) {
		m_zone.setDrawCenter(m_drawCenter);
	}
		
	if(ImGui::Checkbox("Use Culling", &m_useCulling)) {
		m_zone.setUseCulling(m_useCulling);
	}

	if (ImGui::Checkbox("Draw Screen Border", &m_drawScreenBorder)) {
		m_zone.setDrawScreenBorder(m_drawScreenBorder);
	}

	if (ImGui::SliderFloat("Screen Border", &m_screeBorder, -5.0f, 450.0f)) {
		m_zone.setScreeBorder(m_screeBorder);
	}
	if (ImGui::Checkbox("Debug Collision", &m_debugCollision)) {
		m_zone.setDebugCollision(m_debugCollision);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}