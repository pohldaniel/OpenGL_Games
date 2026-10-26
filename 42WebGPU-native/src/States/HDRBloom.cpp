#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <engine/input/GamePad.h>
#include <WebGPU/WgpRenderer.h>
#include <States/Menu.h>

#include "Globals.h"
#include "Application.h"
#include "HDRBloom.h"

HDRBloom::HDRBloom(StateMachine& machine) : State(machine, States::HDR_BLOOM) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f , -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, -5.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(10.0f);
	m_camera.setRotationSpeed(0.1f);

	m_downSampleBuffer.createBuffer(sizeof(DownsampleUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_upSampleBuffer.createBuffer(sizeof(UpsampleUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_compositeBuffer.createBuffer(sizeof(CompositeUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_compositeUniforms.bloomStrength = 0.2f;
	m_compositeUniforms.exposure = 1.0f;
	wgpuQueueWriteBuffer(wgpContext.queue, m_compositeBuffer.getBuffer(), 0, &m_compositeUniforms, sizeof(CompositeUniforms));

	m_storageBuffer.createBuffer(sizeof(Instance) * 20u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);
	m_lightBuffer.createBuffer(sizeof(Light) * 4u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 0.0f, 0.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(50.0f, 100.0f, -100.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	m_wgpSceneTarget.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_BGRA8Unorm, 1u, 1u);
	m_wgpSceneDepth.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_Depth24Plus, 1u, 1u);


	m_wgpSceneFilterTarget.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_BGRA8Unorm, 1u, 1u);
	m_wgpSceneFilterDepth.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_Depth24Plus, 1u, 1u);

	wgpContext.addSahderModule("BASE", "res/shader/bloom.wgsl");
	wgpContext.createRenderPipeline("BASE", "RP_BASE", VL_PTN, std::bind(&HDRBloom::OnBindGroupLayouts, this), 1u);

	wgpContext.addSahderModule("BASE_FILTER", "res/shader/bloom_filter.wgsl");
	wgpContext.createRenderPipeline("BASE_FILTER", "RP_BASE_FILTER", VL_PTN, std::bind(&HDRBloom::OnBindGroupLayouts, this), 1u);

	wgpContext.addSahderModule("DOWN", "res/shader/bloom_down.wgsl");
	wgpContext.createRenderPipeline("DOWN", "RP_DOWN", VL_NONE, std::bind(&HDRBloom::OnBindGroupLayoutsDown, this), 1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_LessEqual,
		{  FRAGMENT_STATE, ColorMode::WRITE_RGBA, DepthMode::PASS, StencilMode::DEFAULT, BlendMode::ADDITIVE_BLENDING_ONE });

	wgpContext.addSahderModule("UP", "res/shader/bloom_up.wgsl");
	wgpContext.createRenderPipeline("UP", "RP_UP", VL_NONE, std::bind(&HDRBloom::OnBindGroupLayoutsUp, this), 1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_LessEqual,
		{  BLEND_STATE | FRAGMENT_STATE, ColorMode::WRITE_RGBA, DepthMode::PASS, StencilMode::DEFAULT, BlendMode::ADDITIVE_BLENDING_ONE });

	wgpContext.addSahderModule("BLOOM_COMPOSITE", "res/shader/bloom_composite.wgsl");
	wgpContext.createRenderPipeline("BLOOM_COMPOSITE", "RP_BLOOM_COMPOSITE", VL_NONE, std::bind(&HDRBloom::OnBindGroupLayoutsComposite, this), 1u);

	m_cube.buildCube();
	m_cubeE.buildCube();
	m_cubeE.flipNormal();

	wgpTextureEmpty.createEmpty(2u, 2u, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat_BGRA8Unorm);
	wgpTextureEmpty.fillLayer();

	wgpTextureWood.setTextureFormat(WGPUTextureFormat_RGBA8UnormSrgb);
	wgpTextureWood.loadFromFile("res/textures/wood.png");
	wgpTextureContainer.setTextureFormat(WGPUTextureFormat_RGBA8UnormSrgb);
	wgpTextureContainer.loadFromFile("res/textures/container.png");

	m_wgpCube.create(m_cube);
	m_wgpCubeE.create(m_cubeE);

	//m_wgpCube.addBindGroup("BG_EMPTY", createBindGroup(wgpTextureEmpty));
	m_wgpCube.addBindGroup("BG_WOOD", createBindGroup(wgpTextureWood));
	m_wgpCube.addBindGroup("BG_CONTAINER", createBindGroup(wgpTextureContainer));
	m_wgpCube.setBindGroupsSlot("BG_WOOD");

	m_wgpCubeE.addBindGroup("BG_EMPTY", createBindGroup(wgpTextureEmpty));
	//m_wgpCubeE.addBindGroup("BG_WOOD", createBindGroup(wgpTextureWood));
	//.addBindGroup("BG_CONTAINER", createBindGroup(wgpTextureContainer));
	m_wgpCubeE.setBindGroupsSlot("BG_EMPTY");

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	wgpContext.OnDraw = std::bind(&HDRBloom::OnDraw, this, std::placeholders::_1, std::placeholders::_2);

	

	m_lightPositions.push_back(Vector3f(0.0f, 0.5f, 1.5f));
	m_lightPositions.push_back(Vector3f(-4.0f, 0.5f, -3.0f));
	m_lightPositions.push_back(Vector3f(3.0f, 0.5f, 1.0f));
	m_lightPositions.push_back(Vector3f(-.8f, 2.4f, -1.0f));

	m_lightColors.push_back(Vector4f(5.0f, 5.0f, 5.0f, 1.0f));
	m_lightColors.push_back(Vector4f(10.0f, 0.0f, 0.0f, 1.0f));
	m_lightColors.push_back(Vector4f(0.0f, 0.0f, 15.0, 1.0f));
	m_lightColors.push_back(Vector4f(0.0f, 5.0f, 0.0f, 1.0f));

	std::vector<Light> lights;
	lights.push_back({ Vector4f(0.0f, 0.5f, 1.5f, 0.0f), Vector4f(5.0f, 5.0f, 5.0f, 1.0f) });
	lights.push_back({ Vector4f(-4.0f, 0.5f, -3.0f, 0.0f), Vector4f(10.0f, 0.0f, 0.0f, 1.0f) });
	lights.push_back({ Vector4f(3.0f, 0.5f, 1.0f, 0.0f), Vector4f(0.0f, 0.0f, 15.0f, 1.0f) });
	lights.push_back({ Vector4f(-.8f, 2.4f, -1.0f, 0.0f), Vector4f(0.0f, 5.0f, 0.0f, 1.0f) });

	wgpuQueueWriteBuffer(wgpContext.queue, m_lightBuffer.getBuffer(), 0, lights.data(), sizeof(Light) * lights.size());

	m_bloomFBO.Init(wgpContext.device, Application::Width / 2, Application::Height / 2, 9u);
	CreateBloomBindGroups(wgpContext.device, 
		                  wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_UP"), 0u), 
		                  wgpContext.getSampler(SS_LINEAR_CLAMP), 
		                  m_wgpSceneFilterTarget.getTextureView());

	m_bindGroupComposite = createBindGroupComposite();
}

HDRBloom::~HDRBloom() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
}

void HDRBloom::fixedUpdate() {

}

void HDRBloom::update() {
	Keyboard& keyboard = Keyboard::instance();
	Mouse& mouse = Mouse::instance();
	Vector3f direction = Vector3f();

	float dx = 0.0f;
	float dy = 0.0f;
	bool move = false;
	bool playerMove = false;

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


	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	m_instances.clear();

	m_instances.push_back({ Matrix4f::Translate(0.0f, -1.0f, 0.0f) * Matrix4f::Scale(12.5f, 0.5f, 12.5f), 
		                    Vector4f(1.0f, 1.0f, 1.0f, 1.0f) });

	m_instances.push_back({Matrix4f::Translate(0.0f, 1.5f, 0.0f) * Matrix4f::Scale(0.5f, 0.5f, 0.5f),
		                    Vector4f(1.0f, 1.0f, 1.0f, 1.0f)});

	m_instances.push_back({ Matrix4f::Translate(2.0f, 0.0f, 1.0f) * Matrix4f::Scale(0.5f, 0.5f, 0.5f), 
		                    Vector4f(1.0f, 1.0f, 1.0f, 1.0f) });

	m_instances.push_back({ Matrix4f::Translate(-1.0f, -1.0f, 2.0f) * Matrix4f::Rotate(Vector3f::Normalize(Vector3f(1.0, 0.0, 1.0)), 60.0f), 
		                    Vector4f(1.0f, 1.0f, 1.0f, 1.0f) });

	m_instances.push_back({ Matrix4f::Translate(0.0f, 2.7f, 4.0f) * Matrix4f::Scale(0.5f, 0.5f, 0.5f), 
		                    Vector4f(1.0f, 1.0f, 1.0f, 1.0f) });

	m_instances.push_back({ Matrix4f::Translate(-2.0f, 1.0f, -3.0f) * Matrix4f::Scale(0.5f, 0.5f, 0.5f), 
		                    Vector4f(1.0f, 1.0f, 1.0f, 1.0f) });

	m_instances.push_back({ Matrix4f::Translate(-3.0f, 0.0f, 0.0f) * Matrix4f::Rotate(Vector3f::Normalize(Vector3f(1.0, 0.0, 1.0)), 124.0f), 
		                    Vector4f(1.0f, 1.0f, 1.0f, 1.0f) });

	for (unsigned int i = 0; i < m_lightPositions.size(); i++) {
		m_instances.push_back({ Matrix4f::Translate(m_lightPositions[i]) * Matrix4f::Scale(0.25f, 0.25f, 0.25f),
								m_lightColors[i] });
	}

	wgpuQueueWriteBuffer(wgpContext.queue, m_storageBuffer.getBuffer(), 0u, m_instances.data(), m_instances.size() * sizeof(Instance));
}

void HDRBloom::render() {
	wgpDraw();
}

void HDRBloom::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	

	WgpRenderer::Draw(m_wgpSceneTarget, m_wgpSceneDepth, [this](const WGPURenderPassEncoder& renderPassEncoder) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BASE"));
		m_wgpCube.setBindGroupsSlot("BG_WOOD");
		m_wgpCube.draw(renderPassEncoder, 1u, 0u);

		m_wgpCube.setBindGroupsSlot("BG_CONTAINER");
		m_wgpCube.draw(renderPassEncoder, 6u, 1u);

		m_wgpCubeE.setBindGroupsSlot("BG_EMPTY");
		m_wgpCubeE.draw(renderPassEncoder, 4u, 7u);
	});

	WgpRenderer::Draw(m_wgpSceneFilterTarget, m_wgpSceneFilterDepth, [this](const WGPURenderPassEncoder& renderPassEncoder) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BASE_FILTER"));
		m_wgpCube.setBindGroupsSlot("BG_WOOD");
		m_wgpCube.draw(renderPassEncoder, 1u, 0u);

		m_wgpCube.setBindGroupsSlot("BG_CONTAINER");
		m_wgpCube.draw(renderPassEncoder, 6u, 1u);

		m_wgpCubeE.setBindGroupsSlot("BG_EMPTY");
		m_wgpCubeE.draw(renderPassEncoder, 4u, 7u);
	});

	RenderDownsamples(commandEncoder);
	RenderUpsamples(commandEncoder, 0.005f);

	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BLOOM_COMPOSITE"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupComposite, 0u, NULL);
		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void HDRBloom::OnMouseMotion(const Event::MouseMoveEvent& event) {

}

