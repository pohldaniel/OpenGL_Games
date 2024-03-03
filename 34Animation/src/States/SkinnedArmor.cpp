#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_opengl3.h>
#include <imgui_internal.h>
#include <engine/Batchrenderer.h>
#include <engine/DebugRenderer.h>

#include "SkinnedArmor.h"
#include "Application.h"
#include "Globals.h"


SkinnedArmor::SkinnedArmor(StateMachine& machine) : State(machine, States::CHARACTER) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	//m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, 5.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.rotate(0.0f, 22.5f);
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(5.0f);
	m_camera.setOffsetDistance(m_offsetDistance);

	glClearColor(0.494f, 0.686f, 0.796f, 1.0f);
	//glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);

	DebugRenderer::Get().setEnable(true);

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * 96);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	//StateMachine::ToggleWireframe();

	BoundingBox box;

	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;
	std::vector<std::vector<Utils::GeometryDesc>> geomDescs;
	std::vector<std::array<unsigned int, 4>> boneIds;
	std::vector<std::array<float, 4>> weights;
	std::vector<std::string> boneList;
	std::vector<ModelBone> meshBones;

	mdlConverter.mdlToBuffer("res/models/Girlbot/Sword.mdl", 100.0f, vertexBuffer, indexBuffer, weights, boneIds, geomDescs, meshBones, box);
	m_sword.fromBuffer(vertexBuffer, indexBuffer, 8);
	vertexBuffer.clear(); vertexBuffer.shrink_to_fit(); indexBuffer.clear(); indexBuffer.shrink_to_fit(); weights.clear(); weights.shrink_to_fit(); boneIds.clear(); boneIds.shrink_to_fit();
	mdlConverter.mdlToBuffer("res/models/Girlbot/Armor.mdl", 1.0f, vertexBuffer, indexBuffer, weights, boneIds, geomDescs, meshBones, box);
	m_armor.fromBuffer(vertexBuffer, indexBuffer, 8, weights, boneIds);
}

SkinnedArmor::~SkinnedArmor() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void SkinnedArmor::fixedUpdate() {
	m_character.fixedUpdate(m_fdt);
}

void SkinnedArmor::update() {

	Mouse &mouse = Mouse::instance();
	float dx = 0.0f;
	float dy = 0.0f;

	Vector3f pos = m_character.m_model.getWorldPosition();
	pos[1] += 1.5f;
	m_camera.Camera::setTarget(pos);

	if (mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT)) {
		dx = mouse.xDelta();
		dy = mouse.yDelta();
	}

	if (dx || dy) {
		m_camera.rotate(dx, dy, pos);
		m_character.m_model.rotate(0.0f, -dx * 0.1f, 0.0f);
	}

	m_character.update(m_dt);
}

void SkinnedArmor::render() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	auto shader = Globals::shaderManager.getAssetPointer("texture");
	shader->use();
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadMatrix("u_model", Matrix4f::IDENTITY);
	shader->loadInt("u_texture", 1);
	Globals::textureManager.get("floor").bind(1);
	Globals::shapeManager.get("floor").drawRaw();
	
	shader->unuse();

	m_character.draw(m_camera);

	shader->use();
	Globals::textureManager.get("sword").bind(1);
	shader->loadMatrix("u_model", m_character.m_locatorNode->getWorldTransformation());
	m_sword.drawRaw();
	shader->unuse();

	shader = Globals::shaderManager.getAssetPointer("animation_new");
	shader->use();
	shader->loadVector("u_light", Vector3f(1.0f, 1.0f, 1.0f));
	shader->loadMatrix("u_projection", m_camera.getPerspectiveMatrix());
	shader->loadMatrix("u_view", m_camera.getViewMatrix());
	shader->loadVector("u_color", Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	Globals::textureManager.get("sword").bind(0);
	m_armor.drawRaw();
	shader->unuse();

	DebugRenderer::Get().SetView(&m_camera);
	//DebugRenderer::Get().AddSkeleton(m_character.m_model.m_meshes[0]->m_bones, m_character.m_model.m_meshes[0]->m_numBones, { 0.0f, 1.0f, 0.0f, 1.0f });
	DebugRenderer::Get().drawBuffer();

	if (m_drawUi)
		renderUi();
}

void SkinnedArmor::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void SkinnedArmor::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}

	if (event.button == 1u) {
		Mouse::instance().attach(Application::GetWindow(), false, false, false);
	}
}

void SkinnedArmor::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 2u || event.button == 1u) {
		Mouse::instance().detach();
	}
}

void SkinnedArmor::OnMouseWheel(Event::MouseWheelEvent& event) {
	if (event.direction == 1u) {
		m_offsetDistance += 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}

	if (event.direction == 0u) {
		m_offsetDistance -= 2.0f;
		m_offsetDistance = std::max(0.0f, std::min(m_offsetDistance, 150.0f));
		m_camera.setOffsetDistance(m_offsetDistance);
	}
}

void SkinnedArmor::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}

	if (event.keyCode == VK_ESCAPE) {
		Mouse::instance().detach();
		m_isRunning = false;
	}
}

void SkinnedArmor::OnKeyUp(Event::KeyboardEvent& event) {

}

void SkinnedArmor::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void SkinnedArmor::renderUi() {
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

