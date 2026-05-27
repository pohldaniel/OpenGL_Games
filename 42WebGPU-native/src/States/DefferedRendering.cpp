#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "DefferedRendering.h"
#include "Application.h"
#include "Globals.h"

DefferedRendering::DefferedRendering(StateMachine& machine) : State(machine, States::DEFFERED_RENDERING) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);

	m_camera.perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 35.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(20.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);

	m_quad.buildQuadXZ({ -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f }, 1u, 1u);
	m_sphere.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 24u, 12u);
	m_cube.buildCube({ -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, 1u, 1u);
	m_jem.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 6u, 5u);
	m_jem.flatShading();
	m_cylinder.buildCylinder({ 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, true, true, 1u, 24u);
	m_cone.buildCylinder({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 1.0f, true, false, 1u, 24u);
	m_torus.buildTorus({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.5f, 12u, 24u);
	m_dice.buildTorus({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.5f, 8u, 8u);
	m_dice.flatShading();

	wgpContext.setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	wgpContext.addSahderModule("STENCIL", "res/shader/stencil.wgsl");
	wgpContext.createRenderPipeline("STENCIL", "RP_STENCIL_MASK", VL_PTN, std::bind(&DefferedRendering::OnBindGroupLayoutsStencil, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined , WGPUCullMode_Undefined, MASK }
	);

	wgpContext.createRenderPipeline("STENCIL", "RP_STENCIL_SET", VL_PTN, std::bind(&DefferedRendering::OnBindGroupLayoutsStencil, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined , WGPUCullMode_Undefined, SET }
	);

	m_wgpModels.resize(8u);
	m_wgpModels[0].create(m_quad);
	m_wgpModels[1].create(m_sphere);
	m_wgpModels[2].create(m_cube);
	m_wgpModels[3].create(m_torus);
	m_wgpModels[4].create(m_jem);
	m_wgpModels[5].create(m_cylinder);
	m_wgpModels[6].create(m_cone);
	m_wgpModels[7].create(m_dice);

	wgpContext.OnDraw = std::bind(&DefferedRendering::OnDraw, this, std::placeholders::_1, std::placeholders::_2);

	m_scenes.resize(7u);
	InitScene(m_scenes[0], 100u, 0.0f / 7.0f, 1u, 1u);
	InitScene(m_scenes[1], 100u, 1.0f / 7.0f, 2u, 1u);
	InitScene(m_scenes[2], 100u, 2.0f / 7.0f, 3u, 1u);
	InitScene(m_scenes[3], 100u, 3.0f / 7.0f, 4u, 1u);
	InitScene(m_scenes[4], 100u, 4.0f / 7.0f, 5u, 1u);
	InitScene(m_scenes[5], 100u, 5.0f / 7.0f, 6u, 1u);
	InitScene(m_scenes[6], 100u, 6.0f / 7.0f, 7u, 1u);

	m_maskScenes.resize(6u);
	for (size_t i = 0; i < m_maskScenes.size(); i++) {
		float hue = (float)i / 6.0f + 0.5f;
		InitScene(m_maskScenes[i], 1u, hue, 0u, 1u);
	}
}

DefferedRendering::~DefferedRendering() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	for (Scene& scene : m_maskScenes) {
		scene.sharedUniformBuffer.markForDelete();
		for (Object& object : scene.objects) {
			object.uniformBuffer.markForDelete();
		}
	}

	for (Scene& scene : m_scenes) {
		scene.sharedUniformBuffer.markForDelete();
		for (Object& object : scene.objects) {
			object.uniformBuffer.markForDelete();
		}
	}
}

void DefferedRendering::fixedUpdate() {

}

void DefferedRendering::update() {
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

	if (mouse.buttonDownInvisible(Mouse::MouseButton::BUTTON_RIGHT)) {
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

	
	float rotations[6][3] = {						 							
							  { 0.0f, 0.0f,  0.0f },
							  { 2.0f, 0.0f,  0.0f },
							  { 0.0f, 0.0f,  1.0f },
							  { 0.0f, 0.0f, -1.0f },
							  {-1.0f, 0.0f,  0.0f },
							  { 1.0f, 0.0f,  0.0f },
							};

	for (size_t i = 0; i < m_maskScenes.size(); i++) {
		updateSceneMask(Globals::clock.getElapsedTimeSec() * 5.0f, m_maskScenes[i], i, rotations[i]);
	}

	UpdateScene0(Globals::clock.getElapsedTimeSec(), m_scenes[0]);
	UpdateScene1(Globals::clock.getElapsedTimeSec(), m_scenes[1]);
	UpdateScene0(Globals::clock.getElapsedTimeSec(), m_scenes[2]);

	UpdateScene1(Globals::clock.getElapsedTimeSec(), m_scenes[3]);
	UpdateScene0(Globals::clock.getElapsedTimeSec(), m_scenes[4]);
	UpdateScene1(Globals::clock.getElapsedTimeSec(), m_scenes[5]);
	UpdateScene0(Globals::clock.getElapsedTimeSec(), m_scenes[6]);
}

void DefferedRendering::render() {
	wgpDraw();
}

void DefferedRendering::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	WGPURenderPassDepthStencilAttachment depthStencilAttachment = wgpCopyDepthStencilAttachment(renderPassDescriptor.depthStencilAttachment);
	WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];

	WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
	{
		depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
		depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
		depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
		rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;
		draw(commandEncoder, rndrPssDscrptor, m_maskScenes[0], 1u, wgpContext.renderPipelines.at("RP_STENCIL_SET"));
	}
	
	{
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;
		depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
		depthStencilAttachment.stencilLoadOp = WGPULoadOp_Load;
		depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
		rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;

		draw(commandEncoder, rndrPssDscrptor, m_maskScenes[1], 2u, wgpContext.renderPipelines.at("RP_STENCIL_SET"));
		draw(commandEncoder, rndrPssDscrptor, m_maskScenes[2], 3u, wgpContext.renderPipelines.at("RP_STENCIL_SET"));
		draw(commandEncoder, rndrPssDscrptor, m_maskScenes[3], 4u, wgpContext.renderPipelines.at("RP_STENCIL_SET"));
		draw(commandEncoder, rndrPssDscrptor, m_maskScenes[4], 5u, wgpContext.renderPipelines.at("RP_STENCIL_SET"));
		draw(commandEncoder, rndrPssDscrptor, m_maskScenes[5], 6u, wgpContext.renderPipelines.at("RP_STENCIL_SET"));
		draw(commandEncoder, rndrPssDscrptor, m_scenes[0], 0u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));

		draw(commandEncoder, rndrPssDscrptor, m_scenes[1], 1u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));
		draw(commandEncoder, rndrPssDscrptor, m_scenes[2], 2u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));
		draw(commandEncoder, rndrPssDscrptor, m_scenes[3], 3u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));
		draw(commandEncoder, rndrPssDscrptor, m_scenes[4], 4u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));
		draw(commandEncoder, rndrPssDscrptor, m_scenes[5], 5u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));
		draw(commandEncoder, rndrPssDscrptor, m_scenes[6], 6u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));
	}

	if (m_drawUi)
	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		renderUi(renderPassEncoder);
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void DefferedRendering::draw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor, const Scene& scene, uint32_t stencilRef, const WGPURenderPipeline& renderPipeline) {
	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, renderPipeline);
	wgpuRenderPassEncoderSetStencilReference(renderPassEncoder, stencilRef);

	for (uint32_t i = 0; i < scene.numObjects; i++) {
		const WgpModel& model = m_wgpModels[scene.objects[i].geometryIndex];
		
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0, scene.objects[i].bindGroup, 0, NULL);
		model.draw(renderPassEncoder);
	}

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
}

