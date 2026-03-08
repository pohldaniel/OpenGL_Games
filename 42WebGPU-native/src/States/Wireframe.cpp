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
	wgpContext.addSampler(wgpCreateSampler());

	wgpContext.addSahderModule("PTN", "res/shader/shader.wgsl");
	wgpContext.createRenderPipeline("PTN", "RP_PTNC", VL_PTNC, std::bind(&Wireframe::OnBindGroupLayoutsPTN, this));

	wgpContext.addSahderModule("WF", "res/shader/wireframe.wgsl");
	wgpContext.createRenderPipeline("WF", "RP_WF", VL_NONE, std::bind(&Wireframe::OnBindGroupLayoutsWF, this), WGPUPrimitiveTopology::WGPUPrimitiveTopology_LineList);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(1.0f, 1.0f, 3.0f), Vector3f(0.2f, 0.2f, 1.5f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_mammoth.loadModel("res/models/mammoth.obj");
	m_mammoth.generateColors(ModelColor::MC_POSITION);

	m_wgpMammoth.create(m_mammoth, m_uniformBuffer);	
	m_wgpMammoth.setBindGroups("BG_WF", std::bind(&Wireframe::OnBindGroupsWF, this));
	m_wgpMammoth.setBindGroups("BG", std::bind(&Wireframe::OnBindGroups, this));
	m_wgpMammoth.setRenderPipelineSlot("RP_PTNC");
	AddBindgroups(m_wgpMammoth);

	m_dragon.loadModel("res/models/dragon/dragon.obj", Vector3f(0.0f, 1.0f, 0.0f), 90.0f, Vector3f(0.0f, -1.0f, 0.0f), 0.1f, false, false, false, false, false, true);
	m_dragon.rewind();
	m_dragon.generateColors(ModelColor::MC_POSITION);

	m_wgpDragon.create(m_dragon, m_uniformBuffer);
	m_wgpDragon.setBindGroups("BG_WF", std::bind(&Wireframe::OnBindGroupsWF, this));
	m_wgpDragon.setBindGroups("BG", std::bind(&Wireframe::OnBindGroups, this));
	m_wgpDragon.setRenderPipelineSlot("RP_PTNC");
	AddBindgroups(m_wgpDragon);

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

	if (StateMachine::GetWireframeEnabled()) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_WF"));

		WgpModel model = m_model == MAMMOTH ? m_wgpMammoth : m_wgpDragon;

		for (std::list<WgpMesh>::const_iterator it = model.getMeshes().begin(); it != model.getMeshes().end(); ++it) {
			const WgpMesh& mesh = *it;
			std::vector<WGPUBindGroup>& bindGroups = mesh.getBindGroups();
			for (uint32_t i = 0u; i < bindGroups.size(); i++) {
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, i, bindGroups[i], 0u, NULL);
			}

			wgpuRenderPassEncoderDraw(renderPassEncoder, 2u * mesh.getDrawCount(), 1u, 0u, 0u);
		}
	}else {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PTNC"));
		m_model == MAMMOTH ? m_wgpMammoth.draw(renderPassEncoder) : m_wgpDragon.draw(renderPassEncoder);
	}

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
		m_wgpMammoth.setRenderPipelineSlot(StateMachine::GetWireframeEnabled() ? "RP_WF" : "RP_PTNC");
		m_wgpDragon.setRenderPipelineSlot(StateMachine::GetWireframeEnabled() ? "RP_WF" : "RP_PTNC");

		m_wgpMammoth.setBindGroupsSlot(StateMachine::GetWireframeEnabled() ? "BG_WF" : "BG");
		m_wgpDragon.setBindGroupsSlot(StateMachine::GetWireframeEnabled() ? "BG_WF" : "BG");
	}

	int currentModel = m_model;
	if (ImGui::Combo("Model", &currentModel, "Mammoth\0Dragon\0\0")) {
		m_model = static_cast<Model>(currentModel);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector <WGPUBindGroupLayout> Wireframe::OnBindGroupLayoutsPTN() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(2);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries0(2);
	
	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries0[0];
	uniformLayout.binding = 0u;
	uniformLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	uniformLayout.buffer.type = WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries0[1];
	samplerBindingLayout.binding = 1u;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(1);

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries1[0];
	textureBindingLayout.binding = 0u;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType_Float;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension_2D;

	
	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector <WGPUBindGroupLayout> Wireframe::OnBindGroupLayoutsWF() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(2);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries0(1);

	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries0[0];
	uniformLayout.binding = 0u;
	uniformLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	uniformLayout.buffer.type = WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();
	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(2);

	WGPUBindGroupLayoutEntry& indiceBindingLayout = bindingLayoutEntries1[0];
	indiceBindingLayout.binding = 0u;
	indiceBindingLayout.visibility = WGPUShaderStage_Vertex;
	indiceBindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	indiceBindingLayout.buffer.hasDynamicOffset = false;

	WGPUBindGroupLayoutEntry& vertexBindingLayout = bindingLayoutEntries1[1];
	vertexBindingLayout.binding = 1u;
	vertexBindingLayout.visibility = WGPUShaderStage_Vertex;
	vertexBindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	vertexBindingLayout.buffer.hasDynamicOffset = false;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();
	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> Wireframe::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);
	
	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PTNC"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroup> Wireframe::OnBindGroupsWF() {
	std::vector<WGPUBindGroup> bindGroups(1);
	
	std::vector<WGPUBindGroupEntry> BindGroupEntries(1);

	BindGroupEntries[0].binding = 0;
	BindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	BindGroupEntries[0].offset = 0;
	BindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WF"), 0u);
	bindGroupDesc.entryCount = (uint32_t)BindGroupEntries.size();
	bindGroupDesc.entries = BindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

WGPUBindGroup Wireframe::AddBindgroups(const WgpModel& model) {
	for (std::list<WgpMesh>::const_iterator it = model.getMeshes().begin(); it != model.getMeshes().end(); ++it) {
		const WgpMesh& mesh = *it;

		std::vector<WGPUBindGroupEntry> bindingGroupEntries(1);
		bindingGroupEntries[0].binding = 0u;
		bindingGroupEntries[0].textureView = mesh.getTexture().getTextureView();

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PTNC"), 1u);
		bindGroupDesc.entryCount = (uint32_t)bindingGroupEntries.size();
		bindGroupDesc.entries = bindingGroupEntries.data();

		mesh.addBindGroup("BG", wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc));

		std::vector<WGPUBindGroupEntry> bindingGroupEntriesWF(2);
		bindingGroupEntriesWF[0].binding = 0u;
		bindingGroupEntriesWF[0].buffer = mesh.getVertexBuffer().getBuffer();
		bindingGroupEntriesWF[0].offset = 0u;
		bindingGroupEntriesWF[0].size = wgpuBufferGetSize(mesh.getVertexBuffer().getBuffer());

		bindingGroupEntriesWF[1].binding = 1u;
		bindingGroupEntriesWF[1].buffer = mesh.getIndexBuffer().getBuffer();
		bindingGroupEntriesWF[1].offset = 0u;
		bindingGroupEntriesWF[1].size = wgpuBufferGetSize(mesh.getIndexBuffer().getBuffer());

		WGPUBindGroupDescriptor bindGroupDescWF = {};
		bindGroupDescWF.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_WF"), 1u);
		bindGroupDescWF.entryCount = (uint32_t)bindingGroupEntriesWF.size();
		bindGroupDescWF.entries = bindingGroupEntriesWF.data();

		mesh.addBindGroup("BG_WF", wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDescWF));
	}
}