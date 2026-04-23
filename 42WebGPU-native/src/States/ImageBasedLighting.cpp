#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include "ImageBasedLighting.h"
#include "Application.h"
#include "Globals.h"


ImageBasedLighting::ImageBasedLighting(StateMachine& machine) : State(machine, States::IMAGE_BASED_LIGHTING) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 20.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);
	m_model.scale(m_zoom, m_zoom, m_zoom);

	m_wgpTextureCube.loadHDRICubeFromFile("res/textures/venice_sunset_1k.hdr", false, true);
	m_wgpTexture.loadHDRIFromFile("res/textures/venice_sunset_1k.hdr", true, true);
	
	//m_wgpTexture.loadFromFile("res/textures/palace.jpg");
	
	//m_helmet.loadModel("res/models/helmet.glb", Vector3f(0.0f, 0.0f, 1.0f), 180.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f, false, false, false, true, true);
	//m_helmet.loadModel("res/models/helmet.gltf", Vector3f(0.0f, 0.0f, 1.0f), 180.0f, Vector3f(0.0f, 0.0f, 0.0f), 1.0f, false, false, false, true, true);
	m_helmet.loadModel("res/models/helmet.obj");
	m_sphereObj.loadModel("res/models/sphere.obj");

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniformModelBuffer.createBuffer(768u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniformLightBuffer.createBuffer(128u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniformMVPBuffer.createBuffer(64u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_roughnessBuffer.createBuffer(4u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Linear, WGPUAddressMode_ClampToEdge, 1u, WGPUMipmapFilterMode_Nearest), SS_0);
	wgpContext.setClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });
	wgpContext.addSahderModule("TEXTURE", "res/shader/texture.wgsl");
	wgpContext.createRenderPipeline("TEXTURE", "RP_PTN", VL_PTN, std::bind(&ImageBasedLighting::OnBindGroupLayouts, this), 4u);

	wgpContext.addSahderModule("ENV_CUBE", "res/shader/env_cube.wgsl");
	wgpContext.createRenderPipeline("ENV_CUBE", "RP_ENV_CUBE", VL_P, std::bind(&ImageBasedLighting::OnBindGroupLayoutsEnvCube, this), 4u);

	wgpContext.addSahderModule("ENV_SPHERE", "res/shader/env_sphere.wgsl");
	wgpContext.createRenderPipeline("ENV_SPHERE", "RP_ENV_SPHERE", VL_P, std::bind(&ImageBasedLighting::OnBindGroupLayoutsEnvSphere, this), 4u);

	wgpContext.addSahderModule("PBR", "res/shader/pbr.wgsl");
	wgpContext.createRenderPipeline("PBR", "RP_PBR", VL_PTN, std::bind(&ImageBasedLighting::OnBindGroupLayoutsPBR, this), 4u);

	wgpContext.addSahderModule("SKYBOX", "res/shader/skybox.wgsl");
	wgpContext.createRenderPipeline("SKYBOX", "RP_SKYBOX", VL_P, std::bind(&ImageBasedLighting::OnBindGroupLayoutsSkybox, this), 4u);

	wgpContext.addSahderModule("IRRADIANCE", "res/shader/irradiance.wgsl");
	wgpContext.createRenderPipeline("IRRADIANCE", "RP_IRRADIANCE", VL_P, 
		std::bind(&ImageBasedLighting::OnBindGroupLayoutsIrradiance, this), 
		1u, 
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		false,
		false);

	wgpContext.addSahderModule("CUBE", "res/shader/cube_map.wgsl");
	wgpContext.createRenderPipeline("CUBE", "RP_CUBE", VL_P,
		std::bind(&ImageBasedLighting::OnBindGroupLayoutsCube, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		false,
		false);

	wgpContext.addSahderModule("PREFILTER", "res/shader/prefilter.wgsl");
	wgpContext.createRenderPipeline("PREFILTER", "RP_PREFILTER", VL_P,
		std::bind(&ImageBasedLighting::OnBindGroupLayoutsPrefilter, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		false,
		false);

	wgpContext.addSahderModule("BRDF", "res/shader/brdf.wgsl");
	wgpContext.createRenderPipeline("BRDF", "RP_BRDF", VL_NONE, 
		NULL, 
		1u, 
		WGPUPrimitiveTopology_TriangleList, 
		WGPUTextureFormat_RG16Float,
		false,
		false);

	m_wgpHelmet.create(m_helmet);
	m_wgpHelmet.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroups, this));
	AddBindgroups(m_wgpHelmet);

	m_uniforms.projectionMatrix = m_camera.getPerspectiveMatrix();
	m_uniforms.viewMatrix = m_camera.getViewMatrix();
	m_uniforms.envMatrix = m_camera.getRotationMatrix();
	m_uniforms.modelMatrix = Matrix4f::IDENTITY;
	m_uniforms.normalMatrix = Matrix4f::IDENTITY;	
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	m_trackball.reshape(Application::Width, Application::Height);

	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, false, false);
	m_cube.rewind();

	m_wgpCube.create(m_cube);
	
	m_sphere.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 49u, 49u, false, false);
	m_sphere.rewind();

	m_spherePBR.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 49u, 49u);
	m_wgpSphere.create(m_sphereObj);
	m_wgpSphere.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsEnvSphere, this));
	AddBindgroups(m_wgpSphere, m_wgpTexture, "RP_ENV_SPHERE");

	initMatrices();
	initIrradianceMatrices();
	initLights();

	m_wgpCube.setBindGroups("CUBE", std::bind(&ImageBasedLighting::OnBindGroupsCube, this));
	renderCube();

	m_wgpCube.setBindGroups("IRRADIANCE", std::bind(&ImageBasedLighting::OnBindGroupsIrradiance, this));
	renderIrradiance();

	m_wgpCube.setBindGroups("PREFILTER", std::bind(&ImageBasedLighting::OnBindGroupsPrefilter, this));
	renderPrefilter();

	renderBrdf();


	m_wgpCube.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsSkybox, this));


	m_wgpSpherePBR.create(m_sphereObj);
	m_wgpSpherePBR.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsPBR, this));

	wgpContext.OnDraw = std::bind(&ImageBasedLighting::OnDraw, this, std::placeholders::_1);
}

