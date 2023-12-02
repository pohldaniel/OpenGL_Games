#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include "Bridge.h"
#include "Application.h"
#include "Globals.h"
#include "Menu.h"
#include "TileSet.h"

Bridge::Bridge(StateMachine& machine) : State(machine, States::BRIDGE), unitRedCreature(Vector2f(64.0f, static_cast<float>(Application::Height) - 344.0f)) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera = Camera();
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	if (!TileSetManager::Get().containsTileset("bridge")) {
		TileSetManager::Get().getTileSet("bridge").loadTileSetCpu({
			"res/bridge/Beam Road.bmp",
			"res/bridge/Beam Wood.bmp",
			"res/bridge/Pin.bmp",
			"res/bridge/Red Creature.bmp" });

		TileSetManager::Get().getTileSet("bridge").loadTileSetGpu();
		Beam::Init(std::vector<TextureRect>(TileSetManager::Get().getTileSet("bridge").getTextureRects().begin(), TileSetManager::Get().getTileSet("bridge").getTextureRects().begin() + 2));
		Pin::Init(TileSetManager::Get().getTileSet("bridge").getTextureRects()[2]);
		UnitBridge::Init(TileSetManager::Get().getTileSet("bridge").getTextureRects()[3]);
	}

	m_sprites = TileSetManager::Get().getTileSet("bridge").getAtlas();
	Spritesheet::Bind(m_sprites);
	
	//Setup a default bridge.
	setupBridge1();
}

Bridge::~Bridge() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Bridge::fixedUpdate() {

}

void Bridge::update() {

	Keyboard &keyboard = Keyboard::instance();
	if (gameModeCurrent == GameMode::PLAYING || gameModeCurrent == GameMode::PAUSED) {
		if (keyboard.keyPressed(Keyboard::KEY_1)) {
			setupBridge1();
		}else if (keyboard.keyPressed(Keyboard::KEY_2)) {
			setupBridge2();
		}else if (keyboard.keyPressed(Keyboard::KEY_R)) {
			gameModeCurrent = (gameModeCurrent == GameMode::PLAYING ? GameMode::PAUSED : GameMode::PLAYING);
		}else if (keyboard.keyPressed(Keyboard::KEY_H)) {
			overlayInstructionsVisible = !overlayInstructionsVisible;
		}

	}
	if (gameModeCurrent == GameMode::PLAYING) {
		//Update the red creature and check if it's stuck or has reached the end of the level.
		unitRedCreature.update(m_dt, listBeams);
		if (unitRedCreature.getIsStuck())
			gameModeCurrent = GameMode::DEFEAT;
		else if (unitRedCreature.getReachedEndOfLevel())
			gameModeCurrent = GameMode::VICTORY;


		//Calculate the forces at the pins (and in the beams).
		for (auto& pinSelected : listPins)
			if (pinSelected != nullptr)
				pinSelected->calculateForces(listBeams, unitRedCreature);

		//Update the beams.
		for (int count = 0; count < listBeams.size(); count++) {
			auto& beamSelected = listBeams[count];
			if (beamSelected != nullptr && beamSelected->getIsBroken()) {
				listBeams.erase(listBeams.begin() + count);
				count--;
			}
		}

		//Update the pins.
		for (auto pinSelected : listPins)
			if (pinSelected != nullptr)
				pinSelected->update(m_dt);
	}
}


