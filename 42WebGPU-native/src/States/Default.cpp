#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include "Default.h"
#include "Application.h"
#include "Globals.h"

Default::Default(StateMachine& machine) : State(machine, States::DEFAULT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 2.0f, 10.0f), Vector3f(0.0f, 2.0f, 10.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_model.loadModel("res/models/mammoth.obj", Vector3f(1.0f, 0.0f, 0.0f), 0.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f, false, false, false, false, false, true, false);
	for (ObjMesh* mesh : m_model.getMeshes()) {
		m_vertexBuffer.push_back(WgpBuffer());
		m_vertexBuffer.back().createBuffer(reinterpret_cast<const void*>(mesh->getVertexBuffer().data()), sizeof(float) * mesh->getVertexBuffer().size(), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
		wgpuQueueWriteBuffer(wgpContext.queue, m_vertexBuffer.back().m_buffer, 0, reinterpret_cast<const void*>(mesh->getVertexBuffer().data()), sizeof(float) * mesh->getVertexBuffer().size());

		m_indexBuffer.push_back(WgpBuffer());
		m_indexBuffer.back().createBuffer(reinterpret_cast<const void*>(mesh->getIndexBuffer().data()), sizeof(unsigned int) * mesh->getIndexBuffer().size(), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Index);
		wgpuQueueWriteBuffer(wgpContext.queue, m_indexBuffer.back().m_buffer, 0, reinterpret_cast<const void*>(mesh->getIndexBuffer().data()), sizeof(unsigned int) * mesh->getIndexBuffer().size());

		//m_textures.push_back(WgpTexture());
		//m_textures.back().loadFromFile("res/textures/grid512.png", true);
		//m_textures.back().copyToDestination(wgpContext.texture);

		m_mammoth.push_back(WgpMesh(m_vertexBuffer.back(), m_indexBuffer.back(), m_textures.back(), mesh->getIndexBuffer().size()));
	}
	wgpContext.OnDraw = std::bind(&Default::OnDraw, this, std::placeholders::_1);
}

Default::~Default() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void Default::fixedUpdate() {

}

void Default::update() {
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

void Default::render() {
	wgpDraw();
}

void Default::OnDraw(const WGPURenderPassEncoder& renderPass) {
	wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0, m_vertexBuffer.back().m_buffer, 0, wgpuBufferGetSize(m_vertexBuffer.back().m_buffer));
	wgpuRenderPassEncoderSetIndexBuffer(renderPass, m_indexBuffer.back().m_buffer, WGPUIndexFormat_Uint32, 0, wgpuBufferGetSize(m_indexBuffer.back().m_buffer));
	wgpuRenderPassEncoderDrawIndexed(renderPass, m_mammoth.back().drawCount, 1, 0, 0, 0);
	
	if (m_drawUi)
		renderUi(renderPass);
}

void Default::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void Default::OnMouseButtonDown(Event::MouseButtonEvent& event) {

}

void Default::OnMouseButtonUp(Event::MouseButtonEvent& event) {

}

void Default::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Default::OnKeyDown(Event::KeyboardEvent& event) {
	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Default::OnKeyUp(Event::KeyboardEvent& event) {

}

void Default::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Default::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
	ImGui_ImplWGPU_NewFrame();
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
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}