ImageBasedLighting::~ImageBasedLighting() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
	m_uniformModelBuffer.markForDelete();
}

void ImageBasedLighting::fixedUpdate() {

}

void ImageBasedLighting::update() {
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

	m_trackball.idle();
	applyTransformation(m_trackball);

	m_uniforms.projectionMatrix = m_camera.getPerspectiveMatrix();
	m_uniforms.viewMatrix = m_camera.getViewMatrix();
	m_uniforms.envMatrix = m_camera.getRotationMatrix();
	m_uniforms.modelMatrix = m_model;
	m_uniforms.normalMatrix = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.modelMatrix);
	m_uniforms.camPosition = m_camera.getPosition();
	
}

void ImageBasedLighting::render() {
	wgpDraw();
}

void ImageBasedLighting::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projectionMatrix), &m_uniforms.projectionMatrix, sizeof(Uniforms::projectionMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, viewMatrix), &m_uniforms.viewMatrix, sizeof(Uniforms::viewMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, envMatrix), &m_uniforms.envMatrix, sizeof(Uniforms::envMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, modelMatrix), &m_uniforms.modelMatrix, sizeof(Uniforms::modelMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, normalMatrix), &m_uniforms.normalMatrix, sizeof(Uniforms::normalMatrix));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, camPosition), &m_uniforms.camPosition, sizeof(Uniforms::camPosition));

	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_SKYBOX"));
	m_wgpCube.draw(renderPassEncoder);

	//wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_ENV_SPHERE"));
	//m_wgpSphere.draw(renderPassEncoder);

	//wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PTN"));
	//m_wgpHelmet.draw(renderPassEncoder);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PBR"));
	m_wgpSpherePBR.draw(renderPassEncoder, 12u);

	if (m_drawUi)
		renderUi(renderPassEncoder);
}

void ImageBasedLighting::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
	applyTransformation(m_trackball);
}

void ImageBasedLighting::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void ImageBasedLighting::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		applyTransformation(m_trackball);
	}else if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().detach();
	}
}

void ImageBasedLighting::OnMouseWheel(const Event::MouseWheelEvent& event) {
	if (event.direction == 0u) {
		m_camera.move(-0.5f);
	}

	if (event.direction == 1u) {
		m_camera.move(0.5f);
	}
}

