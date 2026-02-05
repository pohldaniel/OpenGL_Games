#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include "Wireframe.h"
#include "Application.h"
#include "Globals.h"

Wireframe::Wireframe(StateMachine& machine) : State(machine, States::WIREFRAME) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);	
	m_texture = wgpCreateTexture(512, 512, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);
	m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
	wgpContext.addSampler(wgpCreateSampler());

	wgpContext.addSahderModule("PTN", "res/shader/shader.wgsl");
	wgpContext.createRenderPipelinePTN("PTN", std::bind(&Wireframe::OnBindGroupLayoutPTN, this));
	wgpContext.addSahderModule("wireframe", "res/shader/wireframe.wgsl");
	wgpContext.createRenderPipelineWireframe("wireframe", std::bind(&Wireframe::OnBindGroupLayoutWireframe, this));

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(1.0f, 1.0f, 3.0f), Vector3f(0.2f, 0.2f, 1.5f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_mammoth.loadModel("res/models/mammoth.obj");
	m_wgpMammoth.create(m_mammoth, m_textureView, m_uniformBuffer);
	
	m_dragon.loadModel("res/models/dragon/dragon.obj", Vector3f(0.0f, 1.0f, 0.0f), 90.0f, Vector3f(0.0f, -1.0f, 0.0f), 0.1f, false, false, false, false, false, true);
	m_dragon.rewind();
	m_wgpDragon.create(m_dragon, m_textureView, m_uniformBuffer);

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setTrackballScale(0.5f);
	m_uniforms.projectionMatrix = Matrix4f::IDENTITY;
	m_uniforms.viewMatrix = Matrix4f::IDENTITY;
	m_uniforms.modelMatrix = Matrix4f::IDENTITY;
	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));
	wgpContext.OnDraw = std::bind(&Wireframe::OnDraw, this, std::placeholders::_1);
}

Wireframe::~Wireframe() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();

	wgpuTextureDestroy(m_texture);
	wgpuTextureRelease(m_texture);
	m_texture = NULL;

	wgpuTextureViewRelease(m_textureView);
	m_textureView = NULL;
}

void Wireframe::fixedUpdate() {

}

void Wireframe::update() {
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

	m_trackball.idle();
	applyTransformation(m_trackball);

	m_uniforms.projectionMatrix = m_camera.getPerspectiveMatrix();
	m_uniforms.viewMatrix = m_camera.getViewMatrix();
}

void Wireframe::render() {
	wgpDraw();
}

void Wireframe::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projectionMatrix), &m_uniforms.projectionMatrix, sizeof(Uniforms::projectionMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, modelMatrix), &m_uniforms.modelMatrix, sizeof(Uniforms::modelMatrix));
	
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	m_model == MAMMOTH ? m_wgpMammoth.draw(renderPassEncoder) : m_wgpDragon.draw(renderPassEncoder);

	if (m_drawUi)
		renderUi(renderPassEncoder);
}

void Wireframe::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Wireframe::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Wireframe::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Wireframe::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void Wireframe::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Wireframe::OnKeyUp(const Event::KeyboardEvent& event) {

}

void Wireframe::applyTransformation(TrackBall& arc) {
	m_uniforms.modelMatrix = arc.getTransform();
}

void Wireframe::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void Wireframe::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	if (ImGui::Checkbox("Draw Wirframe", &StateMachine::GetWireframeEnabled())  || StateMachine::IsWireframeToggled()) {
		m_wgpMammoth.setRenderPipelineSlot(StateMachine::GetWireframeEnabled() ? RP_WIREFRAME : RP_PTN);
		m_wgpDragon.setRenderPipelineSlot(StateMachine::GetWireframeEnabled() ? RP_WIREFRAME : RP_PTN);
	}

	int currentModel = m_model;
	if (ImGui::Combo("Model", &currentModel, "Mammoth\0Dragon\0\0")) {
		m_model = static_cast<Model>(currentModel);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

WGPUBindGroupLayout Wireframe::OnBindGroupLayoutPTN() {
	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	
	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries[0];
	uniformLayout.binding = 0;
	uniformLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	uniformLayout.buffer.type = WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries[1];
	textureBindingLayout.binding = 1;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType_Float;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries[2];
	samplerBindingLayout.binding = 2;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();
	
	return wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
}

WGPUBindGroupLayout Wireframe::OnBindGroupLayoutWireframe() {
	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(4);

	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries[0];
	uniformLayout.binding = 0;
	uniformLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	uniformLayout.buffer.type = WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutEntry& indiceBindingLayout = bindingLayoutEntries[1];
	indiceBindingLayout.binding = 1;
	indiceBindingLayout.visibility = WGPUShaderStage_Vertex;
	indiceBindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	indiceBindingLayout.buffer.hasDynamicOffset = false;

	WGPUBindGroupLayoutEntry& vertexBindingLayout = bindingLayoutEntries[2];
	vertexBindingLayout.binding = 2;
	vertexBindingLayout.visibility = WGPUShaderStage_Vertex;
	vertexBindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	vertexBindingLayout.buffer.hasDynamicOffset = false;

	WGPUBindGroupLayoutEntry& colorBindingLayout = bindingLayoutEntries[3];
	colorBindingLayout.binding = 3;
	colorBindingLayout.visibility = WGPUShaderStage_Vertex;
	colorBindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	colorBindingLayout.buffer.hasDynamicOffset = false;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();
	return wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
}