void HDRBloom::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {

		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void HDRBloom::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void HDRBloom::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void HDRBloom::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD

	if (event.keyCode == VK_LMENU) {
		
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		wgpCleanState();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Menu(m_machine));
	}
}

void HDRBloom::OnKeyUp(const Event::KeyboardEvent& event) {

}

void HDRBloom::OnReEnter(unsigned int prevState) {

}

void HDRBloom::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
}

void HDRBloom::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> HDRBloom::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(5);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Instance);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[4].binding = 4u;
	bindingLayoutEntries[4].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[4].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[4].buffer.minBindingSize = sizeof(Light);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> HDRBloom::OnBindGroupLayoutsDown() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = sizeof(DownsampleUniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> HDRBloom::OnBindGroupLayoutsUp() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = sizeof(UpsampleUniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

WGPUBindGroup  HDRBloom::createBindGroup(const WgpTexture& texture) {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(5);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_storageBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_storageBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].textureView = texture.getTextureView();

	bindGroupEntries[4].binding = 4u;
	bindGroupEntries[4].buffer = m_lightBuffer.getBuffer();
	bindGroupEntries[4].offset = 0u;
	bindGroupEntries[4].size = wgpuBufferGetSize(m_lightBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_BASE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

std::vector<WGPUBindGroupLayout> HDRBloom::OnBindGroupLayoutsComposite() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(4);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	bindingLayoutEntries[1].texture.multisampled = false;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	bindingLayoutEntries[2].texture.multisampled = false;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[3].buffer.minBindingSize = sizeof(CompositeUniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

WGPUBindGroup HDRBloom::createBindGroupComposite() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(4);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].textureView = m_wgpSceneTarget.getTextureView();

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_bloomFBO.mMipChain[0].sampledView;

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].buffer = m_compositeBuffer.getBuffer();
	bindGroupEntries[3].offset = 0u;
	bindGroupEntries[3].size = wgpuBufferGetSize(m_compositeBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_BLOOM_COMPOSITE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

void HDRBloom::CreateBloomBindGroups(WGPUDevice device, WGPUBindGroupLayout layout, WGPUSampler sampler, WGPUTextureView sceneColorView) {
	size_t mipCount = m_bloomFBO.mMipChain.size();
	m_downsampleBindGroups.resize(mipCount);
	m_upsampleBindGroups.resize(mipCount);

	for (size_t i = 0; i < mipCount; i++) {
		// --- DOWNSAMPLE ---
		WGPUTextureView currentSrcView = (i == 0) ? sceneColorView : m_bloomFBO.mMipChain[i - 1].sampledView;

		std::array<WGPUBindGroupEntry, 3> downsampleEntries{};
		downsampleEntries[0].binding = 0;
		downsampleEntries[0].sampler = sampler;
		downsampleEntries[1].binding = 1;
		downsampleEntries[1].textureView = currentSrcView;
		downsampleEntries[2].binding = 2;
		downsampleEntries[2].buffer = m_downSampleBuffer.getBuffer(); // Der Buffer von vorhin
		downsampleEntries[2].offset = 0;
		downsampleEntries[2].size = sizeof(DownsampleUniforms);

		WGPUBindGroupDescriptor downsampleDesc{};
		downsampleDesc.layout = layout;
		downsampleDesc.entryCount = downsampleEntries.size();
		downsampleDesc.entries = downsampleEntries.data();
		m_downsampleBindGroups[i] = wgpuDeviceCreateBindGroup(device, &downsampleDesc);

		// --- UPSAMPLE ---
		std::array<WGPUBindGroupEntry, 3> upsampleEntries{};
		upsampleEntries[0].binding = 0;
		upsampleEntries[0].sampler = sampler;
		upsampleEntries[1].binding = 1;
		upsampleEntries[1].textureView = m_bloomFBO.mMipChain[i].sampledView;
		upsampleEntries[2].binding = 2;
		upsampleEntries[2].buffer = m_upSampleBuffer.getBuffer(); // Der Buffer von vorhin
		upsampleEntries[2].offset = 0;
		upsampleEntries[2].size = sizeof(UpsampleUniforms);

		WGPUBindGroupDescriptor upsampleDesc{};
		upsampleDesc.layout = layout;
		upsampleDesc.entryCount = upsampleEntries.size();
		upsampleDesc.entries = upsampleEntries.data();
		m_upsampleBindGroups[i] = wgpuDeviceCreateBindGroup(device, &upsampleDesc);
	}
}

void HDRBloom::RenderDownsamples(const WGPUCommandEncoder& commandEncoder) {
	for (size_t i = 0; i < m_bloomFBO.mMipChain.size(); i++) {
		const auto& mip = m_bloomFBO.mMipChain[i];

		this->UpdateDownsampleUniformBuffer(wgpContext.queue, i);

		WGPURenderPassColorAttachment colorAttachment{};
		colorAttachment.view = mip.renderTargetView;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = WGPULoadOp_Clear;
		colorAttachment.storeOp = WGPUStoreOp_Store;
		colorAttachment.clearValue = WGPUColor{ 0.0, 0.0, 0.0, 1.0 };

		WGPURenderPassDescriptor passDesc{};
		passDesc.colorAttachmentCount = 1;
		passDesc.colorAttachments = &colorAttachment;

		WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &passDesc);

		//wgpuRenderPassEncoderSetViewport(pass, 0.0f, 0.0f, static_cast<float>(mip.width), static_cast<float>(mip.height), 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(pass, wgpContext.renderPipelines.at("RP_DOWN"));

		wgpuRenderPassEncoderSetBindGroup(pass, 0u, m_downsampleBindGroups[i], 0u, nullptr);

		wgpuRenderPassEncoderDraw(pass, 3u, 1u, 0u, 0u);

		wgpuRenderPassEncoderEnd(pass);
		wgpuRenderPassEncoderRelease(pass);
	}
}

void HDRBloom::RenderUpsamples(const WGPUCommandEncoder& commandEncoder, float baseFilterRadius) {
	int mipCount = static_cast<int>(m_bloomFBO.mMipChain.size());
	this->UpdateUpsampleUniformBuffer(wgpContext.queue, baseFilterRadius);
	for (int i = mipCount - 1; i > 0; i--) {
		const auto& nextMip = m_bloomFBO.mMipChain[i - 1];
		float dynamicRadius = baseFilterRadius * static_cast<float>(mipCount - i) * 1.2f;

		

		WGPURenderPassColorAttachment colorAttachment{};
		colorAttachment.view = nextMip.renderTargetView;
		colorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
		colorAttachment.loadOp = WGPULoadOp_Load;
		colorAttachment.storeOp = WGPUStoreOp_Store;

		WGPURenderPassDescriptor passDesc{};
		passDesc.colorAttachmentCount = 1;
		passDesc.colorAttachments = &colorAttachment;

		WGPURenderPassEncoder pass = wgpuCommandEncoderBeginRenderPass(commandEncoder, &passDesc);

		//wgpuRenderPassEncoderSetViewport(pass, 0.0f, 0.0f, static_cast<float>(nextMip.width), static_cast<float>(nextMip.height), 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(pass, wgpContext.renderPipelines.at("RP_UP"));
		wgpuRenderPassEncoderSetBindGroup(pass, 0u, m_upsampleBindGroups[i], 0u, nullptr);

		wgpuRenderPassEncoderDraw(pass, 3u, 1u, 0u, 0u);

		wgpuRenderPassEncoderEnd(pass);
		wgpuRenderPassEncoderRelease(pass);
	}
}

bool BloomFBO::Init(WGPUDevice device, unsigned int windowWidth, unsigned int windowHeight, unsigned int mipChainLength) {
	if (mInit) return true;

	uint32_t mipWidth = windowWidth;
	uint32_t mipHeight = windowHeight;

	WGPUTextureDescriptor texDesc{};
	texDesc.nextInChain = nullptr;
	texDesc.label = WGPU_STR("Bloom Mip-Chain Texture");
	texDesc.dimension = WGPUTextureDimension_2D;
	texDesc.size = WGPUExtent3D{ mipWidth, mipHeight, 1 };
	texDesc.mipLevelCount = mipChainLength;
	texDesc.sampleCount = 1;
	texDesc.format = WGPUTextureFormat_RGBA16Float;
	texDesc.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment;

	m_texture = wgpuDeviceCreateTexture(device, &texDesc);
	if (!m_texture) return false;

	for (uint32_t i = 0; i < mipChainLength; i++) {
		BloomMip mip;
	
		mip.width = mipWidth;
		mip.height = mipHeight;
		mip.size = Vector2f(static_cast<float>(mipWidth), static_cast<float>(mipHeight));

		WGPUTextureViewDescriptor rtViewDesc{};
		rtViewDesc.nextInChain = nullptr;
		rtViewDesc.label = WGPU_STR("Bloom RenderTarget View Mip");
		rtViewDesc.format = WGPUTextureFormat_RGBA16Float;
		rtViewDesc.dimension = WGPUTextureViewDimension_2D;
		rtViewDesc.baseMipLevel = i;
		rtViewDesc.mipLevelCount = 1;
		rtViewDesc.baseArrayLayer = 0;
		rtViewDesc.arrayLayerCount = 1;
		rtViewDesc.aspect = WGPUTextureAspect_All;

		mip.renderTargetView = wgpuTextureCreateView(m_texture, &rtViewDesc);

		WGPUTextureViewDescriptor sampleViewDesc{};
		sampleViewDesc.nextInChain = nullptr;
		sampleViewDesc.label = WGPU_STR("Bloom Sampled View Mip");
		sampleViewDesc.format = WGPUTextureFormat_RGBA16Float;
		sampleViewDesc.dimension = WGPUTextureViewDimension_2D;
		sampleViewDesc.baseMipLevel = i;
		sampleViewDesc.mipLevelCount = 1;
		sampleViewDesc.baseArrayLayer = 0;
		sampleViewDesc.arrayLayerCount = 1;
		sampleViewDesc.aspect = WGPUTextureAspect_All;

		mip.sampledView = wgpuTextureCreateView(m_texture, &sampleViewDesc);

		mMipChain.emplace_back(mip);

		mipWidth = std::max(1u, mipWidth / 2);
		mipHeight = std::max(1u, mipHeight / 2);

	}

	mInit = true;
	return true;
}

void BloomFBO::Cleanup() {
	for (auto& mip : mMipChain) {
		if (mip.renderTargetView) wgpuTextureViewRelease(mip.renderTargetView);
		if (mip.sampledView) wgpuTextureViewRelease(mip.sampledView);
	}
	mMipChain.clear();
	if (m_texture) {
		wgpuTextureDestroy(m_texture);
		wgpuTextureRelease(m_texture);
		m_texture = nullptr;
	}
	mInit = false;
}

void HDRBloom::UpdateDownsampleUniformBuffer(WGPUQueue queue, size_t mipIndex) {
	DownsampleUniforms uniforms{};

	// Für Mip 0 ist die Quelle die Base-Scene, danach das jeweils vorherige Mip-Level
	if (mipIndex == 0) {
		uniforms.srcResolution = Vector2f(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
		uniforms.mipLevel = mKarisAverageOnDownsample ? 0u : 1u;
	}
	else {
		const auto& prevMip = m_bloomFBO.mMipChain[mipIndex - 1];
		uniforms.srcResolution = prevMip.size;
		uniforms.mipLevel = 1u; // Karis Average nur beim allerersten Downsample
	}

	wgpuQueueWriteBuffer(queue, m_downSampleBuffer.getBuffer(), 0, &uniforms, sizeof(DownsampleUniforms));
}

void HDRBloom::UpdateUpsampleUniformBuffer(WGPUQueue queue, float filterRadius) {
	UpsampleUniforms uniforms{};
	uniforms.filterRadius = filterRadius;

	wgpuQueueWriteBuffer(queue, m_upSampleBuffer.getBuffer(), 0, &uniforms, sizeof(UpsampleUniforms));
}