void ImageBasedLighting::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void ImageBasedLighting::OnKeyUp(const Event::KeyboardEvent& event) {

}

void ImageBasedLighting::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void ImageBasedLighting::applyTransformation(TrackBall& arc) {
	m_uniforms.modelMatrix = arc.getTransform();
}

void ImageBasedLighting::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayouts() {
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

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries0(2);

	bindGroupEntries0[0].binding = 0u;
	bindGroupEntries0[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries0[0].offset = 0u;
	bindGroupEntries0[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries0[1].binding = 1u;
	bindGroupEntries0[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	WGPUBindGroupDescriptor bindGroupDesc0 = {};
	bindGroupDesc0.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PTN"), 0u);
	bindGroupDesc0.entryCount = (uint32_t)bindGroupEntries0.size();
	bindGroupDesc0.entries = bindGroupEntries0.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc0);

	return bindGroups;
}

void ImageBasedLighting::AddBindgroups(const WgpModel& model) {
	for (std::list<WgpMesh>::const_iterator it = model.getMeshes().begin(); it != model.getMeshes().end(); ++it) {
		const WgpMesh& mesh = *it;

		std::vector<WGPUBindGroupEntry> bindingGroupEntries(1);
		bindingGroupEntries[0].binding = 0u;
		bindingGroupEntries[0].textureView = mesh.getTexture().getTextureView();

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PTN"), 1u);
		bindGroupDesc.entryCount = (uint32_t)bindingGroupEntries.size();
		bindGroupDesc.entries = bindingGroupEntries.data();

		mesh.addBindGroup("BG", wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc));
	}
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsEnvCube() {
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
	//samplerBindingLayout.sampler.type = WGPUSamplerBindingType_NonFiltering;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(1);

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries1[0];
	textureBindingLayout.binding = 0u;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension_Cube;
	//textureBindingLayout.texture.sampleType = WGPUTextureSampleType_UnfilterableFloat;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType_Float;
	
	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsEnvCube() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries0(2);

	bindGroupEntries0[0].binding = 0u;
	bindGroupEntries0[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries0[0].offset = 0u;
	bindGroupEntries0[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries0[1].binding = 1u;
	bindGroupEntries0[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	WGPUBindGroupDescriptor bindGroupDesc0 = {};
	bindGroupDesc0.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_ENV_CUBE"), 0u);
	bindGroupDesc0.entryCount = (uint32_t)bindGroupEntries0.size();
	bindGroupDesc0.entries = bindGroupEntries0.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc0);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsEnvSphere() {
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
	//samplerBindingLayout.sampler.type = WGPUSamplerBindingType_NonFiltering;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(1);

	WGPUBindGroupLayoutEntry& textureBindingLayout = bindingLayoutEntries1[0];
	textureBindingLayout.binding = 0u;
	textureBindingLayout.visibility = WGPUShaderStage_Fragment;
	textureBindingLayout.texture.viewDimension = WGPUTextureViewDimension_2D;
	//textureBindingLayout.texture.sampleType = WGPUTextureSampleType_UnfilterableFloat;
	textureBindingLayout.texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsEnvSphere() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries0(2);

	bindGroupEntries0[0].binding = 0u;
	bindGroupEntries0[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries0[0].offset = 0u;
	bindGroupEntries0[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries0[1].binding = 1u;
	bindGroupEntries0[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	WGPUBindGroupDescriptor bindGroupDesc0 = {};
	bindGroupDesc0.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_ENV_SPHERE"), 0u);
	bindGroupDesc0.entryCount = (uint32_t)bindGroupEntries0.size();
	bindGroupDesc0.entries = bindGroupEntries0.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc0);

	return bindGroups;
}

void ImageBasedLighting::AddBindgroups(const WgpModel& model, const WgpTexture& texture, std::string pipelineName) {
	for (std::list<WgpMesh>::const_iterator it = model.getMeshes().begin(); it != model.getMeshes().end(); ++it) {
		const WgpMesh& mesh = *it;

		std::vector<WGPUBindGroupEntry> bindingGroupEntries(1);
		bindingGroupEntries[0].binding = 0u;
		bindingGroupEntries[0].textureView = texture.getTextureView();

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at(pipelineName), 1u);
		bindGroupDesc.entryCount = (uint32_t)bindingGroupEntries.size();
		bindGroupDesc.entries = bindingGroupEntries.data();

		mesh.addBindGroup("BG", wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc));
	}
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsPBR() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(2);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries0(3);
	bindingLayoutEntries0[0].binding = 0u;
	bindingLayoutEntries0[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries0[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries0[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries0[1].binding = 1u;
	bindingLayoutEntries0[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries0[1].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries0[1].buffer.minBindingSize = 768u;

	bindingLayoutEntries0[2].binding = 2u;
	bindingLayoutEntries0[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries0[2].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries0[2].buffer.minBindingSize = 128u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(5);
	WGPUBindGroupLayoutEntry& samplerBindingLayout = bindingLayoutEntries1[0];
	samplerBindingLayout.binding = 0u;
	samplerBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	WGPUBindGroupLayoutEntry& samplerBrdfBindingLayout = bindingLayoutEntries1[1];
	samplerBrdfBindingLayout.binding = 1u;
	samplerBrdfBindingLayout.visibility = WGPUShaderStage_Fragment;
	samplerBrdfBindingLayout.sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries1[2].binding = 2u;
	bindingLayoutEntries1[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries1[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries1[2].texture.sampleType = WGPUTextureSampleType_Float;

	bindingLayoutEntries1[3].binding = 3u;
	bindingLayoutEntries1[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries1[3].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries1[3].texture.sampleType = WGPUTextureSampleType_Float;

	bindingLayoutEntries1[4].binding = 4u;
	bindingLayoutEntries1[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries1[4].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries1[4].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsPBR() {
	std::vector<WGPUBindGroup> bindGroups(2);

	std::vector<WGPUBindGroupEntry> bindGroupEntries0(3);

	bindGroupEntries0[0].binding = 0u;
	bindGroupEntries0[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries0[0].offset = 0u;
	bindGroupEntries0[0].size = sizeof(Uniforms);

	bindGroupEntries0[1].binding = 1u;
	bindGroupEntries0[1].buffer = m_uniformModelBuffer.getBuffer();
	bindGroupEntries0[1].offset = 0u;
	bindGroupEntries0[1].size = 768u;

	bindGroupEntries0[2].binding = 2u;
	bindGroupEntries0[2].buffer = m_uniformLightBuffer.getBuffer();
	bindGroupEntries0[2].offset = 0u;
	bindGroupEntries0[2].size = 128u;

	WGPUBindGroupDescriptor bindGroupDesc0 = {};
	bindGroupDesc0.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PBR"), 0u);
	bindGroupDesc0.entryCount = (uint32_t)bindGroupEntries0.size();
	bindGroupDesc0.entries = bindGroupEntries0.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc0);

	std::vector<WGPUBindGroupEntry> bindGroupEntries1(5);

	bindGroupEntries1[0].binding = 0u;
	bindGroupEntries1[0].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);


	bindGroupEntries1[1].binding = 1u;
	bindGroupEntries1[1].sampler = wgpContext.getSampler(SS_0);

	bindGroupEntries1[2].binding = 2u;
	bindGroupEntries1[2].textureView = brdfView;


	bindGroupEntries1[3].binding = 3u;
	bindGroupEntries1[3].textureView = _wgpTextureIrradiance.getTextureView();

	bindGroupEntries1[4].binding = 4u;
	bindGroupEntries1[4].textureView = _wgpTexturePrefilter.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc1 = {};
	bindGroupDesc1.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PBR"), 1u);
	bindGroupDesc1.entryCount = (uint32_t)bindGroupEntries1.size();
	bindGroupDesc1.entries = bindGroupEntries1.data();
	bindGroups[1] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc1);

	return bindGroups;
}

void ImageBasedLighting::initMatrices() {
	const float distance = 2.8f;
	uint32_t idx = 0;
	for (uint32_t y = 0; y < 2; ++y) {
		for (uint32_t x = 0; x < 6; ++x) {
			float pos_x = (float)x * distance - (distance * (6 - 1)) / 2.0f;
			float pos_y = (float)y * distance - distance / 2.0f;
			m_models[idx] = Matrix4f::IDENTITY;
			m_models[idx].translate(pos_x, pos_y, 0.0f);
			idx++;
		}
	}
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformModelBuffer.getBuffer(), 0u, &m_models[0], 768u);
}

void ImageBasedLighting::initLights() {
	m_lights[0].position = { -10.0f, 10.0f, 10.0f };
	m_lights[1].position = { 10.0f, 10.0f, 10.0f };
	m_lights[2].position = { -10.0f, -10.0f, 10.0f };
	m_lights[3].position = { 10.0f, -10.0f, 10.0f };

	m_lights[0].color = { 100.0f, 100.0f, 100.0f };
	m_lights[1].color = { 100.0f, 100.0f, 100.0f };
	m_lights[2].color = { 100.0f, 100.0f, 100.0f };
	m_lights[3].color = { 100.0f, 100.0f, 100.0f };

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformLightBuffer.getBuffer(), 0u, &m_lights[0], 128u);
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsIrradiance() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 64u;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsIrradiance() {
	std::vector<WGPUBindGroup> bindGroups(1);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformMVPBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformMVPBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = _wgpTextureCube.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_IRRADIANCE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsCube() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 64u;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsCube() {
	std::vector<WGPUBindGroup> bindGroups(1);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformMVPBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformMVPBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpTexture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_CUBE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsPrefilter() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(4);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = 64u;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[3].buffer.minBindingSize = 4u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsPrefilter() {
	std::vector<WGPUBindGroup> bindGroups(1);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(4);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformMVPBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformMVPBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = _wgpTextureCube.getTextureView();

	bindGroupEntries[3].binding = 3u;
	bindGroupEntries[3].buffer = m_roughnessBuffer.getBuffer();
	bindGroupEntries[3].offset = 0u;
	bindGroupEntries[3].size = wgpuBufferGetSize(m_roughnessBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PREFILTER"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsSkybox() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}
std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsSkybox() {
	std::vector<WGPUBindGroup> bindGroups(1);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = _wgpTextureIrradiance.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_SKYBOX"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

void ImageBasedLighting::initIrradianceMatrices() {
	m_mvpInvCube[0] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_mvpInvCube[1] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_mvpInvCube[2] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_mvpInvCube[3] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));

	m_mvpInvCube[4] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_mvpInvCube[5] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_mvpCube[0] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f));
	m_mvpCube[1] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f));

	m_mvpCube[2] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));
	m_mvpCube[3] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));

	m_mvpCube[4] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, -1.0f, 0.0f));
	m_mvpCube[5] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, -1.0f, 0.0f));
}

void ImageBasedLighting::renderIrradiance() {

	_wgpTextureIrradiance.createEmpty(32u, 32u, 6u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);

	for (uint32_t face = 0; face < 6; face++) {

		WGPUTextureViewDescriptor faceViewDescriptor = {};
		faceViewDescriptor.label = WGPU_STR("texture_view");
		faceViewDescriptor.aspect = WGPUTextureAspect_All;
		faceViewDescriptor.baseArrayLayer = face;
		faceViewDescriptor.arrayLayerCount = 1u;
		faceViewDescriptor.baseMipLevel = 0u;
		faceViewDescriptor.mipLevelCount = 1u;
		faceViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
		faceViewDescriptor.format = WGPUTextureFormat_RGBA16Float;
		faceViewDescriptor.nextInChain = NULL;

		WGPUTextureView faceView = wgpuTextureCreateView(_wgpTextureIrradiance.getTexture(), &faceViewDescriptor);

		WGPURenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.view = faceView;
		renderPassColorAttachment.resolveTarget = NULL;
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
		renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
		renderPassColorAttachment.clearValue = { 0.0f, 1.0f, 0.0f, 1.0f };
		renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

		WGPURenderPassDescriptor renderPassDesc = {};
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &renderPassColorAttachment;
		renderPassDesc.timestampWrites = NULL;

		WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
		commandEncoderDescriptor.label = WGPU_STR("command_encoder");

		WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDescriptor);
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);

		wgpuQueueWriteBuffer(wgpContext.queue, m_uniformMVPBuffer.getBuffer(), 0u, &m_mvpInvCube[face], 64u);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, 32.0f, 32.0f, 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_IRRADIANCE"));

		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_wgpCube.getMeshes().back().getBindGroups("IRRADIANCE")[0], 0u, NULL);


		m_wgpCube.draw(renderPassEncoder);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
		wgpuTextureViewRelease(faceView);

		WGPUCommandBufferDescriptor commandBufferDescriptor = {};
		commandBufferDescriptor.label = WGPU_STR("command_buffer");
		WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
		wgpuCommandEncoderRelease(commandEncoder);

		wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);
		wgpuCommandBufferRelease(commandBuffer);
	}
}

