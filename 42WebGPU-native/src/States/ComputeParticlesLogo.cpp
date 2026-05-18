#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <engine/utils/BinaryIO.h>

#include "ComputeParticlesLogo.h"
#include "Application.h"
#include "Globals.h"

ComputeParticlesLogo::ComputeParticlesLogo(StateMachine& machine) : State(machine, States::COMPUTE_PARTICLES_LOGO) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpVertexAttribute(VL_0).push_back({NULL, WGPUVertexFormat_Float32x3, 0u, 0u});
	wgpVertexAttribute(VL_0).push_back({NULL, WGPUVertexFormat_Float32x4, 3 * sizeof(float), 1u});
	wgpVertexAttribute(VL_1).push_back({NULL, WGPUVertexFormat_Float32x2, 0u, 2u});

	wgpVertexBufferLayout(VL_0).push_back({NULL, WGPUVertexStepMode_Vertex, 48u, wgpVertexAttribute(VL_0).size(), wgpVertexAttribute(VL_0).data()});
	wgpVertexBufferLayout(VL_0).push_back({NULL, WGPUVertexStepMode_Vertex, 8u , wgpVertexAttribute(VL_1).size(), wgpVertexAttribute(VL_1).data()});

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, -50.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(50.0f);

	wgpContext.setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
	
	wgpContext.addSahderModule("PARTICLE", "res/shader/particle.wgsl");
	wgpContext.createRenderPipeline("PARTICLE", "RP_PARTICLE", VL_0, std::bind(&ComputeParticlesLogo::OnBindGroupLayoutsParticle, this));

	wgpContext.addSahderModule("PROBABILITY", "res/shader/particle_probability.wgsl");
	wgpContext.createComputePipeline("PROBABILITY", "import_level", "CP_IMPORT", std::bind(&ComputeParticlesLogo::OnBindGroupLayoutsProbability, this));
	wgpContext.createComputePipeline("PROBABILITY", "export_level", "CP_EXPORT", std::bind(&ComputeParticlesLogo::OnBindGroupLayoutsProbability, this));

	wgpContext.addSahderModule("SIMULATE", "res/shader/particle_simulate.wgsl");
	wgpContext.createComputePipeline("SIMULATE", "simulate", "CP_SIMULATE", std::bind(&ComputeParticlesLogo::OnBindGroupLayoutsSimulate, this));

	m_wgpWgpuLogo.setTextureUsage(WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst);
	m_wgpWgpuLogo.loadFromFile("res/textures/webgpu.png");

	m_probabilityBuffer.createBuffer(16u, WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst);
	wgpuQueueWriteBuffer(wgpContext.queue, m_probabilityBuffer.getBuffer(), 0, &m_wgpWgpuLogo.width(), 16u);

	m_bufferA.createBuffer(m_wgpWgpuLogo.getWidth() * m_wgpWgpuLogo.getHeight() * 4u, WGPUBufferUsage_Storage);
	m_bufferB.createBuffer(m_wgpWgpuLogo.getWidth() * m_wgpWgpuLogo.getHeight() * 4u, WGPUBufferUsage_Storage);

	WgpRenderer::Dispatch(m_wgpWgpuLogo, m_probabilityBuffer, m_bufferA, m_bufferB);

	m_simulationBuffer.createBuffer(sizeof(ParticleData), WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst);
	m_particlesBuffer.createBuffer(PARTICLE_NUM * 48u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex | WGPUBufferUsage_Storage);
	m_renderParamsBuffer.createBuffer(sizeof(RenderParams), WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
	
	static const float vertex_data[6 * 2] = {
		-1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f,
		-1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,
	};
	m_quadVerticesBuffer.createBuffer(vertex_data, sizeof(vertex_data), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Vertex);
	
	m_computeBindGroup = createComputeBindGroup();
	m_bindGroup = createBindGroup();

	wgpContext.OnDraw = std::bind(&ComputeParticlesLogo::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

ComputeParticlesLogo::~ComputeParticlesLogo() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void ComputeParticlesLogo::fixedUpdate() {

}

void ComputeParticlesLogo::update() {
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
	
}

void ComputeParticlesLogo::render() {
	wgpDraw();
}

void ComputeParticlesLogo::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	
	WGPUComputePassEncoder computePassEncoder = wgpuCommandEncoderBeginComputePass(commandEncoder, NULL);
	wgpuComputePassEncoderSetPipeline(computePassEncoder, wgpContext.computePipelines.at("CP_SIMULATE"));

	wgpuComputePassEncoderSetBindGroup(computePassEncoder, 0u, m_computeBindGroup, 0u, NULL);
	wgpuComputePassEncoderDispatchWorkgroups(computePassEncoder, ceil(PARTICLE_NUM / 64.f), 1u, 1u);
	wgpuComputePassEncoderEnd(computePassEncoder);
	wgpuComputePassEncoderRelease(computePassEncoder);
	
	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PARTICLE"));
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroup, 0u, 0u);
	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0u, m_particlesBuffer.getBuffer(), 0, wgpuBufferGetSize(m_particlesBuffer.getBuffer()));
	wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 1u, m_quadVerticesBuffer.getBuffer(), 0u, wgpuBufferGetSize(m_quadVerticesBuffer.getBuffer()));
	wgpuRenderPassEncoderDraw(renderPassEncoder, 6, PARTICLE_NUM, 0, 0);

	if (m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
}

void ComputeParticlesLogo::OnMouseMotion(const Event::MouseMoveEvent& event) {

}

void ComputeParticlesLogo::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void ComputeParticlesLogo::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().detach();
	}
}

void ComputeParticlesLogo::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void ComputeParticlesLogo::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void ComputeParticlesLogo::OnKeyUp(const Event::KeyboardEvent& event) {

}

void ComputeParticlesLogo::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void ComputeParticlesLogo::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> ComputeParticlesLogo::OnBindGroupLayoutsProbability() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 16u;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 512u * 512u * 4u;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Storage;
	bindingLayoutEntries[2].buffer.minBindingSize = 512u * 512u * 4u;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[4].storageTexture.access = WGPUStorageTextureAccess::WGPUStorageTextureAccess_WriteOnly;
	bindingLayoutEntries[4].storageTexture.format = WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm;
	bindingLayoutEntries[4].storageTexture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	
	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> ComputeParticlesLogo::OnBindGroupLayoutsSimulate() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(ParticleData);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Storage;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> ComputeParticlesLogo::OnBindGroupLayoutsParticle() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(RenderParams);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

WGPUBindGroup ComputeParticlesLogo::createComputeBindGroup() {
	std::vector<WGPUBindGroupEntry> entries(3);

	entries[0].binding = 0u;
	entries[0].buffer = m_simulationBuffer.getBuffer();
	entries[0].offset = 0u;
	entries[0].size = wgpuBufferGetSize(m_simulationBuffer.getBuffer());

	entries[1].binding = 1u;
	entries[1].buffer = m_particlesBuffer.getBuffer();
	entries[1].offset = 0u;
	entries[1].size = wgpuBufferGetSize(m_particlesBuffer.getBuffer());

	entries[2].binding = 2u;
	entries[2].textureView = m_wgpWgpuLogo.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuComputePipelineGetBindGroupLayout(wgpContext.computePipelines.at("CP_SIMULATE"), 0);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup ComputeParticlesLogo::createBindGroup() {
	std::vector<WGPUBindGroupEntry> entries(1);

	entries[0].binding = 0u;
	entries[0].buffer = m_renderParamsBuffer.getBuffer();
	entries[0].offset = 0u;
	entries[0].size = wgpuBufferGetSize(m_renderParamsBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PARTICLE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}