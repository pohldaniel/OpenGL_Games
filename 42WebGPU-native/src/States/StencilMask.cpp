#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "StencilMask.h"
#include "Application.h"
#include "Globals.h"

StencilMask::StencilMask(StateMachine& machine) : State(machine, States::STENCIL_MASK) {

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

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

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

	wgpContext.addSahderModule("COLOR_PTN", "res/shader/color_PTN.wgsl");
	wgpContext.createRenderPipeline("COLOR_PTN", "RP_COLOR_PTN", VL_PTN, std::bind(&StencilMask::OnBindGroupLayoutsColor, this));

	wgpContext.addSahderModule("STENCIL", "res/shader/stencil.wgsl");
	wgpContext.createRenderPipeline("STENCIL", "RP_STENCIL_MASK", VL_PTN, std::bind(&StencilMask::OnBindGroupLayoutsStencil, this));
	wgpContext.createRenderPipeline("STENCIL", "RP_STENCIL_SET", VL_PTN, std::bind(&StencilMask::OnBindGroupLayoutsStencil, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined , WGPUCullMode_Undefined, SET }
	);

	wgpContext.OnDraw = std::bind(&StencilMask::OnDraw, this, std::placeholders::_1, std::placeholders::_2);

	m_quad.buildQuadXY({ -0.5f, -0.5f, 0.0f }, { 1.0f, 1.0f }, 1u, 1u);
	m_sphere.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 24u, 12u);
	m_cube.buildCube({ -0.5f, 0.5f, -0.5f }, { 1.0f, 1.0f, 1.0f }, 1u, 1u);
	m_jem.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 6u, 5u);
	m_cylinder.buildCylinder({ 0.0f, 0.0f, 0.0f }, 1.0f, 1.0f, 1.0f, true, true, 1u, 24u);
	m_cone.buildCylinder({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 1.0f, true, false, 1u, 24u);
	m_torus.buildTorus({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.5f, 12u, 24u);
	m_dice.buildTorus({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.5f, 8u, 8u);

	m_wgpModels.resize(8u);

	m_wgpQuad.create(m_quad);
	m_wgpQuad.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[0].create(m_quad);

	m_wgpSphere.create(m_sphere);
	m_wgpSphere.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[1].create(m_sphere);

	m_wgpCube.create(m_cube);
	m_wgpCube.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[2].create(m_cube);

	m_wgpTorus.create(m_torus);
	m_wgpTorus.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[3].create(m_torus);

	m_wgpJem.create(m_jem);
	m_wgpJem.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[4].create(m_jem);

	m_wgpCylinder.create(m_cylinder);
	m_wgpCylinder.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[5].create(m_cylinder);

	m_wgpCone.create(m_cone);
	m_wgpCone.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[6].create(m_cone);

	m_wgpDice.create(m_dice);
	m_wgpDice.setBindGroups("BG", std::bind(&StencilMask::OnBindGroupsColor, this));
	m_wgpModels[7].create(m_dice);

	m_scenes.resize(7u);
	InitScene(m_scenes[0], 100u, 0.0f / 7.0f, 1u, 1u);
	InitScene(m_scenes[1], 100u, 1.0f / 7.0f, 2u, 1u);

	m_maskScenes.resize(6u);
	InitScene(m_maskScenes[0], 1u, 0.5f, 0u, 1u);
}

StencilMask::~StencilMask() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
}

void StencilMask::fixedUpdate() {

}

void StencilMask::update() {
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

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = m_trackball.getTransform();
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_uniforms.view * m_uniforms.model);
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::IDENTITY;
	m_uniforms.lightPosition = Vector3f(1.0f, 8.0f, 10.0f);
	Vector3f::Normalize(m_uniforms.lightPosition);
	
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	float rotations[6][3] = {
   {0.0f, 0.0f, 0.0f},  /* front */
   {1.0f, 0.0f, 0.0f},  /* back */
   {0.0f, 0.0f, 0.5f},  /* left */
   {0.0f, 0.0f, -0.5f}, /* right */
   {-0.5f, 0.0f, 0.0f}, /* top */
   {0.5f, 0.0f, 0.0f},  /* bottom */
	};

	updateSceneMask(Globals::clock.getElapsedTimeSec(), m_maskScenes[0], rotations[0]);

	updateScene0(Globals::clock.getElapsedTimeSec(), m_scenes[0]);
	updateScene1(Globals::clock.getElapsedTimeSec(), m_scenes[1]);
}

void StencilMask::render() {
	wgpDraw();
}

