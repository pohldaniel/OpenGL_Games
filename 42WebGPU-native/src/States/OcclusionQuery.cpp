#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "OcclusionQuery.h"
#include "Application.h"
#include "Globals.h"

OcclusionQuery::OcclusionQuery(StateMachine& machine) : State(machine, States::OCCLUSION_QUERY) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	m_camera.perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.5f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(20.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);

	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, false, true);

	wgpContext.setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
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

	m_resolveBuffer.createBuffer(6u * sizeof(uint64_t), WGPUBufferUsage_QueryResolve | WGPUBufferUsage_CopySrc);
	m_resultBuffer.createBuffer(6u * sizeof(uint64_t), WGPUBufferUsage_CopyDst | WGPUBufferUsage_MapRead);

	wgpContext.addSahderModule("OCCLUSION", "res/shader/occlusion.wgsl");
	wgpContext.createRenderPipeline("OCCLUSION", "RP_OCCLUSION", VL_PNC, std::bind(&OcclusionQuery::OnBindGroupLayouts, this));

	wgpContext.OnDraw = std::bind(&OcclusionQuery::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	wgpContext.OnPostDraw = std::bind(&OcclusionQuery::OnPostDraw, this);

	m_scenes.resize(6);

	InitScene(m_scenes[0], m_cube, m_uniformBuffer, { 1.0f, 0.0f, 0.0f, 1.0f });
	InitScene(m_scenes[1], m_cube, m_uniformBuffer, { 1.0f, 1.0f, 0.0f, 1.0f });
	InitScene(m_scenes[2], m_cube, m_uniformBuffer, { 0.0f, 0.5f, 0.0f, 1.0f });
	InitScene(m_scenes[3], m_cube, m_uniformBuffer, { 1.0f, 0.6f, 0.0f, 1.0f });
	InitScene(m_scenes[4], m_cube, m_uniformBuffer, { 0.0f, 0.0f, 1.0f, 1.0f });
	InitScene(m_scenes[5], m_cube, m_uniformBuffer, { 0.5f, 0.0f, 0.5f, 1.0f });

	m_querySet = createQuerySet();
}

OcclusionQuery::~OcclusionQuery() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
	m_resolveBuffer.markForDelete();
	m_resultBuffer.markForDelete();

	wgpuQuerySetRelease(m_querySet);
	for (Scene& scene : m_scenes) {
		scene.uniformBuffer.markForDelete();
	}
}

void OcclusionQuery::fixedUpdate() {

}

void OcclusionQuery::update() {
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

	if (m_animate) {
		m_time += m_dt;
		Vector3f pos = Math::Lerp(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, 40.0f), PingPongSine(m_time * 0.2f));
		m_camera.setPosition(pos);
	}
	
	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix() * m_trackball.getTransform();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = m_trackball.getTransform();
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_uniforms.view * m_uniforms.model);
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::IDENTITY;
	m_uniforms.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	UpdateScene(m_scenes[0], Vector3f(-1.0f,  0.0f,  0.0f) * 10.0f, m_time);
	UpdateScene(m_scenes[1], Vector3f( 1.0f,  0.0f,  0.0f) * 10.0f, m_time);
	UpdateScene(m_scenes[2], Vector3f( 0.0f, -1.0f,  0.0f) * 10.0f, m_time);
	UpdateScene(m_scenes[3], Vector3f( 0.0f,  1.0f,  0.0f) * 10.0f, m_time);
	UpdateScene(m_scenes[4], Vector3f( 0.0f,  0.0f, -1.0f) * 10.0f, m_time);
	UpdateScene(m_scenes[5], Vector3f( 0.0f,  0.0f,  1.0f) * 10.0f, m_time);
}

void OcclusionQuery::render() {
	wgpDraw();
}

void OcclusionQuery::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	
	WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
	rndrPssDscrptor.occlusionQuerySet = m_querySet;
	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_OCCLUSION"));
		for (uint32_t i = 0u; i < 6u; ++i) {
			wgpuRenderPassEncoderBeginOcclusionQuery(renderPassEncoder, i);
			m_scenes[i].model.draw(renderPassEncoder);
			wgpuRenderPassEncoderEndOcclusionQuery(renderPassEncoder);
		}
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	
		wgpuCommandEncoderResolveQuerySet(commandEncoder, m_querySet, 0u, 6u, m_resolveBuffer.getBuffer(), 0u);
		if (wgpuBufferGetMapState(m_resultBuffer.getBuffer())== WGPUBufferMapState_Unmapped) {
			wgpuCommandEncoderCopyBufferToBuffer(commandEncoder, m_resolveBuffer.getBuffer(), 0u, m_resultBuffer.getBuffer(), 0u, 6u * sizeof(uint64_t));
		}	
	}

	if (m_drawUi)
	{
		WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;

		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		renderUi(renderPassEncoder);
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void OcclusionQuery::OnPostDraw() {
	if (wgpuBufferGetMapState(m_resultBuffer.getBuffer()) == WGPUBufferMapState_Unmapped) {
		WGPUBufferMapCallbackInfo bufferMapCallbackInfo = {};
		bufferMapCallbackInfo.callback = OnMapBuffer;
		bufferMapCallbackInfo.mode = WGPUCallbackMode_AllowProcessEvents;
		bufferMapCallbackInfo.userdata1 = &m_resultBuffer;
		bufferMapCallbackInfo.userdata2 = &m_scenes;

		wgpuBufferMapAsync(m_resultBuffer.getBuffer(), WGPUMapMode_Read, 0u, 6u * sizeof(uint64_t), bufferMapCallbackInfo);
	}
}

void OcclusionQuery::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void OcclusionQuery::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);

}

