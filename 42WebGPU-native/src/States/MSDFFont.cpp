#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>


#include "MSDFFont.h"
#include "Application.h"
#include "Globals.h"

MSDFFont::MSDFFont(StateMachine& machine) : State(machine, States::MSDF_FONT) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	WgpFontRenderer::Get().init(2400u);
	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_characterSet.loadMsdfBmFromFile("res/fonts/YaHei_msdf_bm.json", "res/fonts/YaHei_msdf_bm.png");
	//m_characterSet.loadMsdfBmFromFile("res/fonts/upheavtt_msdf_bm.json", "res/fonts/upheavtt_msdf_bm.png");
	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	
	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	wgpContext.addSahderModule("FONT", "res/shader/font.wgsl");
	wgpContext.createRenderPipeline("FONT", "RP_FONT", VL_BATCH, std::bind(&MSDFFont::OnBindGroupLayouts, this));

	
	wgpContext.OnDraw = std::bind(&MSDFFont::OnDraw, this, std::placeholders::_1);

	m_uniforms.projectionMatrix = m_camera.getPerspectiveMatrix();
	m_uniforms.viewMatrix = m_camera.getViewMatrix();
	m_uniforms.modelMatrix = Matrix4f::IDENTITY;
	m_uniforms.normalMatrix = Matrix4f::IDENTITY;
	
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	WgpFontRenderer::Get().setBindGroups(std::bind(&MSDFFont::OnBindGroups, this));

	static const char* text
		= "\n"
		"WebGPU exposes an API for performing operations, such as rendering\n"
		"and computation, on a Graphics Processing Unit.\n"
		"\n"
		"Graphics Processing Units, or GPUs for short, have been essential\n"
		"in enabling rich rendering and computational applications in personal\n"
		"computing. WebGPU is an API that exposes the capabilities of GPU\n"
		"hardware for the Web. The API is designed from the ground up to\n"
		"efficiently map to (post-2014) native GPU APIs. WebGPU is not related\n"
		"to WebGL and does not explicitly target OpenGL ES.\n"
		"\n"
		"WebGPU sees physical GPU hardware as GPUAdapters. It provides a\n"
		"connection to an adapter via GPUDevice, which manages resources, and\n"
		"the device's GPUQueues, which execute commands. GPUDevice may have\n"
		"its own memory with high-speed access to the processing units.\n"
		"GPUBuffer and GPUTexture are the physical resources backed by GPU\n"
		"memory. GPUCommandBuffer and GPURenderBundle are containers for\n"
		"user-recorded commands. GPUShaderModule contains shader code. The\n"
		"other resources, such as GPUSampler or GPUBindGroup, configure the\n"
		"way physical resources are used by the GPU.\n"
		"\n"
		"GPUs execute commands encoded in GPUCommandBuffers by feeding data\n"
		"through a pipeline, which is a mix of fixed-function and programmable\n"
		"stages. Programmable stages execute shaders, which are special\n"
		"programs designed to run on GPU hardware. Most of the state of a\n"
		"pipeline is defined by a GPURenderPipeline or a GPUComputePipeline\n"
		"object. The state not included in these pipeline objects is set\n"
		"during encoding with commands, such as beginRenderPass() or\n"
		"setBlendConstant().";

	m_formatedText.create(text);

	wgpContext.addSahderModule("CUBE", "res/shader/cube.wgsl");
	wgpContext.createRenderPipeline("CUBE", "RP_CUBE", VL_PTN, std::bind(&MSDFFont::OnBindGroupLayoutsCube, this));

	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, true, true, false);
	m_wgpCube.create(m_cube);
	m_wgpCube.setBindGroups("BG", std::bind(&MSDFFont::OnBindGroupsCube, this));

	initTextTransforms();
}

MSDFFont::~MSDFFont() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
}

void MSDFFont::fixedUpdate() {

}

