#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "RenderBundles.h"
#include "Application.h"
#include "Globals.h"

RenderBundles::RenderBundles(StateMachine& machine) : State(machine, States::RENDER_BUNDLES), m_countAsteroids(5000u) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.5f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(4.0f, 0.1f * 180.0f, 0.0f, 0.1f * 180.0f);
	m_camera.setMovingSpeed(5.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	m_sphere.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 32u, 16u, true, true);

	m_asteroids.resize(5);
	m_asteroids[0].buildSphere({ 0.0f, 0.0f, 0.0f }, 0.01f , 0.15f, 8u, 6u, true, true);
	m_asteroids[1].buildSphere({ 0.0f, 0.0f, 0.0f }, 0.013f, 0.15f, 8u, 6u, true, true);
	m_asteroids[2].buildSphere({ 0.0f, 0.0f, 0.0f }, 0.017f, 0.15f, 8u, 6u, true, true);
	m_asteroids[3].buildSphere({ 0.0f, 0.0f, 0.0f }, 0.02f , 0.15f, 8u, 6u, true, true);
	m_asteroids[4].buildSphere({ 0.0f, 0.0f, 0.0f }, 0.03f , 0.15f, 16u, 8u, true, true);

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);	
	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::Translate(0.0f, -45.0f, 0.0f);
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	m_modelBuffer.createBuffer(sizeof(Matrix4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpuQueueWriteBuffer(wgpContext.queue, m_modelBuffer.getBuffer(), 0u, &Matrix4f::IDENTITY, sizeof(Matrix4f));

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	wgpContext.addSahderModule("RENDER_BUNDLES", "res/shader/render_bundles.wgsl");
	wgpContext.createRenderPipeline("RENDER_BUNDLES", "RP_RENDER_BUNDLES", VL_PTN, std::bind(&RenderBundles::OnBindGroupLayouts, this));

	m_saturnTexture.loadFromFile("res/textures/saturn.jpg", true);
	m_moonTexture.loadFromFile("res/textures/moon.jpg");
	m_wgpSphere.create(m_sphere);
	m_wgpSphere.setBindGroups("BG", std::bind(&RenderBundles::OnBindGroups, this));

	m_wgpAsteroids.resize(5);
	m_wgpAsteroids[0].create(m_asteroids[0]);
	m_wgpAsteroids[1].create(m_asteroids[1]);
	m_wgpAsteroids[2].create(m_asteroids[2]);
	m_wgpAsteroids[3].create(m_asteroids[3]);
	m_wgpAsteroids[4].create(m_asteroids[4]);

	wgpContext.OnDraw = std::bind(&RenderBundles::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	wgpContext.OnPostDraw = std::bind(&RenderBundles::OnPostDraw, this);
	placeAsteroids();
	updateRenderBundle();
}

RenderBundles::~RenderBundles() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
	m_modelBuffer.markForDelete();
	m_saturnTexture.markForDelete();
	m_moonTexture.markForDelete();

	for (const Renderable& renderable : m_renderables) {
		renderable.uniformBuffer.markForDelete();
		wgpuBindGroupRelease(renderable.bindGroup);
	}
	wgpuRenderBundleRelease(m_renderBundle);
}

void RenderBundles::fixedUpdate() {

}

void RenderBundles::update() {

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

	m_camera.rotateY(m_dt * 2.0f);

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = m_trackball.getTransform();
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;

	wgpuQueueWriteBuffer(wgpContext.queue, m_modelBuffer.getBuffer(), 0u, &m_uniforms.model, sizeof(Matrix4f));
}

void RenderBundles::render() {
	wgpDraw();
}

void RenderBundles::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
	
	if (m_useRendeBundle) {
		wgpuRenderPassEncoderExecuteBundles(renderPassEncoder, 1u, &m_renderBundle);
	}else {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_RENDER_BUNDLES"));
		m_wgpSphere.draw(renderPassEncoder);

		for (uint32_t index = 0u; index < m_countAsteroids; index++) {
			const Renderable& renderable = m_renderables[index];
			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 1u, renderable.bindGroup, 0u, NULL);
			m_wgpAsteroids[renderable.geometryIndex].draw(renderPassEncoder);
		}
	}

	if (m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
}

void RenderBundles::OnPostDraw() {
	if (m_replace) {
		placeAsteroids();
		wgpuRenderBundleRelease(m_renderBundle);
		updateRenderBundle();
		m_replace = false;
	}
}

void RenderBundles::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void RenderBundles::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);

}

void RenderBundles::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void RenderBundles::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void RenderBundles::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void RenderBundles::OnKeyUp(const Event::KeyboardEvent& event) {

}

