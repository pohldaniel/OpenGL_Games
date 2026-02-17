#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include "Specularity.h"
#include "Application.h"
#include "Globals.h"

Specularity::Specularity(StateMachine& machine) : State(machine, States::SPECULARITY) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniformLigthBuffer.createBuffer(sizeof(LightingUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpContext.addSampler(wgpCreateSampler());
	m_texture = wgpCreateTexture(512, 512, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);
	m_textureView = wgpCreateTextureView(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, WGPUTextureAspect::WGPUTextureAspect_All, m_texture);

	wgpContext.addSahderModule("BOAT", "res/shader/specularity.wgsl");
	wgpContext.createRenderPipeline("BOAT", "RP_PTNC", VL_PTNC, std::bind(&Specularity::OnBindGroupLayout, this));

	m_boat.loadModel("res/models/fourareen.obj", false, false, false, false, false, true);
	m_boat.generateColors();
	m_wgpBoat.create(m_boat, m_textureView, m_uniformBuffer);
	m_wgpBoat.setBindGroup(std::bind(&Specularity::OnBindGroup, this, std::placeholders::_1));

	wgpContext.OnDraw = std::bind(&Specularity::OnDraw, this, std::placeholders::_1);

	float cx = cos(m_cameraState.angles[0]);
	float sx = sin(m_cameraState.angles[0]);
	float cy = cos(m_cameraState.angles[1]);
	float sy = sin(m_cameraState.angles[1]);
	Vector3f position = Vector3f(cx * cy, sx * cy, sy) * std::expf(-m_cameraState.zoom);

	m_uniforms.modelMatrix = Matrix4f::IDENTITY;
	m_uniforms.viewMatrix = Matrix4f::LookAt(position, Vector3f(0.0f), Vector3f(0.0f, 0.0f, 1.0f));
	m_uniforms.projectionMatrix = Matrix4f::Perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.01f, 100.0f);
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	m_lightingUniforms.directions[0] = { 0.5f, -0.9f, 0.1f, 0.0f };
	m_lightingUniforms.directions[1] = { 0.2f, 0.4f, 0.3f, 0.0f };
	m_lightingUniforms.colors[0] = { 1.0f, 0.9f, 0.6f, 1.0f };
	m_lightingUniforms.colors[1] = { 0.6f, 0.9f, 1.0f, 1.0f };
	updateLightingUniforms();
}

Specularity::~Specularity() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
	m_uniformLigthBuffer.markForDelete();
	wgpuTextureDestroy(m_texture);
	wgpuTextureRelease(m_texture);
	m_texture = NULL;

	wgpuTextureViewRelease(m_textureView);
	m_textureView = NULL;
}

void Specularity::fixedUpdate() {

}

void Specularity::update() {
	updateDragInertia();
	updateLightingUniforms();
}

void Specularity::render() {
	wgpDraw();
}

void Specularity::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projectionMatrix), &m_uniforms.projectionMatrix, sizeof(Uniforms::projectionMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, modelMatrix), &m_uniforms.modelMatrix, sizeof(Uniforms::modelMatrix));

	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PTNC"));

	m_wgpBoat.drawRaw(renderPassEncoder);

	if (m_drawUi)
		renderUi(renderPassEncoder);
}

void Specularity::OnMouseMotion(const Event::MouseMoveEvent& event) {
	if (m_drag.active) {
		Vector2f currentMouse = Vector2f(-static_cast<float>(event.x), static_cast<float>(event.y));
		Vector2f delta = (currentMouse - m_drag.startMouse) * m_drag.sensitivity;
		m_cameraState.angles = m_drag.startCameraState.angles + delta;
		m_cameraState.angles[1] = Math::Clamp(m_cameraState.angles[1], -HALF_PI + 1e-5f, HALF_PI - 1e-5f);
		updateViewMatrix();
		m_drag.velocity = delta - m_drag.previousDelta;
		m_drag.previousDelta = delta;
	}
}

void Specularity::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}else if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_drag.active = true;
		m_drag.startMouse = Vector2f(-static_cast<float>(event.x), static_cast<float>(event.y));
		m_drag.startCameraState = m_cameraState;
	}
}

void Specularity::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT){
		Mouse::instance().detach();
	}else if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_drag.active = false;
	}
}

void Specularity::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void Specularity::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Specularity::OnKeyUp(const Event::KeyboardEvent& event) {

}