void OcclusionQuery::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void OcclusionQuery::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void OcclusionQuery::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void OcclusionQuery::OnKeyUp(const Event::KeyboardEvent& event) {

}

void OcclusionQuery::resize(int deltaW, int deltaH) {
	m_camera.perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.5f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void OcclusionQuery::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	ImGui::Checkbox("Animate", &m_animate);
	ImGui::Text("Visible:");
	const char* cube_labels[6u] = { "Red", "Yellow", "Green", "Orange", "Blue", "Purple" };

	bool firstVisible = true;
	for (size_t i = 0u; i < m_scenes.size(); ++i) {
		if (m_scenes[i].isVisible) {
			if (!firstVisible) {
				ImGui::SameLine(0.0f, 3.0f);
			}
			firstVisible = false;
			ImGui::PushID(static_cast<int>(i));
			ImGui::ColorButton(cube_labels[i], { m_scenes[i].color[0], m_scenes[i].color[1], m_scenes[i].color[2], m_scenes[i].color[3] }, ImGuiColorEditFlags_NoTooltip | ImGuiColorEditFlags_NoPicker, { 20.0f, 20.0f });
			ImGui::PopID();
		}
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> OcclusionQuery::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Matrix4f);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

WGPUQuerySet OcclusionQuery::createQuerySet() {
	WGPUQuerySetDescriptor querySetDescriptor = {};
	querySetDescriptor.label = WGPU_STR("query_set");
	querySetDescriptor.count = 6u;
	querySetDescriptor.type = WGPUQueryType_Occlusion;

	return wgpuDeviceCreateQuerySet(wgpContext.device, &querySetDescriptor);
}

float OcclusionQuery::PingPongSine(float t) {
	return sinf(t * TWO_PI) * 0.5f + 0.5f;
}

void OcclusionQuery::InitScene(Scene& scene, Shape& shape, const WgpBuffer& uniformBuffer, std::array<float, 4> color) {
	scene.isVisible = false;
	scene.color = color;
	scene.model.create(shape);
	scene.model.addColor(color);
	scene.uniformBuffer.createBuffer(sizeof(Matrix4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpuQueueWriteBuffer(wgpContext.queue, scene.uniformBuffer.getBuffer(), 0, &Matrix4f::IDENTITY, sizeof(Matrix4f));

	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = uniformBuffer.getBuffer();
	bindGroupEntries[0].size = wgpuBufferGetSize(uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = scene.uniformBuffer.getBuffer();
	bindGroupEntries[1].size = wgpuBufferGetSize(scene.uniformBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_OCCLUSION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	scene.model.addBindGroup("BG", wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc));
}

void OcclusionQuery::UpdateScene(Scene& scene, const Vector3f& position, float time) {
	Transform transform;
	transform.translate(-position);
	transform.rotate((-time) * _180_ON_PI, 0.0f, 0.0f, false);
	transform.rotate(0.0f, (-time * 0.7f) * _180_ON_PI, 0.0f, false);
	wgpuQueueWriteBuffer(wgpContext.queue, scene.uniformBuffer.getBuffer(), 0, &transform.getTransformationMatrix(), sizeof(Matrix4f));
}

void OcclusionQuery::OnMapBuffer(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPUMapAsyncStatus_Success) {
		WgpBuffer* buffer = static_cast<WgpBuffer*>(userdata1);
		std::vector<Scene>& scenes = *static_cast<std::vector<Scene>*>(userdata2);

		uint64_t const* mapping = (uint64_t*)wgpuBufferGetConstMappedRange(buffer->getBuffer(), 0u, 6u * sizeof(uint64_t));

		for (uint32_t i = 0; i < 6u; ++i) {
			scenes[i].isVisible = mapping[i] > 0;
		}

		wgpuBufferUnmap(buffer->getBuffer());
	}else {
		std::cout << "Buffer error: " << message.data << std::endl;
	}
}