void ImageBasedLighting::renderCube() {

	_wgpTextureCube.createEmpty(512u, 512u, 6u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);

	for (uint32_t face = 0; face < 6; face++) {

		WGPUTextureViewDescriptor faceViewDescriptor = {};
		faceViewDescriptor.label = WGPU_STR("texture_view");
		faceViewDescriptor.aspect = WGPUTextureAspect_All;
		faceViewDescriptor.baseArrayLayer = face;
		faceViewDescriptor.arrayLayerCount = 1u;
		faceViewDescriptor.baseMipLevel = 0u;
		faceViewDescriptor.mipLevelCount = 1u;
		faceViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
		faceViewDescriptor.format = WGPUTextureFormat_RGBA16Float;
		faceViewDescriptor.nextInChain = NULL;

		WGPUTextureView faceView = wgpuTextureCreateView(_wgpTextureCube.getTexture(), &faceViewDescriptor);

		WGPURenderPassColorAttachment renderPassColorAttachment = {};
		renderPassColorAttachment.view = faceView;
		renderPassColorAttachment.resolveTarget = NULL;
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
		renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
		renderPassColorAttachment.clearValue = { 0.0f, 1.0f, 0.0f, 1.0f };
		renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

		WGPURenderPassDescriptor renderPassDesc = {};
		renderPassDesc.colorAttachmentCount = 1;
		renderPassDesc.colorAttachments = &renderPassColorAttachment;
		renderPassDesc.timestampWrites = NULL;

		WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
		commandEncoderDescriptor.label = WGPU_STR("command_encoder");

		WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDescriptor);
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);

		wgpuQueueWriteBuffer(wgpContext.queue, m_uniformMVPBuffer.getBuffer(), 0u, &m_mvpCube[face], 64u);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, 512.0f, 512.0f, 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_CUBE"));

		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_wgpCube.getMeshes().back().getBindGroups("CUBE")[0], 0u, NULL);


		m_wgpCube.draw(renderPassEncoder);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
		wgpuTextureViewRelease(faceView);

		WGPUCommandBufferDescriptor commandBufferDescriptor = {};
		commandBufferDescriptor.label = WGPU_STR("command_buffer");
		WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
		wgpuCommandEncoderRelease(commandEncoder);

		wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);
		wgpuCommandBufferRelease(commandBuffer);
	}
}