void DefferedRendering::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void DefferedRendering::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);

}

void DefferedRendering::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void DefferedRendering::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void DefferedRendering::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void DefferedRendering::OnKeyUp(const Event::KeyboardEvent& event) {

}

void DefferedRendering::resize(int deltaW, int deltaH) {
	m_camera.perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void DefferedRendering::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

void DefferedRendering::updateSceneMask(float time, Scene& scene, size_t index, float rotation[3]) {
	Matrix4f view = Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 45.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	wgpuQueueWriteBuffer(wgpContext.queue, scene.sharedUniformBuffer.getBuffer(), offsetof(Uniforms, view), &view, sizeof(Matrix4f));

	float worldX = ((static_cast<float>(Mouse::instance().xPos()) / static_cast<float>(Application::Width) * 2.0f) - 1.0f) * 10.0f;
	float worldY = ((static_cast<float>(Application::Height - Mouse::instance().yPos()) / static_cast<float>(Application::Height) * 2.0f) - 1.0f) * 10.0f;

	for (uint32_t i = 0; i < scene.numObjects; i++) {
		Transform transform;
		transform.scale(10.0f, 10.0f, 10.0f);
		transform.rotate(rotation[0] * 90.0f, 0.0f, 0.0f, false);
		transform.rotate(0.0f, 0.0f, rotation[2] * 90.0f, false);
		transform.apply(m_trackball.getTransform());
		transform.translate(worldX, worldY, 0.0f);
		wgpuQueueWriteBuffer(wgpContext.queue, scene.objects[i].uniformBuffer.getBuffer(), 0u, &transform.getTransformationMatrix(), sizeof(Matrix4f));
	}
}

std::vector<WGPUBindGroupLayout> DefferedRendering::OnBindGroupLayoutsStencil() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 20u * sizeof(float);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	return bindingLayouts;
}

