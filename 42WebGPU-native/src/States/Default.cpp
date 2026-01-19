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

	wgpContext.addSahderModule("PTN", "res/shader/shader.wgsl");
	wgpContext.createRenderPipelinePTN("PTN", std::bind(&Default::OnBindGroupLayoutPTN, this));
	m_bindGroupPTN = createBindGroupPTN();

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(1.0f, 1.0f, 3.0f), Vector3f(0.2f, 0.2f, 1.5f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_model.loadModel("res/models/mammoth.obj");
	for (ObjMesh* mesh : m_model.getMeshes()) {
		m_vertexBuffer.push_back(WgpBuffer());
		m_vertexBuffer.back().createBuffer(reinterpret_cast<const void*>(mesh->getVertexBuffer().data()), sizeof(float) * mesh->getVertexBuffer().size(), WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);

		m_indexBuffer.push_back(WgpBuffer());
		m_indexBuffer.back().createBuffer(reinterpret_cast<const void*>(mesh->getIndexBuffer().data()), sizeof(unsigned int) * mesh->getIndexBuffer().size(), WGPUBufferUsage_Index | WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);

		m_textures.push_back(WgpTexture());
		m_textures.back().loadFromFile(mesh->getMaterial().textures.at(TextureSlot::TEXTURE_DIFFUSE), true);
		m_textures.back().copyToDestination(m_texture);
		m_mammoth.push_back(WgpMesh(m_vertexBuffer.back(), m_indexBuffer.back(), m_textures.back(), mesh->getIndexBuffer().size()));
	}

	wgpContext.addSahderModule("wireframe", "res/shader/wireframe.wgsl");
	wgpContext.createRenderPipelineWireframe("wireframe", std::bind(&Default::OnBindGroupLayoutWireframe, this));
	m_bindGroupWireframe = createBindGroupWireframe();

	m_trackball.reshape(Application::Width, Application::Height);
	m_trackball.setTrackballScale(0.5f);
	m_uniforms.projectionMatrix = Matrix4f::IDENTITY;
	m_uniforms.viewMatrix = Matrix4f::IDENTITY;
	m_uniforms.modelMatrix = Matrix4f::IDENTITY;


	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer, 0, &m_uniforms, sizeof(Uniforms));
	wgpContext.OnDraw = std::bind(&Default::OnDraw, this, std::placeholders::_1);
}

Default::~Default() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	wgpuBindGroupRelease(m_bindGroupPTN);

	wgpuBufferDestroy(m_uniformBuffer);
	wgpuBufferRelease(m_uniformBuffer);
	m_uniformBuffer = nullptr;

	wgpuTextureDestroy(m_texture);
	wgpuTextureRelease(m_texture);
	m_texture = nullptr;

	wgpuTextureViewRelease(m_textureView);
	m_textureView = nullptr;
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

	m_trackball.idle();
	applyTransformation(m_trackball);

	m_uniforms.projectionMatrix = m_camera.getPerspectiveMatrix();
	m_uniforms.viewMatrix = m_camera.getViewMatrix();
}

void Default::render() {
	wgpDraw();
}

void Default::OnDraw(const WGPURenderPassEncoder& renderPass) {

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer, offsetof(Uniforms, projectionMatrix), &m_uniforms.projectionMatrix, sizeof(Uniforms::projectionMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer, offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer, offsetof(Uniforms, modelMatrix), &m_uniforms.modelMatrix, sizeof(Uniforms::modelMatrix));
	

	if (StateMachine::GetEnableWireframe()) {
		wgpuRenderPassEncoderSetPipeline(renderPass, wgpContext.renderPipelines.at(RP_WIREFRAME));
		wgpuRenderPassEncoderSetBindGroup(renderPass, 0u, m_bindGroupWireframe, 0u, nullptr);

		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0u, m_vertexBuffer.back().m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.back().m_buffer));
		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 1u, m_indexBuffer.back().m_buffer,0u , wgpuBufferGetSize(m_indexBuffer.back().m_buffer));
		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 2u, m_vertexBuffer.back().m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.back().m_buffer));
			
		const uint32_t num_triangles = m_model.getMeshes()[0]->getNumberOfTriangles();
		wgpuRenderPassEncoderDraw(renderPass, 6 * num_triangles, 1, 0, 0);
	}else {
		wgpuRenderPassEncoderSetPipeline(renderPass, wgpContext.renderPipelines.at(RP_PTN));
		wgpuRenderPassEncoderSetBindGroup(renderPass, 0u, m_bindGroupPTN, 0u, nullptr);

		wgpuRenderPassEncoderSetVertexBuffer(renderPass, 0u, m_vertexBuffer.back().m_buffer, 0u, wgpuBufferGetSize(m_vertexBuffer.back().m_buffer));
		wgpuRenderPassEncoderSetIndexBuffer(renderPass, m_indexBuffer.back().m_buffer, WGPUIndexFormat_Uint32, 0u, wgpuBufferGetSize(m_indexBuffer.back().m_buffer));
		wgpuRenderPassEncoderDrawIndexed(renderPass, m_mammoth.back().m_drawCount, 1u, 0u, 0u, 0u);
	}
	
	
	
	if (m_drawUi)
		renderUi(renderPass);
}

