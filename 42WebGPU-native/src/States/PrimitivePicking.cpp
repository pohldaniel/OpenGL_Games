#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <engine/utils/BinaryIO.h>

#include "PrimitivePicking.h"
#include "Application.h"
#include "Globals.h"

PrimitivePicking::PrimitivePicking(StateMachine& machine) : State(machine, States::PRIMITIVE_PICKING) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 30.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(20.0f);
	m_camera.setRotationSpeed(0.1f);

	//Utils::JsonIO jsonIO;
	//jsonIO.jsonToObj("res/models/teapot.json", "res/models/teapot.obj");
	m_teapot.loadModel("res/models/teapot.obj", false, false, true);
	m_trackball.reshape(Application::Width, Application::Height);

	wgpContext.setClearColor({ 0.0f, 0.0f, 1.0f, 1.0f });

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_uniforms.view * m_uniforms.model);
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::IDENTITY;
	m_uniforms.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	wgpContext.addSahderModule("COLOR_PN", "res/shader/color_PN.wgsl");
	wgpContext.createRenderPipeline("COLOR_PN", "RP_COLOR", VL_PN, std::bind(&PrimitivePicking::OnBindGroupLayoutsColor, this));

	m_wgpTeapot.create(m_teapot);
	m_wgpTeapot.setBindGroups("BG", std::bind(&PrimitivePicking::OnBindGroupsColor, this));

	wgpContext.OnDraw = std::bind(&PrimitivePicking::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

PrimitivePicking::~PrimitivePicking() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
}

void PrimitivePicking::fixedUpdate() {

}

void PrimitivePicking::update() {
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
	m_trackball.idle();

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = m_trackball.getTransform();
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_uniforms.view * m_uniforms.model);
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::IDENTITY;
	m_uniforms.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));
}

void PrimitivePicking::render() {
	wgpDraw();
}

void PrimitivePicking::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_COLOR"));
		m_wgpTeapot.draw(renderPassEncoder);

		if (m_drawUi)
			renderUi(renderPassEncoder);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void PrimitivePicking::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void PrimitivePicking::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
	}else if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void PrimitivePicking::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
	}else if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().detach();
	}
}

void PrimitivePicking::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void PrimitivePicking::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void PrimitivePicking::OnKeyUp(const Event::KeyboardEvent& event) {

}

void PrimitivePicking::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void PrimitivePicking::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> PrimitivePicking::OnBindGroupLayoutsColor() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);
	
	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	return bindingLayouts;
}

std::vector<WGPUBindGroup> PrimitivePicking::OnBindGroupsColor() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindings(1);

	bindings[0].binding = 0;
	bindings[0].buffer = m_uniformBuffer.getBuffer();
	bindings[0].offset = 0;
	bindings[0].size = sizeof(Uniforms);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_COLOR"), 0);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
	return bindGroups;
}