void DefferedRendering::InitScene(Scene& scene, uint32_t numInstances, float hue, uint32_t geometryIndex, uint32_t geometryIndexCount) {
	scene.numObjects = numInstances;
	scene.objects.resize(scene.numObjects);

	scene.sharedUniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	Uniforms uniforms;
	uniforms.projection = Matrix4f::Perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	uniforms.view = Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 35.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	uniforms.env = Matrix4f::GetRotationMatrix(uniforms.view);
	uniforms.model = Matrix4f::IDENTITY;
	uniforms.normal = Matrix4f::GetNormalMatrix(uniforms.view * uniforms.model);
	uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	uniforms.camPosition = Vector3f(0.0f, 0.0f, 35.0f);
	uniforms.lightVP = Matrix4f::IDENTITY;
	uniforms.shadow = Matrix4f::IDENTITY;
	uniforms.lightPosition = Vector3f(1.0f, 8.0f, 10.0f);
	Vector3f::Normalize(uniforms.lightPosition);

	wgpuQueueWriteBuffer(wgpContext.queue, scene.sharedUniformBuffer.getBuffer(), 0, &uniforms, sizeof(Uniforms));

	for (uint32_t i = 0; i < numInstances; i++) {	
		float rgba[4];
		HslToTgba(hue + Randf(0.0f, 0.2f), Randf(0.7f, 1.0f), Randf(0.5f, 0.8f), rgba);
		scene.objects[i].uniformValues[0]  = 1.0f; scene.objects[i].uniformValues[1]  = 0.0f; scene.objects[i].uniformValues[2]  = 0.0f; scene.objects[i].uniformValues[3]  = 0.0f;
		scene.objects[i].uniformValues[4]  = 0.0f; scene.objects[i].uniformValues[5]  = 1.0f; scene.objects[i].uniformValues[6]  = 0.0f; scene.objects[i].uniformValues[7]  = 0.0f;
		scene.objects[i].uniformValues[8]  = 0.0f; scene.objects[i].uniformValues[9]  = 0.0f; scene.objects[i].uniformValues[10] = 1.0f; scene.objects[i].uniformValues[11] = 0.0f;
		scene.objects[i].uniformValues[12] = 0.0f; scene.objects[i].uniformValues[13] = 0.0f; scene.objects[i].uniformValues[14] = 0.0f; scene.objects[i].uniformValues[15] = 1.0f;

		scene.objects[i].uniformValues[16] = rgba[0];
		scene.objects[i].uniformValues[17] = rgba[1];
		scene.objects[i].uniformValues[18] = rgba[2];
		scene.objects[i].uniformValues[19] = rgba[3];

		scene.objects[i].uniformBuffer.createBuffer(reinterpret_cast<void*>(scene.objects[i].uniformValues), 20u * sizeof(float), WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst);

		std::vector<WGPUBindGroupEntry> bindings(2);
		bindings[0].binding = 0u;
		bindings[0].buffer = scene.objects[i].uniformBuffer.getBuffer();
		bindings[0].offset = 0u;
		bindings[0].size = wgpuBufferGetSize(scene.objects[i].uniformBuffer.getBuffer());

		bindings[1].binding = 1u;
		bindings[1].buffer = scene.sharedUniformBuffer.getBuffer();
		bindings[1].offset = 0u;
		bindings[1].size = wgpuBufferGetSize(scene.sharedUniformBuffer.getBuffer());

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_STENCIL_MASK"), 0u);
		bindGroupDesc.entryCount = (uint32_t)bindings.size();
		bindGroupDesc.entries = bindings.data();
		scene.objects[i].bindGroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

		scene.objects[i].geometryIndex = geometryIndex + RandElem(geometryIndexCount);
	}	
}

