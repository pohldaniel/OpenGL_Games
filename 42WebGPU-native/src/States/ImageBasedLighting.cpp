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

	wgpSetMSAASampleCount(4u, Application::OnSurfaceChange);
	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);

	m_camera.perspective(90.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(4.1115341187f, 0.5f * _180_ON_PI,  0.0f);
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(10.0f);

	m_trackball.reshape(Application::Width, Application::Height);	
	m_quad.buildQuadXY({ -1.0f, -1.0f, 0.0f }, { 2.0f, 2.0f }, 1u, 1u, true, false);
	m_spherePBR.buildSphere({ 0.0f, 0.0f, 0.0f }, 1.0f, 0.0f, 49u, 49u, true, true);
	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, false, false);
	m_cube.rewind();	
	m_helmet.loadModel("res/models/helmet/helmet.obj", false, false, true);

	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Linear, WGPUAddressMode_ClampToEdge, 1u, WGPUMipmapFilterMode_Undefined), SS_0);
	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Linear, WGPUAddressMode_Repeat, 1u, WGPUMipmapFilterMode_Undefined), SS_1);
	wgpContext.addSampler(wgpCreateSampler(WGPUFilterMode_Linear, WGPUAddressMode_Repeat, 1u, WGPUMipmapFilterMode_Undefined, WGPUCompareFunction_LessEqual), SS_2);
	wgpContext.setClearColor({ 1.0f, 1.0f, 1.0f, 1.0f });

	
	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniformMaterial.createBuffer(sizeof(MaterialUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_instanceBuffer.createBuffer(sizeof(Matrix4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);
	m_uniformModelBuffer.createBuffer(768u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniformLightBuffer.createBuffer(128u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniformMVPBuffer.createBuffer(64u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_roughnessBuffer.createBuffer(4u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_wgpTextureHDRCube.loadHDRICubeFromFile("res/textures/venice_sunset_1k.hdr", false, true);
	m_wgpTextureHDR.loadHDRIFromFile("res/textures/venice_sunset_1k.hdr", true, true);

	m_wgpTextutreNormal.loadFromFile("res/models/helmet/helmet_normal.png");
	m_wgpTextutreEmission.loadFromFile("res/models/helmet/helmet_emission.png");
	m_wgpTextutreMetalness.loadFromFile("res/models/helmet/helmet_metalness.png");
	m_wgpTextutreLightmap.loadFromFile("res/models/helmet/helmet_lightmap.png");

	wgpContext.addSahderModule("PBR", "res/shader/pbr.wgsl");
	wgpContext.createRenderPipeline("PBR", "RP_PBR", VL_PTN, std::bind(&ImageBasedLighting::OnBindGroupLayoutsPBR, this), 4u);

	wgpContext.addSahderModule("PBR_HELMET", "res/shader/pbr_helmet.wgsl");
	wgpContext.createRenderPipeline("PBR_HELMET", "RP_PBR_HELMET", VL_PTNTB, std::bind(&ImageBasedLighting::OnBindGroupLayoutsPBRHelmet, this), 4u);

	wgpContext.addSahderModule("SKYBOX", "res/shader/skybox.wgsl");
	wgpContext.createRenderPipeline("SKYBOX", "RP_SKYBOX", VL_P, 
		std::bind(&ImageBasedLighting::OnBindGroupLayoutsSkybox, this), 
		4u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_LessEqual,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING }
	);
	
	wgpContext.addSahderModule("IRRADIANCE", "res/shader/irradiance.wgsl");
	wgpContext.createRenderPipeline("IRRADIANCE", "RP_IRRADIANCE", VL_P, 
		std::bind(&ImageBasedLighting::OnBindGroupLayoutsIrradiance, this), 
		1u, 
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING }
	);

	wgpContext.addSahderModule("CUBE", "res/shader/cube_map.wgsl");
	wgpContext.createRenderPipeline("CUBE", "RP_CUBE", VL_P,
		std::bind(&ImageBasedLighting::OnBindGroupLayoutsCube, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING }
	);

	wgpContext.addSahderModule("PREFILTER", "res/shader/prefilter.wgsl");
	wgpContext.createRenderPipeline("PREFILTER", "RP_PREFILTER", VL_P,
		std::bind(&ImageBasedLighting::OnBindGroupLayoutsPrefilter, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_RGBA16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING }
	);

	wgpContext.addSahderModule("BRDF", "res/shader/brdf.wgsl");
	wgpContext.createRenderPipeline("BRDF", "RP_BRDF", VL_PT,
		NULL, 
		1u, 
		WGPUPrimitiveTopology_TriangleList, 
		WGPUTextureFormat_RG16Float,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING }
	);

	m_wgpCube.create(m_cube);
	m_wgpQuad.create(m_quad);
	m_wgpSpherePBR.create(m_spherePBR);
	m_wgpHelmet.create(m_helmet);

	m_lightProjection = Matrix4f::Orthographic(-20.0f, 20.0f, -20.0f, 20.0f, 0.0f, 100.0f);
	m_lightView = Matrix4f::LookAt(Vector3f(0.25f, 0.5f, 1.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = m_lightProjection * m_lightView;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(0.25f, 0.5f, 1.0f);
	
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	m_material.alphaCutoff = 0.5f;
	m_material.baseColorFactor = { 1.0f, 1.0f, 1.0f, 1.0f };
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformMaterial.getBuffer(), 0, &m_material, sizeof(MaterialUniforms));

	initMatrices();
	initIrradianceMatrices();
	initLights();

	m_wgpTextureShadow.createEmpty(1u, 1u, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_Depth32Float);
	m_wgpCube.setBindGroups("CUBE", std::bind(&ImageBasedLighting::OnBindGroupsCube, this));
	m_wgpTextureCube.createEmpty(512u, 512u, 6u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);
	WgpRenderer::Draw(m_wgpTextureCube, std::bind(&ImageBasedLighting::OnDrawCube, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_wgpCube.setBindGroups("IRRADIANCE", std::bind(&ImageBasedLighting::OnBindGroupsIrradiance, this));
	m_wgpTextureIrradiance.createEmpty(32u, 32u, 6u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);
	WgpRenderer::Draw(m_wgpTextureIrradiance, std::bind(&ImageBasedLighting::OnDrawIrradiance, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_wgpCube.setBindGroups("PREFILTER", std::bind(&ImageBasedLighting::OnBindGroupsPrefilter, this));
	m_wgpTexturePrefilter.createEmpty(256u, 256u, 6u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float, ROUGHNESS_LEVELS);
	WgpRenderer::Draw(m_wgpTexturePrefilter, std::bind(&ImageBasedLighting::OnDrawPrefilter, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_wgpTextureBrdf.createEmpty(512, 512, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RG16Float);
	WgpRenderer::Draw(m_wgpTextureBrdf, std::bind(&ImageBasedLighting::OnDrawBrdf, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	m_wgpCube.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsSkyboxHelmet, this));
	m_wgpSpherePBR.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsPBR, this));
	m_wgpHelmet.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsPBRHelmet, this));

	wgpContext.OnDraw = std::bind(&ImageBasedLighting::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

ImageBasedLighting::~ImageBasedLighting() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
	m_uniformModelBuffer.markForDelete();
	m_uniformLightBuffer.markForDelete();
	m_uniformMVPBuffer.markForDelete();
	m_roughnessBuffer.markForDelete();
	m_uniformMaterial.markForDelete();
	m_instanceBuffer.markForDelete();

	m_wgpTextureHDRCube.markForDelete();
	m_wgpTextureHDR.markForDelete();
	m_wgpTextureCube.markForDelete();
	m_wgpTextureIrradiance.markForDelete();
	m_wgpTexturePrefilter.markForDelete();
	m_wgpTextureBrdf.markForDelete();
	m_wgpTextureShadow.markForDelete();

	m_wgpTextutreNormal.markForDelete();
	m_wgpTextutreEmission.markForDelete();
	m_wgpTextutreMetalness.markForDelete();
	m_wgpTextutreLightmap.markForDelete();
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

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	//m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = m_lightProjection * m_lightView;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	
}

void ImageBasedLighting::render() {
	wgpDraw();
}

void ImageBasedLighting::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projection), &m_uniforms.projection, sizeof(Uniforms::projection));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, view), &m_uniforms.view, sizeof(Uniforms::view));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, env), &m_uniforms.env, sizeof(Uniforms::env));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, model), &m_uniforms.model, sizeof(Uniforms::model));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, normal), &m_uniforms.normal, sizeof(Uniforms::normal));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, camPosition), &m_uniforms.camPosition, sizeof(Uniforms::camPosition));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, lightVP), &m_uniforms.lightVP, sizeof(Uniforms::lightVP));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, shadow), &m_uniforms.shadow, sizeof(Uniforms::shadow));

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	if (m_scene == Scene::HELMET) {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PBR_HELMET"));
		m_wgpHelmet.draw(renderPassEncoder);
	}else {
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PBR"));
		m_wgpSpherePBR.draw(renderPassEncoder, 12u);
	}

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_SKYBOX"));
	m_wgpCube.draw(renderPassEncoder);

	if (m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
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
	m_uniforms.model = arc.getTransform();
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
	int currentScene = m_scene;
	if (ImGui::Combo("Scene", &currentScene, "Sphere\0Helmet\0\0")) {
		m_scene = static_cast<Scene>(currentScene);
		if (m_scene == Scene::HELMET) {
			m_wgpCube.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsSkyboxHelmet, this));
			m_camera.perspective(90.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
			m_camera.lookAt(4.1115341187f, 0.5f * _180_ON_PI, 0.0f);
		}else {
			m_wgpCube.setBindGroups("BG", std::bind(&ImageBasedLighting::OnBindGroupsSkybox, this));
			m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
			m_camera.lookAt(Vector3f(0.0f, 0.0f, 20.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
		}
		m_trackball.reset();
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
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
	bindGroupEntries1[2].textureView = m_wgpTextureBrdf.getTextureView();


	bindGroupEntries1[3].binding = 3u;
	bindGroupEntries1[3].textureView = m_wgpTextureIrradiance.getTextureView();

	bindGroupEntries1[4].binding = 4u;
	bindGroupEntries1[4].textureView = m_wgpTexturePrefilter.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc1 = {};
	bindGroupDesc1.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PBR"), 1u);
	bindGroupDesc1.entryCount = (uint32_t)bindGroupEntries1.size();
	bindGroupDesc1.entries = bindGroupEntries1.data();
	bindGroups[1] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc1);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> ImageBasedLighting::OnBindGroupLayoutsPBRHelmet() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(4);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries0(1);

	bindingLayoutEntries0[0].binding = 0u;
	bindingLayoutEntries0[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries0[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries0[0].buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(1);
	bindingLayoutEntries1[0].binding = 0u;
	bindingLayoutEntries1[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries1[0].buffer.type = WGPUBufferBindingType_ReadOnlyStorage;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries2(11);

	bindingLayoutEntries2[0].binding = 0u;
	bindingLayoutEntries2[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries2[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries2[0].buffer.minBindingSize = sizeof(MaterialUniforms);

	//ALBEDO
	bindingLayoutEntries2[1].binding = 1u;
	bindingLayoutEntries2[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries2[2].binding = 2u;
	bindingLayoutEntries2[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries2[2].texture.sampleType = WGPUTextureSampleType_Float;

	//NORMAL
	bindingLayoutEntries2[3].binding = 3u;
	bindingLayoutEntries2[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[3].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries2[4].binding = 4u;
	bindingLayoutEntries2[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[4].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries2[4].texture.sampleType = WGPUTextureSampleType_Float;

	//ROUGHNESS
	bindingLayoutEntries2[5].binding = 5u;
	bindingLayoutEntries2[5].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[5].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries2[6].binding = 6u;
	bindingLayoutEntries2[6].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[6].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries2[6].texture.sampleType = WGPUTextureSampleType_Float;

	//AO
	bindingLayoutEntries2[7].binding = 7u;
	bindingLayoutEntries2[7].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[7].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries2[8].binding = 8u;
	bindingLayoutEntries2[8].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[8].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries2[8].texture.sampleType = WGPUTextureSampleType_Float;

	//EMISSIVE
	bindingLayoutEntries2[9].binding = 9u;
	bindingLayoutEntries2[9].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[9].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries2[10].binding = 10u;
	bindingLayoutEntries2[10].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries2[10].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries2[10].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor2 = {};
	bindGroupLayoutDescriptor2.entryCount = (uint32_t)bindingLayoutEntries2.size();
	bindGroupLayoutDescriptor2.entries = bindingLayoutEntries2.data();

	bindingLayouts[2] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor2);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries3(7);
	//BRDF
	bindingLayoutEntries3[0].binding = 0u;
	bindingLayoutEntries3[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries3[0].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries3[1].binding = 1u;
	bindingLayoutEntries3[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries3[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries3[2].binding = 2u;
	bindingLayoutEntries3[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries3[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries3[2].texture.sampleType = WGPUTextureSampleType_Float;

	bindingLayoutEntries3[3].binding = 3u;
	bindingLayoutEntries3[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries3[3].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries3[3].texture.sampleType = WGPUTextureSampleType_Float;

	bindingLayoutEntries3[4].binding = 4u;
	bindingLayoutEntries3[4].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries3[4].texture.viewDimension = WGPUTextureViewDimension_Cube;
	bindingLayoutEntries3[4].texture.sampleType = WGPUTextureSampleType_Float;

	bindingLayoutEntries3[5].binding = 5u;
	bindingLayoutEntries3[5].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries3[5].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries3[5].texture.sampleType = WGPUTextureSampleType_Depth;

	bindingLayoutEntries3[6].binding = 6u;
	bindingLayoutEntries3[6].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries3[6].sampler.type = WGPUSamplerBindingType_Comparison;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor3 = {};
	bindGroupLayoutDescriptor3.entryCount = (uint32_t)bindingLayoutEntries3.size();
	bindGroupLayoutDescriptor3.entries = bindingLayoutEntries3.data();

	bindingLayouts[3] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor3);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsPBRHelmet() {
	std::vector<WGPUBindGroup> bindGroups(4);

	std::vector<WGPUBindGroupEntry> bindGroupEntries0(1);
	bindGroupEntries0[0].binding = 0u;
	bindGroupEntries0[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries0[0].offset = 0u;
	bindGroupEntries0[0].size = sizeof(Uniforms);

	WGPUBindGroupDescriptor bindGroupDesc0 = {};
	bindGroupDesc0.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PBR_HELMET"), 0u);
	bindGroupDesc0.entryCount = (uint32_t)bindGroupEntries0.size();
	bindGroupDesc0.entries = bindGroupEntries0.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc0);

	std::vector<WGPUBindGroupEntry> bindGroupEntries1(1);

	bindGroupEntries1[0].binding = 0u;
	bindGroupEntries1[0].buffer = m_instanceBuffer.getBuffer();
	bindGroupEntries1[0].offset = 0u;
	bindGroupEntries1[0].size = sizeof(Matrix4f);

	WGPUBindGroupDescriptor bindGroupDesc1 = {};
	bindGroupDesc1.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PBR_HELMET"), 1u);
	bindGroupDesc1.entryCount = (uint32_t)bindGroupEntries1.size();
	bindGroupDesc1.entries = bindGroupEntries1.data();
	bindGroups[1] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries2(11);
	bindGroupEntries2[0].binding = 0u;
	bindGroupEntries2[0].buffer = m_uniformMaterial.getBuffer();
	bindGroupEntries2[0].offset = 0u;
	bindGroupEntries2[0].size = sizeof(MaterialUniforms);

	bindGroupEntries2[1].binding = 1u;
	bindGroupEntries2[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries2[2].binding = 2u;
	bindGroupEntries2[2].textureView = m_wgpHelmet.getMeshes().begin()->getTexture().getTextureView();

	bindGroupEntries2[3].binding = 3u;
	bindGroupEntries2[3].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries2[4].binding = 4u;
	bindGroupEntries2[4].textureView = m_wgpTextutreNormal.getTextureView();

	bindGroupEntries2[5].binding = 5u;
	bindGroupEntries2[5].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries2[6].binding = 6u;
	bindGroupEntries2[6].textureView = m_wgpTextutreMetalness.getTextureView();

	bindGroupEntries2[7].binding = 7u;
	bindGroupEntries2[7].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries2[8].binding = 8u;
	bindGroupEntries2[8].textureView = m_wgpTextutreLightmap.getTextureView();

	bindGroupEntries2[9].binding = 9u;
	bindGroupEntries2[9].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries2[10].binding = 10u;
	bindGroupEntries2[10].textureView = m_wgpTextutreEmission.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc2 = {};
	bindGroupDesc2.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PBR_HELMET"), 2u);
	bindGroupDesc2.entryCount = (uint32_t)bindGroupEntries2.size();
	bindGroupDesc2.entries = bindGroupEntries2.data();
	bindGroups[2] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc2);


	std::vector<WGPUBindGroupEntry> bindGroupEntries3(7);
	bindGroupEntries3[0].binding = 0u;
	bindGroupEntries3[0].sampler = wgpContext.getSampler(SS_1);

	bindGroupEntries3[1].binding = 1u;
	bindGroupEntries3[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries3[2].binding = 2u;
	bindGroupEntries3[2].textureView = m_wgpTextureBrdf.getTextureView();

	bindGroupEntries3[3].binding = 3u;
	bindGroupEntries3[3].textureView = m_wgpTextureIrradiance.getTextureView();

	bindGroupEntries3[4].binding = 4u;
	bindGroupEntries3[4].textureView = m_wgpTexturePrefilter.getTextureView();

	bindGroupEntries3[5].binding = 5u;
	bindGroupEntries3[5].textureView = m_wgpTextureShadow.getTextureView();

	bindGroupEntries3[6].binding = 6u;
	bindGroupEntries3[6].sampler = wgpContext.getSampler(SS_2);

	WGPUBindGroupDescriptor bindGroupDesc3 = {};
	bindGroupDesc3.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PBR_HELMET"), 3u);
	bindGroupDesc3.entryCount = (uint32_t)bindGroupEntries3.size();
	bindGroupDesc3.entries = bindGroupEntries3.data();
	bindGroups[3] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc3);

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
	bindGroupEntries[2].textureView = m_wgpTextureHDR.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_CUBE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
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
	bindGroupEntries[2].textureView = m_wgpTextureCube.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_IRRADIANCE"), 0u);
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
	bindGroupEntries[2].textureView = m_wgpTextureCube.getTextureView();

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
	bindGroupEntries[2].textureView = m_wgpTextureIrradiance.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_SKYBOX"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroup> ImageBasedLighting::OnBindGroupsSkyboxHelmet() {
	std::vector<WGPUBindGroup> bindGroups(1);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpTextureCube.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_SKYBOX"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

void ImageBasedLighting::OnDrawIrradiance(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip) {
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformMVPBuffer.getBuffer(), 0u, &m_mvpInvCube[layer], 64u);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_IRRADIANCE"));
	m_wgpCube.draw(renderPassEncoder);
}

void ImageBasedLighting::OnDrawCube(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip) {
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformMVPBuffer.getBuffer(), 0u, &m_mvpCube[layer], 64u);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_CUBE"));
	m_wgpCube.draw(renderPassEncoder);
}

void ImageBasedLighting::OnDrawPrefilter(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip) {
	float roughness_val = (float)mip / (float)(ROUGHNESS_LEVELS - 1);
	wgpuQueueWriteBuffer(wgpContext.queue, m_roughnessBuffer.getBuffer(), 0, &roughness_val, sizeof(float));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformMVPBuffer.getBuffer(), 0u, &m_mvpInvCube[layer], 64u);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PREFILTER"));
	m_wgpCube.draw(renderPassEncoder);
}

void ImageBasedLighting::OnDrawBrdf(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip) {
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_BRDF"));
	m_wgpQuad.draw(renderPassEncoder);
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

void ImageBasedLighting::initIrradianceMatrices() {
	m_mvpInvCube[0] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_mvpInvCube[1] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_mvpInvCube[2] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_mvpInvCube[3] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));

	m_mvpInvCube[4] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_mvpInvCube[5] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::InvLookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_mvpCube[0] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(1.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f));
	m_mvpCube[1] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(-1.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f));

	m_mvpCube[2] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, -1.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f));
	m_mvpCube[3] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f));

	m_mvpCube[4] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, 1.0f), Vector3f(0.0f, -1.0f, 0.0f));
	m_mvpCube[5] = Matrix4f::Perspective(90.0f, 1.0, 0.1f, 10.0f) * Matrix4f::LookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, -1.0f, 0.0f));
}