void Default::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void Default::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void Default::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	if (event.button == 1u) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == 2u) {
		Mouse::instance().detach();
	}
}

void Default::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void Default::OnKeyDown(Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Default::OnKeyUp(Event::KeyboardEvent& event) {

}

void Default::applyTransformation(TrackBall& arc) {
	m_uniforms.modelMatrix = arc.getTransform();
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

WGPUBindGroupLayout Default::OnBindGroupLayoutPTN() {
	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries.resize(3);
	WGPUBindGroupLayoutEntry& bindingLayout = bindingLayoutEntries[0];
	bindingLayout.binding = 0;
	bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayout.buffer.minBindingSize = sizeof(Uniforms);

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

WGPUBindGroup Default::createBindGroupPTN() {
	m_uniformBuffer = wgpCreateEmptyBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_texture = wgpCreateTexture(512, 512, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);
	m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);
	wgpContext.addSampler(wgpCreateSampler());

	std::vector<WGPUBindGroupEntry> bindings(3);

	bindings[0].binding = 0;
	bindings[0].buffer = m_uniformBuffer;
	bindings[0].offset = 0;
	bindings[0].size = sizeof(Uniforms);

	bindings[1].binding = 1;
	bindings[1].textureView = m_textureView;

	bindings[2].binding = 2;
	bindings[2].sampler = wgpContext.getSampler(SS_LINEAR);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at(RP_PTN), 0);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroupLayout Default::OnBindGroupLayoutWireframe() {
	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);

	WGPUBindGroupLayoutEntry& bindingLayout = bindingLayoutEntries[0];
	bindingLayout.binding = 0;
	bindingLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayout.buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutEntry& indiceBindingLayout = bindingLayoutEntries[1];
	indiceBindingLayout.binding = 1;
	indiceBindingLayout.visibility = WGPUShaderStage_Vertex;
	indiceBindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	indiceBindingLayout.buffer.hasDynamicOffset = false;
	indiceBindingLayout.buffer.minBindingSize = sizeof(unsigned int) * m_model.getMeshes()[0]->getIndexBuffer().size();

	WGPUBindGroupLayoutEntry& vertexBindingLayout = bindingLayoutEntries[2];
	vertexBindingLayout.binding = 2;
	vertexBindingLayout.visibility = WGPUShaderStage_Vertex;
	vertexBindingLayout.buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	vertexBindingLayout.buffer.hasDynamicOffset = false;
	vertexBindingLayout.buffer.minBindingSize = sizeof(float) * m_model.getMeshes()[0]->getVertexBuffer().size();

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries[3];
	textureBindingLayout.binding = 3;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType_Float;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries[4];
	samplerBindingLayout.binding = 4;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();
	return wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
}

WGPUBindGroup Default::createBindGroupWireframe() {
	std::vector<WGPUBindGroupEntry> bindings(5);

	bindings[0].binding = 0;
	bindings[0].buffer = m_uniformBuffer;
	bindings[0].offset = 0;
	bindings[0].size = sizeof(Uniforms);

	bindings[1].binding = 1;
	bindings[1].buffer = m_indexBuffer.back().m_buffer;
	bindings[1].offset = 0;
	bindings[1].size = sizeof(unsigned int) * m_model.getMeshes()[0]->getIndexBuffer().size();

	bindings[2].binding = 2;
	bindings[2].buffer = m_vertexBuffer.back().m_buffer;
	bindings[2].offset = 0;
	bindings[2].size = sizeof(float) * m_model.getMeshes()[0]->getVertexBuffer().size();

	bindings[3].binding = 3;
	bindings[3].textureView = m_textureView;

	bindings[4].binding = 4;
	bindings[4].sampler = wgpContext.getSampler(SS_LINEAR);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at(RP_WIREFRAME), 0);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}