void StencilMask::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	
	
	
	WGPURenderPassDepthStencilAttachment depthStencilAttachment = wgpCopyDepthStencilAttachment(renderPassDescriptor.depthStencilAttachment);
	WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
	

	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Clear;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
	rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;
	draw(commandEncoder, rndrPssDscrptor, m_maskScenes[0], 1u, wgpContext.renderPipelines.at("RP_STENCIL_SET"));

	depthStencilAttachment.stencilReadOnly = WGPUOptionalBool_False;
	depthStencilAttachment.stencilLoadOp = WGPULoadOp_Load;
	depthStencilAttachment.stencilStoreOp = WGPUStoreOp_Store;
	rndrPssDscrptor.depthStencilAttachment = &depthStencilAttachment;
	draw(commandEncoder, rndrPssDscrptor, m_scenes[1], 1u, wgpContext.renderPipelines.at("RP_STENCIL_MASK"));
}

void StencilMask::draw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor, const Scene& scene, uint32_t stencilRef, const WGPURenderPipeline& renderPipeline) {
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

void StencilMask::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void StencilMask::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);

}

void StencilMask::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void StencilMask::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void StencilMask::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void StencilMask::OnKeyUp(const Event::KeyboardEvent& event) {

}

void StencilMask::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void StencilMask::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	if (ImGui::Button("Toggle Debug")) {
		m_debug = !m_debug;
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> StencilMask::OnBindGroupLayoutsColor() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> StencilMask::OnBindGroupLayoutsStencil() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 20u * sizeof(float);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	return bindingLayouts;
}

std::vector<WGPUBindGroup> StencilMask::OnBindGroupsColor() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindings(1);

	bindings[0].binding = 0u;
	bindings[0].buffer = m_uniformBuffer.getBuffer();
	bindings[0].offset = 0u;
	bindings[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_COLOR_PTN"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
	return bindGroups;
}

void StencilMask::InitScene(Scene& scene, uint32_t numInstances, float hue, uint32_t geometryIndex, uint32_t geometryIndexCount) {
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
		hsl_to_rgba(hue + randf(0.0f, 0.2f), randf(0.7f, 1.0f), randf(0.5f, 0.8f), rgba);
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

		scene.objects[i].geometryIndex = geometryIndex + rand_elem(geometryIndexCount);
	}	
}

void StencilMask::updateScene0(float time, Scene& scene) {
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

void StencilMask::updateScene1(float time, Scene& scene) {
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

void StencilMask::updateSceneMask(float time, Scene& scene, float rotation[3]) {
	Matrix4f view = Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 45.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	wgpuQueueWriteBuffer(wgpContext.queue, scene.sharedUniformBuffer.getBuffer(), offsetof(Uniforms, view), &view, sizeof(Matrix4f));

	float worldX = ((static_cast<float>(Mouse::instance().xPos()) / static_cast<float>(Application::Width) * 2.0f) - 1.0f)  * 10.0f;
	float worldY = ((static_cast<float>(Application::Height - Mouse::instance().yPos()) / static_cast<float>(Application::Height) * 2.0f) - 1.0f) * 10.0f;

	for (uint32_t i = 0; i < scene.numObjects; i++) {
		Transform transform;
		transform.scale(10.0f, 10.0f, 1.0f);
		transform.translate(worldX, worldY, 0.0f);
		//transform.rotate((time * 0.53f + i) * _180_ON_PI, 0.0f, 0.0f, false);
		//transform.translate(0.0f, 0.0f, sinf(i * 9.721f + time * 0.1f) * 10.0f);
		//transform.rotate(0.0f, (i * 2.967f) * _180_ON_PI, 0.0f, false);
		//transform.rotate((i * 4.567f) * _180_ON_PI, 0.0f, 0.0f, false);
		//transform.translate(0.0f, 0.0f, sinf(i * 3.721f + time * 0.1f) * 10.0f);
		wgpuQueueWriteBuffer(wgpContext.queue, scene.objects[i].uniformBuffer.getBuffer(), 0u, &transform.getTransformationMatrix(), sizeof(Matrix4f));
	}
}

/* Convert HSL to RGBA */
void StencilMask::hsl_to_rgba(float h, float s, float l, float* rgba){
	/* Normalize hue to 0-1 range */
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

float StencilMask::randf(float min_val, float max_val){
	return min_val + ((float)rand() / (float)RAND_MAX) * (max_val - min_val);
}

uint32_t StencilMask::rand_elem(uint32_t count){
	return (uint32_t)(randf(0.0f, (float)count));
}