void ImageBasedLighting::renderPrefilter() {
	const uint32_t ROUGHNESS_LEVELS = 5u;
	float vpWidth = 256.0f;
	float vpHeight = 256.0f;

	_wgpTexturePrefilter.createEmpty(256u, 256u, 6u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float, ROUGHNESS_LEVELS);

	for (uint32_t mip = 0; mip < ROUGHNESS_LEVELS; ++mip) {
		float roughness_val = (float)mip / (float)(ROUGHNESS_LEVELS - 1);
		wgpuQueueWriteBuffer(wgpContext.queue, m_roughnessBuffer.getBuffer(), 0, &roughness_val, sizeof(float));
		for (uint32_t face = 0; face < 6; ++face) {

			WGPUTextureViewDescriptor faceViewDescriptor = {};
			faceViewDescriptor.label = WGPU_STR("texture_view");
			faceViewDescriptor.aspect = WGPUTextureAspect_All;
			faceViewDescriptor.baseArrayLayer = face;
			faceViewDescriptor.arrayLayerCount = 1u;
			faceViewDescriptor.baseMipLevel = mip;
			faceViewDescriptor.mipLevelCount = 1u;
			faceViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
			faceViewDescriptor.format = WGPUTextureFormat_RGBA16Float;
			faceViewDescriptor.nextInChain = NULL;

			WGPUTextureView faceView = wgpuTextureCreateView(_wgpTexturePrefilter.getTexture(), &faceViewDescriptor);

			WGPURenderPassColorAttachment renderPassColorAttachment = {};
			renderPassColorAttachment.view = faceView;
			renderPassColorAttachment.resolveTarget = NULL;
			renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
			renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
			renderPassColorAttachment.clearValue = { 0.0f, 1.0f, 0.0f, 1.0f };
			renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

			WGPURenderPassDescriptor renderPassDesc = {};
			renderPassDesc.colorAttachmentCount = 1;
			renderPassDesc.colorAttachments = &renderPassColorAttachment;
			renderPassDesc.timestampWrites = NULL;

			WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
			commandEncoderDescriptor.label = WGPU_STR("command_encoder");

			WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDescriptor);
			WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);

			wgpuQueueWriteBuffer(wgpContext.queue, m_uniformMVPBuffer.getBuffer(), 0u, &m_mvpInvCube[face], 64u);
			wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, vpWidth, vpHeight, 0.0f, 1.0f);
			wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PREFILTER"));

			wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_wgpCube.getMeshes().back().getBindGroups("PREFILTER")[0], 0u, NULL);


			m_wgpCube.draw(renderPassEncoder);

			wgpuRenderPassEncoderEnd(renderPassEncoder);
			wgpuRenderPassEncoderRelease(renderPassEncoder);
			wgpuTextureViewRelease(faceView);

			WGPUCommandBufferDescriptor commandBufferDescriptor = {};
			commandBufferDescriptor.label = WGPU_STR("command_buffer");
			WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
			wgpuCommandEncoderRelease(commandEncoder);

			wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);
			wgpuCommandBufferRelease(commandBuffer);		
		}
		vpWidth *= 0.5f;
		vpHeight *= 0.5f;
	}
}