void Bridge::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Globals::textureManager.get("bridge_background").bind(0);
	auto shader = Globals::shaderManager.getAssetPointer("quad_back");
	shader->use();

	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	Globals::shapeManager.get("quad").drawRaw();
	shader->unuse();

	//Draw the red creature
	unitRedCreature.drawBatched();

	//Draw the beams.
	for (auto& beamSelected : listBeams)
		if (beamSelected != nullptr)
			beamSelected->drawBatched();

	//Draw the pins.
	for (auto& pinSelected : listPins)
		if (pinSelected != nullptr)
			pinSelected->drawBatched();
	
	Batchrenderer::Get().drawBuffer();

	switch (gameModeCurrent) {
		case GameMode::PLAYING: case GameMode::PAUSED:
			//Draw the instructions overlay.
			if (overlayInstructionsVisible) {
				Globals::textureManager.get("bridge_overlay").bind(0);
				auto shader = Globals::shaderManager.getAssetPointer("quad");
				shader->use();
				shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * Matrix4f::Translate(318.0f * 0.5f + 40.0f, Application::Height - (40.0f + 203.0f * 0.5f), 0.0f) * Matrix4f::Scale(318.0f * 0.5f, 203.0f * 0.5f, 0.0f));
				shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
				Globals::shapeManager.get("quad").drawRaw();
				shader->unuse();
			}
			break;


		case GameMode::VICTORY:
			Globals::textureManager.get("bridge_victory").bind(0);
			shader = Globals::shaderManager.getAssetPointer("quad");
			shader->use();
			shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
			shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
			Globals::shapeManager.get("quad").drawRaw();
			shader->unuse();
			break;
		case GameMode::DEFEAT:
			Globals::textureManager.get("bridge_defeat").bind(0);
			shader = Globals::shaderManager.getAssetPointer("quad");
			shader->use();
			shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
			shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
			Globals::shapeManager.get("quad").drawRaw();
			shader->unuse();
			break;
	}
}

void Bridge::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Bridge::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
}

void Bridge::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	
}

void Bridge::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Bridge::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Bridge::OnKeyUp(Event::KeyboardEvent& event) {

}

void Bridge::resize(int deltaW, int deltaH) {
}

void Bridge::renderUi() {
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

void Bridge::setupBridge1() {
	unitRedCreature.reset();

	listPins.clear();
	listBeams.clear();

	listPins.push_back(std::make_shared<Pin>(Vector2f(264.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f), true));
	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(696.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f), true));

	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 293.5f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 293.5f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 293.5f - 28.0f)));

	listBeams.push_back(std::make_shared<Beam>(listPins[0], listPins[1], true));
	listBeams.push_back(std::make_shared<Beam>(listPins[1], listPins[2], true));
	listBeams.push_back(std::make_shared<Beam>(listPins[2], listPins[3], true));
	listBeams.push_back(std::make_shared<Beam>(listPins[3], listPins[4], true));

	listBeams.push_back(std::make_shared<Beam>(listPins[5], listPins[0]));
	listBeams.push_back(std::make_shared<Beam>(listPins[5], listPins[1]));
	listBeams.push_back(std::make_shared<Beam>(listPins[5], listPins[6]));
	listBeams.push_back(std::make_shared<Beam>(listPins[6], listPins[2]));
	listBeams.push_back(std::make_shared<Beam>(listPins[7], listPins[3]));
	listBeams.push_back(std::make_shared<Beam>(listPins[7], listPins[4]));
	listBeams.push_back(std::make_shared<Beam>(listPins[7], listPins[6]));
}


void Bridge::setupBridge2() {
	unitRedCreature.reset();

	listPins.clear();
	listBeams.clear();

	listPins.push_back(std::make_shared<Pin>(Vector2f(264.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f), true));
	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(696.0f, static_cast<float>(Application::Height) - 387.0f - 28.0f), true));

	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 293.5f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 293.5f - 28.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 293.5f - 28.0f)));

	listBeams.push_back(std::make_shared<Beam>(listPins[0], listPins[1], true));
	listBeams.push_back(std::make_shared<Beam>(listPins[1], listPins[2], true));
	listBeams.push_back(std::make_shared<Beam>(listPins[2], listPins[3], true));
	listBeams.push_back(std::make_shared<Beam>(listPins[3], listPins[4], true));

	listBeams.push_back(std::make_shared<Beam>(listPins[5], listPins[0]));
	listBeams.push_back(std::make_shared<Beam>(listPins[5], listPins[1]));
	listBeams.push_back(std::make_shared<Beam>(listPins[5], listPins[6]));
	listBeams.push_back(std::make_shared<Beam>(listPins[6], listPins[1]));
	listBeams.push_back(std::make_shared<Beam>(listPins[6], listPins[2]));
	listBeams.push_back(std::make_shared<Beam>(listPins[6], listPins[3]));
	listBeams.push_back(std::make_shared<Beam>(listPins[7], listPins[3]));
	listBeams.push_back(std::make_shared<Beam>(listPins[7], listPins[4]));
	listBeams.push_back(std::make_shared<Beam>(listPins[7], listPins[6]));
}