void MSDFFont::update() {
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
 
	float sec = Globals::clock.getElapsedTimeSec();
	float now = sec / 5.0f;

	Matrix4f transCube;
	transCube.translate(0.0f, 2.0f, -3.0f);

	Matrix4f rotCube;
	rotCube.rotate(Vector3f(sinf(now), cosf(now), 0.0f), 1.0f * _180_ON_PI);

	float crawl = fmodf(sec / 2.5f, 14.0f);

	Matrix4f rot;
	rot.rotate(-22.5f, 0.0f, 0.0f);

	Matrix4f trans;
	trans.translate({ 0.0f, crawl -3.0f, 0.0f });

	m_model = rot * trans;

	m_uniforms.projectionMatrix = m_camera.getPerspectiveMatrix();
	m_uniforms.viewMatrix = m_camera.getViewMatrix();
	m_uniforms.modelMatrix = transCube * rotCube;
	m_uniforms.normalMatrix = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.modelMatrix);
}

void MSDFFont::render() {
	wgpDraw();
}

void MSDFFont::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {
	WgpFontRenderer::Get().reset();


	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projectionMatrix), &m_uniforms.projectionMatrix, sizeof(Uniforms::projectionMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, modelMatrix), &m_uniforms.modelMatrix, sizeof(Uniforms::modelMatrix));

	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
	
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_CUBE"));
	m_wgpCube.draw(renderPassEncoder);
	
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_FONT"));

	Matrix4f transOrigin;

	transOrigin.translate(-m_characterSet.getWidth("Front") * 0.5f * largeScale, -m_characterSet.lineHeight * 0.5f * largeScale, 0.0f);
	WgpFontRenderer::Get().addTextTransformed(m_characterSet, "Front", (m_uniforms.modelMatrix * m_textTransforms[0] * transOrigin)[0], { 1.0f, 0.0f, 0.0f, 1.0f }, largeScale);
	transOrigin.translate(-m_characterSet.getWidth("Back") * 0.5f * largeScale, -m_characterSet.lineHeight * 0.5f * largeScale, 0.0f);
	WgpFontRenderer::Get().addTextTransformed(m_characterSet, "Back", (m_uniforms.modelMatrix * m_textTransforms[1] * transOrigin)[0], { 0.0f, 1.0f, 1.0f, 1.0f }, largeScale);
	transOrigin.translate(-m_characterSet.getWidth("Right") * 0.5f * largeScale, -m_characterSet.lineHeight * 0.5f * largeScale, 0.0f);
	WgpFontRenderer::Get().addTextTransformed(m_characterSet, "Right", (m_uniforms.modelMatrix * m_textTransforms[2] * transOrigin)[0], { 0.0f, 1.0f, 0.0f, 1.0f }, largeScale);
	transOrigin.translate(-m_characterSet.getWidth("Left") * 0.5f * largeScale, -m_characterSet.lineHeight * 0.5f * largeScale, 0.0f);
	WgpFontRenderer::Get().addTextTransformed(m_characterSet, "Left", (m_uniforms.modelMatrix * m_textTransforms[3] * transOrigin)[0], { 1.0f, 0.0f, 1.0f, 1.0f }, largeScale);
	transOrigin.translate(-m_characterSet.getWidth("Top") * 0.5f * largeScale, -m_characterSet.lineHeight * 0.5f * largeScale, 0.0f);
	WgpFontRenderer::Get().addTextTransformed(m_characterSet, "Top", (m_uniforms.modelMatrix * m_textTransforms[4] * transOrigin)[0], { 0.0f, 0.0f, 1.0f, 1.0f }, largeScale);
	transOrigin.translate(-m_characterSet.getWidth("Bottom") * 0.5f * largeScale, -m_characterSet.lineHeight * 0.5f * largeScale, 0.0f);
	WgpFontRenderer::Get().addTextTransformed(m_characterSet, "Bottom", (m_uniforms.modelMatrix * m_textTransforms[5] * transOrigin)[0], { 1.0f, 1.0f, 0.0f, 1.0f }, largeScale);

	WgpFontRenderer::Get().draw(renderPassEncoder);

	transOrigin.translate(-m_characterSet.getWidth("WebGPU") * 0.5f * largeScale, -m_characterSet.lineHeight * 0.5f * largeScale, 0.0f);

	WgpFontRenderer::Get().addTextTransformed(m_characterSet, "WebGPU", (m_model * transOrigin)[0], {1.0f, 1.0f, 1.0f, 1.0f}, largeScale);
	WgpFontRenderer::Get().draw(renderPassEncoder);

	transOrigin.translate({ -3.0f, -0.1f - (m_characterSet.lineHeight * 0.5f * largeScale), 0.0f });
	WgpFontRenderer::Get().addTextTransformed(m_characterSet, m_formatedText, (m_model * transOrigin)[0], { 1.0f, 1.0f, 1.0f, 1.0f }, smallScale);

	WgpFontRenderer::Get().draw(renderPassEncoder);

	if (m_drawUi)
		renderUi(renderPassEncoder);
}