void Specularity::resize(int deltaW, int deltaH) {
	
}

namespace ImGui {
	bool DragDirection(const char* label, Vector4f& direction) {
		Vector2f angles = Vector3f::Polar(Vector3f(direction)) * _180_ON_PI;
		bool changed = ImGui::DragFloat2(label, &angles[0]);
		direction = Vector4f(Vector3f::Euclidean(angles * PI_ON_180), direction[3]);
		return changed;
	}
}

void Specularity::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
		ImGui::DockBuilderDockWindow("Lighting", dock_id_left);
	}

	// Build our UI
	{
		bool changed = false;
		ImGui::Begin("Lighting", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		changed = ImGui::ColorEdit3("Color #0", &m_lightingUniforms.colors[0][0]) || changed;
		changed = ImGui::DragDirection("Direction #0", m_lightingUniforms.directions[0]) || changed;
		changed = ImGui::ColorEdit3("Color #1", &m_lightingUniforms.colors[1][0]) || changed;
		changed = ImGui::DragDirection("Direction #1", m_lightingUniforms.directions[1]) || changed;
		changed = ImGui::SliderFloat("Hardness", &m_lightingUniforms.hardness, 1.0f, 100.0f) || changed;
		changed = ImGui::SliderFloat("K Diffuse", &m_lightingUniforms.kd, 0.0f, 1.0f) || changed;
		changed = ImGui::SliderFloat("K Specular", &m_lightingUniforms.ks, 0.0f, 1.0f) || changed;
		ImGui::End();
		m_updateLight = changed;
	}

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}
WGPUBindGroupLayout Specularity::OnBindGroupLayout() {
	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(4);

	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries[0];
	uniformLayout.binding = 0;
	uniformLayout.visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	uniformLayout.buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries[1];
	textureBindingLayout.binding = 1;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries[2];
	samplerBindingLayout.binding = 2;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutEntry& lightUniformLayout = bindingLayoutEntries[3];
	lightUniformLayout.binding = 3;
	lightUniformLayout.visibility = WGPUShaderStage_Fragment;
	lightUniformLayout.buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	lightUniformLayout.buffer.minBindingSize = sizeof(LightingUniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	return wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
}

WGPUBindGroup Specularity::OnBindGroup(const WGPUTextureView textureView) {
	std::vector<WGPUBindGroupEntry> bindings(4);

	bindings[0].binding = 0;
	bindings[0].buffer = m_uniformBuffer.getBuffer();
	bindings[0].offset = 0;
	bindings[0].size = sizeof(Uniforms);

	bindings[1].binding = 1;
	bindings[1].textureView = textureView;

	bindings[2].binding = 2;
	bindings[2].sampler = wgpContext.getSampler(SS_LINEAR);

	bindings[3].binding = 3;
	bindings[3].buffer = m_uniformLigthBuffer.getBuffer();
	bindings[3].offset = 0;
	bindings[3].size = sizeof(LightingUniforms);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PTNC"), 0);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

void Specularity::updateViewMatrix() {
	float cx = cos(m_cameraState.angles[0]);
	float sx = sin(m_cameraState.angles[0]);
	float cy = cos(m_cameraState.angles[1]);
	float sy = sin(m_cameraState.angles[1]);
	Vector3f position = Vector3f(cx * cy, sx * cy, sy) * std::expf(-m_cameraState.zoom);
	m_uniforms.viewMatrix = Matrix4f::LookAt(position, Vector3f(0.0f), Vector3f(0.0f, 0.0f, 1.0f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
}

void Specularity::updateLightingUniforms() {
	if (m_updateLight) {
		wgpuQueueWriteBuffer(wgpContext.queue, m_uniformLigthBuffer.getBuffer(), 0u, &m_lightingUniforms, sizeof(LightingUniforms));
		m_updateLight = false;
	}
}

void Specularity::updateDragInertia() {
	constexpr float eps = 1e-4f;
	if (!m_drag.active) {

		if (std::abs(m_drag.velocity[0]) < eps && std::abs(m_drag.velocity[1]) < eps) {
			return;
		}
		m_cameraState.angles += m_drag.velocity;
		m_cameraState.angles[1] = Math::Clamp(m_cameraState.angles[1], -HALF_PI + 1e-5f, HALF_PI - 1e-5f);
		m_drag.velocity *= m_drag.intertia;
		updateViewMatrix();
	}
}