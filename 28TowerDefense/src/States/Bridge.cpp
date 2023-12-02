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
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	glClearDepth(1.0f);

	m_background.setLayer(std::vector<BackgroundLayer>{
		{ &Globals::textureManager.get("forest_1"), 1, 1.0f },
		{ &Globals::textureManager.get("forest_2"), 1, 2.0f },
		{ &Globals::textureManager.get("forest_3"), 1, 3.0f },
		{ &Globals::textureManager.get("forest_4"), 1, 4.0f },
		{ &Globals::textureManager.get("forest_5"), 1, 5.0f }});
	m_background.setSpeed(0.005f);

	TileSetManager::Get().getTileSet("bridge").loadTileSetCpu({
		"res/bridge/Beam Road.bmp",
		"res/bridge/Beam Wood.bmp",
		"res/bridge/Pin.bmp",
		"res/bridge/Red Creature.bmp"});

	TileSetManager::Get().getTileSet("bridge").loadTileSetGpu();
	m_sprites = TileSetManager::Get().getTileSet("bridge").getAtlas();

	Beam::Init(std::vector<TextureRect>(TileSetManager::Get().getTileSet("bridge").getTextureRects().begin(), TileSetManager::Get().getTileSet("bridge").getTextureRects().begin() + 2));
	Pin::Init(TileSetManager::Get().getTileSet("bridge").getTextureRects()[2]);
	UnitBridge::Init(TileSetManager::Get().getTileSet("bridge").getTextureRects()[3]);

	Spritesheet::Bind(m_sprites);
	Globals::textureManager.get("bridge_background").bind(0);
	//Setup a default bridge.
	setupBridge2();
}

Bridge::~Bridge() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Bridge::fixedUpdate() {

}

void Bridge::update() {
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
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (move || dx != 0.0f || dy != 0.0f) {
		if (dx || dy) {
			m_camera.rotate(dx, dy);
		}

		if (move) {
			m_camera.move(directrion * m_dt);
		}
	}

	m_background.update(m_dt);

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

	//m_background.draw();

	//if (m_drawUi)
	//	renderUi();
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
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		ImGui::GetIO().WantCaptureMouse = false;
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void Bridge::OnKeyUp(Event::KeyboardEvent& event) {

}

void Bridge::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
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

	listPins.push_back(std::make_shared<Pin>(Vector2f(264.0f, static_cast<float>(Application::Height) - 387.0f), true));
	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 387.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 387.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 387.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(696.0f, static_cast<float>(Application::Height) - 387.0f), true));

	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 293.5f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 293.5f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 293.5f)));

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

	listPins.push_back(std::make_shared<Pin>(Vector2f(264.0f, static_cast<float>(Application::Height) - 387.0f), true));
	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 387.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 387.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 387.0f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(696.0f, static_cast<float>(Application::Height) - 387.0f), true));

	listPins.push_back(std::make_shared<Pin>(Vector2f(372.0f, static_cast<float>(Application::Height) - 293.5f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(480.0f, static_cast<float>(Application::Height) - 293.5f)));
	listPins.push_back(std::make_shared<Pin>(Vector2f(588.0f, static_cast<float>(Application::Height) - 293.5f)));

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