void RenderBundles::resize(int deltaW, int deltaH) {
	m_camera.perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.5f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void RenderBundles::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	ImGui::Checkbox("Use Render Bundle", &m_useRendeBundle);
	if (ImGui::SliderInt("Asteroid Count", &m_countAsteroids, 1000, MAX_ASTEROID_COUNT)) {
		m_replace = true;
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> RenderBundles::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(2);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries0(1);

	bindingLayoutEntries0[0].binding = 0u;
	bindingLayoutEntries0[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries0[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries0[0].buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(3);

	bindingLayoutEntries1[0].binding = 0u;
	bindingLayoutEntries1[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries1[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries1[0].buffer.minBindingSize = sizeof(Matrix4f);

	bindingLayoutEntries1[1].binding = 1u;
	bindingLayoutEntries1[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries1[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries1[2].binding = 2u;
	bindingLayoutEntries1[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries1[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries1[2].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> RenderBundles::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(2);

	std::vector<WGPUBindGroupEntry> bindGroupEntries0(1);
	bindGroupEntries0[0].binding = 0u;
	bindGroupEntries0[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries0[0].offset = 0u;
	bindGroupEntries0[0].size = sizeof(Uniforms);

	WGPUBindGroupDescriptor bindGroupDesc0 = {};
	bindGroupDesc0.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_RENDER_BUNDLES"), 0u);
	bindGroupDesc0.entryCount = (uint32_t)bindGroupEntries0.size();
	bindGroupDesc0.entries = bindGroupEntries0.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc0);

	std::vector<WGPUBindGroupEntry> bindGroupEntries1(3);
	bindGroupEntries1[0].binding = 0u;
	bindGroupEntries1[0].buffer = m_modelBuffer.getBuffer();
	bindGroupEntries1[0].offset = 0u;
	bindGroupEntries1[0].size = sizeof(Matrix4f);


	bindGroupEntries1[1].binding = 1u;
	bindGroupEntries1[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries1[2].binding = 2u;
	bindGroupEntries1[2].textureView = m_saturnTexture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc1 = {};
	bindGroupDesc1.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_RENDER_BUNDLES"), 1u);
	bindGroupDesc1.entryCount = (uint32_t)bindGroupEntries1.size();
	bindGroupDesc1.entries = bindGroupEntries1.data();

	bindGroups[1] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc1);

	return bindGroups;
}

void RenderBundles::createAsteroid(Renderable& renderable, uint32_t geometryIndex, const Matrix4f& model) {
	renderable.geometryIndex = geometryIndex;

	renderable.uniformBuffer.createBuffer(sizeof(Matrix4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpuQueueWriteBuffer(wgpContext.queue, renderable.uniformBuffer.getBuffer(), 0u, &model, sizeof(Matrix4f));

	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = renderable.uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Matrix4f);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_moonTexture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_RENDER_BUNDLES"), 1u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	renderable.bindGroup = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

float random_float_min_max(float min, float max){
	return ((max - min) * ((float)rand() / (float)RAND_MAX)) + min;
}

float random_float(void){
	return random_float_min_max(0.0f, 1.0f); /* [0, 1.0] */
}

void RenderBundles::placeAsteroids() {
	for (const Renderable& renderable : m_renderables) {
		renderable.uniformBuffer.markForDelete();
		wgpuBindGroupRelease(renderable.bindGroup);
	}
	m_renderables.clear();
	m_renderables.shrink_to_fit();

	m_renderables.resize(m_countAsteroids);

	float radius = 0.0f, radians = 0.0f, x = 0.0f, y = 0.0f, z = 0.0f;
	Transform tranform;
	for (uint32_t index = 0u; index < m_countAsteroids; index++) {
		radius = random_float() * 1.7f + 1.25f;
		radians = random_float() * PI * 2.0f;
		x = sinf(radians) * radius;
		y = (random_float() - 0.5f) * 0.015f;
		z = cosf(radians) * radius;

		tranform.reset();
		tranform.rotate(0.0f, random_float() * 180.0f, 0.0f);
		tranform.rotate(random_float() * 180.0f, 0.0f, 0.0f);
		tranform.translate(x, y, z);

		createAsteroid(m_renderables[index], index % m_wgpAsteroids.size(), tranform.getTransformationMatrix());
	}
}

void RenderBundles::updateRenderBundle() {

	WGPURenderBundleEncoderDescriptor renderBundleEncoderDescriptor = {};
	renderBundleEncoderDescriptor.label = STRVIEW("render_bundle_encoder");
	renderBundleEncoderDescriptor.colorFormatCount = 1u;
	renderBundleEncoderDescriptor.colorFormats = &wgpContext.colorformat;
	renderBundleEncoderDescriptor.depthStencilFormat = wgpContext.depthformat;
	renderBundleEncoderDescriptor.sampleCount = wgpContext.msaaSampleCount;

	renderBundleEncoderDescriptor.depthReadOnly = WGPUOptionalBool::WGPUOptionalBool_False;
	renderBundleEncoderDescriptor.stencilReadOnly = WGPUOptionalBool::WGPUOptionalBool_True;

	WGPURenderBundleEncoder renderBundleEncoder = wgpuDeviceCreateRenderBundleEncoder(wgpContext.device, &renderBundleEncoderDescriptor);			
	wgpuRenderBundleEncoderSetPipeline(renderBundleEncoder, wgpContext.renderPipelines.at("RP_RENDER_BUNDLES"));
	m_wgpSphere.draw(renderBundleEncoder);

	for (uint32_t index = 0u; index < m_countAsteroids; index++) {
		const Renderable& renderable = m_renderables[index];
		wgpuRenderBundleEncoderSetBindGroup(renderBundleEncoder, 1u, renderable.bindGroup, 0u, NULL);
		m_wgpAsteroids[renderable.geometryIndex].draw(renderBundleEncoder);
	}

	m_renderBundle = wgpuRenderBundleEncoderFinish(renderBundleEncoder, NULL);
}