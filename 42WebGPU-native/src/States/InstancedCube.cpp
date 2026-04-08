#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include "InstancedCube.h"
#include "Application.h"
#include "Globals.h"

InstancedCube::InstancedCube(StateMachine& machine) : State(machine, States::INSTANCED_CUBE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 12.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_uniformBuffer.createBuffer(1024u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	wgpContext.addSampler(wgpCreateSampler());
	wgpContext.setClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
	wgpContext.addSahderModule("CUBE", "res/shader/instance.wgsl");
	wgpContext.createRenderPipeline("CUBE", "RP_INSTANCED", VL_PTN, std::bind(&InstancedCube::OnBindGroupLayouts, this));
	wgpContext.OnDraw = std::bind(&InstancedCube::OnDraw, this, std::placeholders::_1);

	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, true, true, false);
	m_wgpCube.create(m_cube);
	m_wgpCube.setBindGroups("BG", std::bind(&InstancedCube::OnBindGroups, this));

	initMVPMatrices();
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_mvps[0], 1024u);
}

InstancedCube::~InstancedCube() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
}

void InstancedCube::fixedUpdate() {

}

void InstancedCube::update() {
	Keyboard& keyboard = Keyboard::instance();
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

	Mouse& mouse = Mouse::instance();

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
	updateMVPMatrices();
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_mvps[0], 1024u);
}

void InstancedCube::render() {
	wgpDraw();
}

void InstancedCube::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {

	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_INSTANCED"));
	m_wgpCube.draw(renderPassEncoder, 16u);

	if (m_drawUi)
		renderUi(renderPassEncoder);
}

void InstancedCube::OnMouseMotion(const Event::MouseMoveEvent& event) {

}

void InstancedCube::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void InstancedCube::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().detach();
	}
}

void InstancedCube::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void InstancedCube::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void InstancedCube::OnKeyUp(const Event::KeyboardEvent& event) {

}

void InstancedCube::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void InstancedCube::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

	ImGui::Begin("Settings", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> InstancedCube::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);
	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries[0];
	uniformLayout.binding = 0u;
	uniformLayout.visibility = WGPUShaderStage_Vertex;
	uniformLayout.buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = 1024u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> InstancedCube::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(1);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = 1024u;

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_INSTANCED"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

void InstancedCube::initMVPMatrices() {
	const float step = 4.0f;
	uint32_t m = 0u;
	for (uint32_t x = 0; x < 4u; x++) {
		for (uint32_t y = 0; y < 4u; y++) {
			m_mvps[m].translate(step * (x - 4u / 2.0f + 0.5f), step * (y - 4u / 2.0f + 0.5f), 0.0f);
			m_mvps[m] = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix() * m_mvps[m];
			++m;
		}
	}
}

void InstancedCube::updateMVPMatrices() {
	const float sec = Globals::clock.getElapsedTimeSec();
	const float step = 4.0f;

	uint32_t m = 0u;
	for (uint32_t x = 0; x < 4u; ++x) {
		for (uint32_t y = 0; y < 4u; ++y) {
			m_mvps[m] = Matrix4f::Rotate(Vector3f(sinf(((float)x + 0.5f) * sec), cosf(((float)y + 0.5f) * sec), 0.0f), 1.0f * _180_ON_PI);
			m_mvps[m] = Matrix4f::Translate(step * (x - 4u / 2.0f + 0.5f), step * (y - 4u / 2.0f + 0.5f), 0.0f) * m_mvps[m];			
			m_mvps[m] = m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix() * m_mvps[m];
			++m;
		}
	}
}