void DefferedRendering::UpdateScene0(float time, Scene& scene) {
	for (uint32_t i = 0; i < scene.numObjects; i++) {		
		Transform transform;
		transform.rotate((time * 0.53f + i) * _180_ON_PI, 0.0f, 0.0f, false);
		transform.translate(0.0f, 0.0f, sinf(i * 9.721f + time * 0.1f) * 10.0f);
		transform.rotate(0.0f, (i * 2.967f) * _180_ON_PI, 0.0f, false);
		transform.rotate((i * 4.567f) * _180_ON_PI, 0.0f, 0.0f, false);
		transform.translate(0.0f, 0.0f, sinf(i * 3.721f + time * 0.1f) * 10.0f);
		wgpuQueueWriteBuffer(wgpContext.queue, scene.objects[i].uniformBuffer.getBuffer(), 0u, &transform.getTransformationMatrix(), sizeof(Matrix4f));
	}
}

void DefferedRendering::UpdateScene1(float time, Scene& scene) {
	float radius = 35.0f;
	float t = time * 0.5f;
	Matrix4f view = Matrix4f::LookAt(Vector3f(cosf(t) * radius, 4.0f, sinf(t) * radius), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	wgpuQueueWriteBuffer(wgpContext.queue, scene.sharedUniformBuffer.getBuffer(), offsetof(Uniforms, view), &view, sizeof(Matrix4f));

	for (uint32_t i = 0; i < scene.numObjects; i++) {
		Transform transform;
		transform.rotate((time * 0.53f + i) * _180_ON_PI, 0.0f, 0.0f, false);
		transform.translate(0.0f, 0.0f, sinf(i * 9.721f + time * 0.1f) * 10.0f);
		transform.rotate(0.0f, (i * 2.967f) * _180_ON_PI, 0.0f, false);
		transform.rotate((i * 4.567f) * _180_ON_PI, 0.0f, 0.0f, false);
		transform.translate(0.0f, 0.0f, sinf(i * 3.721f + time * 0.1f) * 10.0f);
		wgpuQueueWriteBuffer(wgpContext.queue, scene.objects[i].uniformBuffer.getBuffer(), 0u, &transform.getTransformationMatrix(), sizeof(Matrix4f));
	}
}

void DefferedRendering::HslToTgba(float h, float s, float l, float* rgba){
	h = fmodf(h, 1.0f);
	if (h < 0.0f)
		h += 1.0f;

	float c = (1.0f - fabsf(2.0f * l - 1.0f)) * s;
	float x = c * (1.0f - fabsf(fmodf(h * 6.0f, 2.0f) - 1.0f));
	float m = l - c / 2.0f;

	float r, g, b;
	if (h < 1.0f / 6.0f) {
		r = c;
		g = x;
		b = 0;
	}
	else if (h < 2.0f / 6.0f) {
		r = x;
		g = c;
		b = 0;
	}
	else if (h < 3.0f / 6.0f) {
		r = 0;
		g = c;
		b = x;
	}
	else if (h < 4.0f / 6.0f) {
		r = 0;
		g = x;
		b = c;
	}
	else if (h < 5.0f / 6.0f) {
		r = x;
		g = 0;
		b = c;
	}
	else {
		r = c;
		g = 0;
		b = x;
	}

	rgba[0] = r + m;
	rgba[1] = g + m;
	rgba[2] = b + m;
	rgba[3] = 1.0f;
}

float DefferedRendering::Randf(float min_val, float max_val){
	return min_val + ((float)rand() / (float)RAND_MAX) * (max_val - min_val);
}

uint32_t DefferedRendering::RandElem(uint32_t count){
	return (uint32_t)(Randf(0.0f, (float)count));
}