void ImageBasedLighting::renderBrdf() {
	WGPUTextureDescriptor textureDescriptor = {};
	textureDescriptor.label = WGPU_STR("texture");
	textureDescriptor.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
	textureDescriptor.size = { 512u, 512u, 1u };
	textureDescriptor.format = WGPUTextureFormat_RG16Float;
	textureDescriptor.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment;
	textureDescriptor.mipLevelCount = 1u;
	textureDescriptor.sampleCount = 1u;
	textureDescriptor.nextInChain = NULL;

	brdfTexture = wgpuDeviceCreateTexture(wgpContext.device, &textureDescriptor);

	WGPUTextureViewDescriptor textureViewDescriptor = {};
	textureViewDescriptor.label = WGPU_STR("texture_view");
	textureViewDescriptor.aspect = WGPUTextureAspect_All;
	textureViewDescriptor.baseArrayLayer = 0u;
	textureViewDescriptor.arrayLayerCount = 1u;
	textureViewDescriptor.baseMipLevel = 0u;
	textureViewDescriptor.mipLevelCount = 1u;
	textureViewDescriptor.dimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	textureViewDescriptor.format = WGPUTextureFormat_RG16Float;
	textureViewDescriptor.nextInChain = NULL;

	brdfView = wgpuTextureCreateView(brdfTexture, &textureViewDescriptor);

	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.view = brdfView;
	renderPassColorAttachment.resolveTarget = NULL;
	renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = { 0.0f, 1.0f, 0.0f, 1.0f };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

	WGPURenderPassDescriptor renderPassDesc = {};
	renderPassDesc.colorAttachmentCount = 1;
	renderPassDesc.colorAttachments = &renderPassColorAttachment;
	renderPassDesc.timestampWrites = NULL;

	WGPUCommandEncoderDescriptor commandEncoderDescriptor = {};
	commandEncoderDescriptor.label = WGPU_STR("command_encoder");

	WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, &commandEncoderDescriptor);
	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDesc);

	
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, 512.0f, 512.0f, 0.0f, 1.0f);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BRDF"));

	wgpuRenderPassEncoderDraw(renderPassEncoder, 3, 1, 0, 0);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
	//gpuTextureViewRelease(faceView);

	WGPUCommandBufferDescriptor commandBufferDescriptor = {};
	commandBufferDescriptor.label = WGPU_STR("command_buffer");
	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, &commandBufferDescriptor);
	wgpuCommandEncoderRelease(commandEncoder);

	wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);
	wgpuCommandBufferRelease(commandBuffer);
}