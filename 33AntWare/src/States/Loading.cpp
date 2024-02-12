#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>

#include<States/Game.h>

#include "Loading.h"
#include "Globals.h"
#include "SceneManager.h"

Loading::Loading(StateMachine& machine) : State(machine, States::LOADING) {
	Globals::musicManager.get("background").play(SceneManager::Get().getCurrentMusic());
	m_camera.orthographic(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);

	m_background.loadFromFile("res/textures/loading.png", true);
	m_background.markForDelete();

	m_sop.setPosition(0.0f, 0.0f, 0.0f);
	m_sop.setScale(16.0f, 16.0f, 1.0f);

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClearDepth(1.0f);

	LoadingManagerSplitted::Get().addTask(new Loading::LoadSceneTask(this, &Loading::OnProcess, &Loading::OnComplete));
	LoadingManagerSplitted::Get().startBackgroundThread();

	m_background.bind();
}

Loading::~Loading() {
	m_background.unbind();
}

void Loading::fixedUpdate() {

}

void Loading::update() {
	LoadingManagerSplitted::Get().update();

	if(LoadingManagerSplitted::Get().isFinished()){	
		m_isRunning = false;
		m_machine.addStateAtBottom(new Game(m_machine));
	}
}

void Loading::render() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("quad");
	shader->use();
	shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_sop.getTransformationSOP());

	
	Globals::shapeManager.get("quad_half").drawRaw();
	shader->unuse();
}

void Loading::resize(int deltaW, int deltaH) {
	
}

void Loading::renderUi() {
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

void Loading::OnComplete() {
	SceneManager::Get().getScene("scene").loadSceneGpu();
}

void Loading::OnProcess() {
	SceneManager::Get().getScene("scene").loadScene(SceneManager::Get().getCurrentSceneFile());
}