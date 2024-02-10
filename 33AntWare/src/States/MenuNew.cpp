#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include <States/Loading.h>

#include "MenuNew.h"
#include "Application.h"
#include "Globals.h"

#include "SceneManager.h"
#include "TileSet.h"

MenuNew::MenuNew(StateMachine& machine) : State(machine, States::MENUNEW),
currentPosition(SceneManager::Get().m_currentPosition) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);
	
	SceneManager::Get().loadSettings("res/default_settings.json");
	m_offset = static_cast<short>(SceneManager::Get().getLevels().size());
	currentPosition = 0;
	
	TileSetManager::Get().getTileSet("menu").loadTileSetCpu(SceneManager::Get().getThumbs(), true);
	TileSetManager::Get().getTileSet("menu").loadTileSetCpu(std::vector<std::string>({
		"res/textures/Main Menu BG.png",
		"res/textures/Game Label.png",
		"res/textures/Credits.png" }), false);
	TileSetManager::Get().getTileSet("menu").loadTileSetGpu();

	m_tileSet = TileSetManager::Get().getTileSet("menu").getTextureRects();
	m_menuAtlas = TileSetManager::Get().getTileSet("menu").getAtlas();

	Spritesheet::SetFilter(m_menuAtlas, GL_LINEAR);
	Spritesheet::SetWrapMode(m_menuAtlas, GL_REPEAT);
	//Spritesheet::Safe("menu", m_menuAtlas);

	auto shader = Globals::shaderManager.getAssetPointer("hud");
	shader->use();
	shader->loadInt("u_texture", 2);
	shader->unuse();

	Spritesheet::Bind(m_menuAtlas, 2u); 

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClearDepth(1.0f);

}

MenuNew::~MenuNew() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void MenuNew::fixedUpdate() {

}

void MenuNew::update() {
	processInput();
}

void MenuNew::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("hud");
	shader->use();
	shader->loadVector("u_color", Vector4f(1.0f));
	m_sop.setPosition(0.0f, 0.0f, 0.0f);
	m_sop.setScale(12.0, 12.0f, 1.0f);
	TextureRect rect = m_tileSet[m_offset];
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Globals::shapeManager.get("quad_half").drawRaw();

	m_sop.setPosition(gameLabelPos[0], gameLabelPos[1], 0.0f);
	m_sop.setScale(gameLabelSize[0], gameLabelSize[1], 1.0f);
	rect = m_tileSet[m_offset + 1];
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Globals::shapeManager.get("quad_half").drawRaw();

	m_sop.setPosition(creditsPos[0], creditsPos[1], 0.0f);
	m_sop.setScale(creditsSize[0], creditsSize[1], 1.0f);
	rect = m_tileSet[m_offset + 2];
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Globals::shapeManager.get("quad_half").drawRaw();

	Vector2f labelPos = firstLevelPos;
	for (short i = 0; i < m_offset; ++i){
		if (i != currentPosition){
			m_sop.setPosition(labelPos[0], labelPos[1], 0.0f);
			m_sop.setScale(labelSize[0], labelSize[1], 1.0f);
			TextureRect rect = m_tileSet[i];

			shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
			shader->loadVector("u_color", dimColor);
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();

			
		}else{
			m_sop.setPosition(labelPos[0] + 0.4f, labelPos[1], 0.0f);
			m_sop.setScale(labelSize[0], labelSize[1], 1.0f);
			TextureRect rect = m_tileSet[i];

			shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
			shader->loadVector("u_color", Vector4f(1.0f));
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();
		}
		labelPos[1] += yMargain;
	}

	shader->unuse();
}

void MenuNew::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void MenuNew::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void MenuNew::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void MenuNew::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void MenuNew::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void MenuNew::OnKeyUp(Event::KeyboardEvent& event) {

}

void MenuNew::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_ENTER) && !keyboard.keyDown(Keyboard::KEY_RALT)) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Loading(m_machine));
		return;
	}

	if (keyboard.keyPressed(Keyboard::KEY_UP)) {
		currentPosition--;

		if (currentPosition < 0) {
			currentPosition = 0;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)) {
		currentPosition++;

		if (currentPosition >= m_offset) {
			currentPosition = m_offset - 1;
		}
	}
}

void MenuNew::resize(int deltaW, int deltaH) {
	//m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	//m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void MenuNew::renderUi() {
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