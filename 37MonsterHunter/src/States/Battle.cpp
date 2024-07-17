#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <States/Menu.h>
#include <States/MonsterHunter.h>

#include "Battle.h"
#include "Application.h"
#include "Globals.h"

Battle::Battle(StateMachine& machine) : State(machine, States::BATTLE), m_mapHeight(0.0f), m_viewWidth(1280.0f), m_viewHeight(720.0f){

	m_viewWidth = 1280.0f;
	m_viewHeight = 720.0f;

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);
	
	glClearColor(0.6f, 0.8f, 0.92f, 1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

	m_playerMonster.push_back({ "Ivieron", 32u, false });
	m_playerMonster.push_back({ "Atrox", 15u, false });
	m_playerMonster.push_back({ "Cindrill", 23u, false });
	m_playerMonster.push_back({ "Atrox", 30u, false });
	m_playerMonster.push_back({ "Sparchu", 24u, false });
	m_playerMonster.push_back({ "Gulfin", 17u, false });
	m_playerMonster.push_back({ "Jacana", 16u, false });
	m_playerMonster.push_back({ "Plumette", 9u, false });
	m_playerMonster.push_back({ "Cleaf", 3u, false });
	m_playerMonster.push_back({ "Charmadillo", 30u, false });

	m_opponentMonster.push_back({ "Draem", 13u, false });
	m_opponentMonster.push_back({ "Finiette", 13u, false });
	m_opponentMonster.push_back({ "Pouch", 15u, false });
	m_opponentMonster.push_back({ "Finsta", 14u, false });
	m_opponentMonster.push_back({ "Cleaf", 14u, false });
	m_opponentMonster.push_back({ "Friolera", 20u, false });

	m_cells.reserve(6);

	m_cells.push_back({ 360.0f - 96.0f, m_viewHeight - 260.0f - 96.0f, 192.0f, 192.0f, 0 , 360.0f, m_viewHeight - 260.0f, true });
	m_cells.push_back({ 190.0f - 96.0f, m_viewHeight - 400.0f - 96.0f, 192.0f, 192.0f, 8 , 190.0f, m_viewHeight - 400.0f, true });
	m_cells.push_back({ 410.0f - 96.0f, m_viewHeight - 520.0f - 96.0f, 192.0f, 192.0f, 16, 410.0f, m_viewHeight - 520.0f, true });

	m_cells.push_back({ 900.0f - 96.0f,  m_viewHeight - 260.0f - 96.0f, 192.0f, 192.0f, 24, 900.0f , m_viewHeight - 260.0f, true });
	m_cells.push_back({ 1110.0f - 96.0f, m_viewHeight - 390.0f - 96.0f, 192.0f, 192.0f, 32, 1110.0f, m_viewHeight - 390.0f, true });
	m_cells.push_back({ 900.0f - 96.0f,  m_viewHeight - 550.0f - 96.0f, 192.0f, 192.0f, 40, 900.0f , m_viewHeight - 550.0f, true });

	for (auto& cell : m_cells) {
		m_monster.push_back(Monster(cell));
	}
}

Battle::~Battle() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Battle::fixedUpdate() {

}

void Battle::update() {
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

	if (keyboard.keyDown(Keyboard::KEY_Q)) {
		direction += Vector3f(0.0f, -1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyDown(Keyboard::KEY_E)) {
		direction += Vector3f(0.0f, 1.0f, 0.0f);
		move |= true;
	}

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		m_isRunning = false;
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

	for (auto& monster : m_monster) {
		monster.update(m_dt);
	}
}

void Battle::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Globals::textureManager.get("forest").bind();
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
	shader->loadVector("u_texRect", Vector4f(0.0f, 0.0f, 1.0f, 1.0f));
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::shapeManager.get("quad").drawRaw();

	int index = 0;
	Spritesheet::Bind(TileSetManager::Get().getTileSet("monster").getAtlas());
	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("monster").getTextureRects();
	for (const Cell& cell : m_cells) {
		const TextureRect& rect = rects[cell.currentFrame];
		if(index < 3)
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, rect.width, rect.height), Vector4f(rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY, -rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		else
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
		index++;
	}

	Batchrenderer::Get().drawBuffer();

	if (m_drawUi)
		renderUi();
}

void Battle::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Battle::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Battle::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Battle::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Battle::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void Battle::OnKeyUp(Event::KeyboardEvent& event) {

}

void Battle::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, m_viewWidth, 0.0f, m_viewHeight, -1.0f, 1.0f);

	auto shader = Globals::shaderManager.getAssetPointer("batch");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("font");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("dialog");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix());
	shader->unuse();
}

void Battle::renderUi() {
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

void Battle::setMapHeight(float mapHeight) {
	m_mapHeight = mapHeight;
}

void Battle::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}