void MSDFFont::OnMouseMotion(const Event::MouseMoveEvent& event) {

}

void MSDFFont::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void MSDFFont::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().detach();
	}
}

void MSDFFont::OnMouseWheel(const Event::MouseWheelEvent& event) {
	
}

void MSDFFont::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void MSDFFont::OnKeyUp(const Event::KeyboardEvent& event) {

}

void MSDFFont::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void MSDFFont::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> MSDFFont::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(2);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries0(2);
	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries0[0];
	uniformLayout.binding = 0u;
	uniformLayout.visibility = WGPUShaderStage_Vertex;
	uniformLayout.buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries0[1];
	samplerBindingLayout.binding = 1u;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(1);
	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries1[0];
	textureBindingLayout.binding = 0u;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> MSDFFont::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(2);

	std::vector<WGPUBindGroupEntry> bindGroupEntries0(2);
	bindGroupEntries0[0].binding = 0u;
	bindGroupEntries0[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries0[0].offset = 0u;
	bindGroupEntries0[0].size = sizeof(Uniforms);

	bindGroupEntries0[1].binding = 1u;
	bindGroupEntries0[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);
	
	WGPUBindGroupDescriptor bindGroupDesc0 = {};
	bindGroupDesc0.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_FONT"), 0u);
	bindGroupDesc0.entryCount = (uint32_t)bindGroupEntries0.size();
	bindGroupDesc0.entries = bindGroupEntries0.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc0);

	std::vector<WGPUBindGroupEntry> bindGroupEntries1(1);
	bindGroupEntries1[0].binding = 0u;
	bindGroupEntries1[0].textureView = m_characterSet.m_texture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc1 = {};
	bindGroupDesc1.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_FONT"), 1u);
	bindGroupDesc1.entryCount = (uint32_t)bindGroupEntries1.size();
	bindGroupDesc1.entries = bindGroupEntries1.data();
	bindGroups[1] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc1);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> MSDFFont::OnBindGroupLayoutsCube() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);
	WGPUBindGroupLayoutEntry& uniformLayout = bindingLayoutEntries[0];
	uniformLayout.binding = 0u;
	uniformLayout.visibility = WGPUShaderStage_Vertex;
	uniformLayout.buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	uniformLayout.buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> MSDFFont::OnBindGroupsCube() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(1);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_CUBE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

void MSDFFont::initTextTransforms() {
	m_textTransforms[0] = Matrix4f::Translate(0.0f, 0.0f, 1.1f) * Matrix4f::Rotate(0.0f, 0.0f, 0.0f);
	m_textTransforms[1] = Matrix4f::Translate(0.0f, 0.0f, -1.1f) * Matrix4f::Rotate(0.0f, 180.0f, 0.0f);
	m_textTransforms[2] = Matrix4f::Translate(1.1f, 0.0f, 0.0f) * Matrix4f::Rotate(0.0f, 90.0f, 0.0f);
	m_textTransforms[3] = Matrix4f::Translate(-1.1f, 0.0f, 0.0f) * Matrix4f::Rotate(0.0f, -90.0f, 0.0f);
	m_textTransforms[4] = Matrix4f::Translate(0.0f, 1.1f, 0.0f) * Matrix4f::Rotate(-90.0f, 0.0f, 0.0f);
	m_textTransforms[5] = Matrix4f::Translate(0.0f, -1.1f, 0.0f) * Matrix4f::Rotate(90.0f, 0.0f, 0.0f);
}