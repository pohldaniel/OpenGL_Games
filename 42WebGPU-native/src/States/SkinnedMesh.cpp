#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <engine/utils/BinaryIO.h>
#include <States/Wireframe.h>

#include "SkinnedMesh.h"
#include "Application.h"
#include "Globals.h"

SkinnedMesh::SkinnedMesh(StateMachine& machine) : State(machine, States::SKINNED_MESH), m_fade(m_fadeValue) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, -50.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(50.0f);

	m_attack.loadAnimationAssimp("res/models/whale/whale.glb", "ATTACK", "attack");
	m_swim.loadAnimationAssimp("res/models/whale/whale.glb", "swim", "swim");

	//Utils::MdlcIO mdlcIO;
	//mdlcIO.animationToAnic("res/models/whale/swim.anic", m_swim.getAnimationName(), m_swim.getLength(), m_swim.getAnimationTracks());
	//mdlcIO.animationToAnic("res/models/whale/attack.anic", m_attack.getAnimationName(), m_attack.getLength(), m_attack.getAnimationTracks());

	m_whale.loadModelAssimp("res/models/whale/whale.glb", 1u);
	m_whale.scale(10.0f, 10.0f, 10.0f);
	m_whale.rotate(-90.0f, 0.0f, 0.0f);
	m_whale.rotate(0.0f, 0.0f, 180.0f);
	m_whale.translate(0.0f, -5.0f, 0.0f);
	if (m_animation == SelectedAnimation::PROCEDURAL) {
		m_whale.scale(4.0f, 4.0f, 4.0f);
		m_whale.translate(0.0f, -20.0f, 0.0f);
	}
	m_whale.applyBindpose(true);
	m_whale.addAnimationState(m_attack);
	m_whale.getAnimationState(0)->setLooped(true);

	//const AnimatedMesh* mesh = static_cast<const AnimatedMesh*>(m_whale.getMesh());
	//mdlcIO.animatedModelToMdlc("res/whale.mdlc", mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getStride(), mesh->getWeights(), mesh->getJoints(), mesh->getBoneDescriptions());

	m_dance.loadAnimationAssimp("res/models/vampire/dancing_vampire.dae", "Hips", "vampire_dance");
	m_vampire.loadModelAssimp("res/models/vampire/dancing_vampire.dae", 1u);

	m_vampire.scale(0.1f, 0.1f, 0.1f);
	m_vampire.rotate(0.0f, 180.0f, 0.0f);
	m_vampire.translate(-7.5f, -7.5f, -25.0f);
	m_vampire.applyBindpose(true);
	m_vampire.addAnimationState(m_dance);
	m_vampire.getAnimationState(0)->setLooped(true);

	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, false, false);
	m_cube.rewind();

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_skinBuffer.createBuffer(sizeof(Matrix4f) * 96u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);
	m_modeBuffer.createBuffer(sizeof(unsigned int), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	wgpContext.setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
	wgpContext.addSahderModule("ANIMATION", "res/shader/animation.wgsl");
	wgpContext.createRenderPipeline("ANIMATION", "RP_ANIMATION", VL_PTNWJ, std::bind(&SkinnedMesh::OnBindGroupLayouts, this));

	wgpContext.addSahderModule("SKYBOX", "res/shader/env_cube.wgsl");
	wgpContext.createRenderPipeline("SKYBOX", "RP_SKYBOX", VL_P,
		std::bind(&SkinnedMesh::OnBindGroupLayoutsSkybox, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_LessEqual,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING }
	);

	m_lightProjection = Matrix4f::Orthographic(-80.0f, 80.0f, -80.0f, 80.0f, -200.0f, 300.0f);
	m_lightView = Matrix4f::LookAt(Vector3f(50.0f, 100.0f, -100.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = m_lightProjection * m_lightView;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(50.0f, 100.0f, -100.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	std::string faces[] = { "res/textures/cubemaps/ocean/ocean_cube_px.jpg", "res/textures/cubemaps/ocean/ocean_cube_nx.jpg", "res/textures/cubemaps/ocean/ocean_cube_py.jpg", "res/textures/cubemaps/ocean/ocean_cube_ny.jpg", "res/textures/cubemaps/ocean/ocean_cube_pz.jpg", "res/textures/cubemaps/ocean/ocean_cube_nz.jpg" };
	m_wgpTextureCube.loadCubeFromFiles(faces, true);
	m_wgpVampire.create(m_vampire);
	m_wgpVampire.setBindGroups("BG", std::bind(&SkinnedMesh::OnBindGroups, this));

	m_wgpWhale.create(m_whale);
	m_wgpWhale.setBindGroups("BG", std::bind(&SkinnedMesh::OnBindGroups, this));

	m_wgpCube.create(m_cube);
	m_wgpCube.setBindGroups("BG", std::bind(&SkinnedMesh::OnBindGroupsSkybox, this));

	wgpContext.OnDraw = std::bind(&SkinnedMesh::OnDraw, this, std::placeholders::_1, std::placeholders::_2);

	m_fade.start();
	m_fade.setTransitionSpeed(m_speed * 0.02f);
	m_fade.setOnFadeEnd([&m_whale = m_whale] {
		m_whale.applyBindpose();
	});
}

SkinnedMesh::~SkinnedMesh() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
	m_skinBuffer.markForDelete();
	m_modeBuffer.markForDelete();
	m_wgpTextureCube.markForDelete();
}

void SkinnedMesh::fixedUpdate() {

}

void SkinnedMesh::update() {
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

	if (keyboard.keyPressed(Keyboard::KEY_T)) {
		wgpPipelineLayoutsRelease();
		wgpPipelinesRelease();
		wgpShaderModulesRelease();
		m_isRunning = false;
		m_machine.addStateAtBottom(new Wireframe(m_machine));
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
	m_fade.update(m_dt);

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = m_lightProjection * m_lightView;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	
	const AnimatedMesh* mesh;
	if (m_model == SelectedModel::WHALE) {	
		mesh = static_cast<const AnimatedMesh*>(m_whale.getMesh());
		if(m_skinMode)
			m_animation == SelectedAnimation::PROCEDURAL ? proceduralSkinning(mesh->bones(), mesh->getNumBones(), m_fadeValue) : m_whale.update(m_dt);
		m_whale.updateSkinning();		
	}else {
		if (m_skinMode)
			m_vampire.update(m_dt);
		m_vampire.updateSkinning();
		mesh = static_cast<const AnimatedMesh*>(m_vampire.getMesh());
	}

	wgpuQueueWriteBuffer(wgpContext.queue, m_skinBuffer.getBuffer(), 0u, mesh->getSkinMatrices(), mesh->getNumBones() * sizeof(Matrix4f));
}

void SkinnedMesh::render() {
	wgpDraw();
}

void SkinnedMesh::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projection), &m_uniforms.projection, sizeof(Uniforms::projection));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, view), &m_uniforms.view, sizeof(Uniforms::view));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, env), &m_uniforms.env, sizeof(Uniforms::env));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, model), &m_uniforms.model, sizeof(Uniforms::model));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, normal), &m_uniforms.normal, sizeof(Uniforms::normal));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, camPosition), &m_uniforms.camPosition, sizeof(Uniforms::camPosition));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, lightVP), &m_uniforms.lightVP, sizeof(Uniforms::lightVP));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, shadow), &m_uniforms.shadow, sizeof(Uniforms::shadow));
	wgpuQueueWriteBuffer(wgpContext.queue, m_modeBuffer.getBuffer(), 0u, &m_mode, sizeof(unsigned int));

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_ANIMATION"));
	if (m_model == SelectedModel::WHALE) {
		m_wgpWhale.draw(renderPassEncoder);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_SKYBOX"));
		m_wgpCube.draw(renderPassEncoder);
	}else
		m_wgpVampire.draw(renderPassEncoder);
	
	if (m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
}

void SkinnedMesh::OnMouseMotion(const Event::MouseMoveEvent& event) {

}

void SkinnedMesh::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow());
	}
}

void SkinnedMesh::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().detach();
	}
}

void SkinnedMesh::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void SkinnedMesh::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void SkinnedMesh::OnKeyUp(const Event::KeyboardEvent& event) {

}

void SkinnedMesh::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void SkinnedMesh::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	int currentMode = m_mode;
	if (ImGui::Combo("Mode", &currentMode, "Normal\0Joints\0Weights\0\0")) {
		m_mode = static_cast<SelectedMode>(currentMode);
	}

	if (!m_skinMode) {
		if (ImGui::Button("Skin Mode On")) {
			m_skinMode = !m_skinMode;
		}
	}else {
		if (ImGui::Button("Skin Mode Off")) {
			m_skinMode = !m_skinMode;
			
			m_whale.applyBindpose(true);
			m_whale.getAnimationState(0)->reset();

			m_vampire.applyBindpose(true);
			m_vampire.getAnimationState(0)->reset();
		}
	}

	int currentModel = m_model;
	if (ImGui::Combo("Model", &currentModel, "Vampire\0Whale\0\0")) {
		m_model = static_cast<SelectedModel>(currentModel);
	}

	if (m_model == SelectedModel::WHALE) {
		int currentAnimation = m_animation;
		SelectedAnimation prevAnimation = m_animation;
		if (ImGui::Combo("Animation", &currentAnimation, "Attack\0Swim\0Procedural\0\0")) {
			m_animation = static_cast<SelectedAnimation>(currentAnimation);
			if (m_animation == SelectedAnimation::ATTACK) {
				m_whale.removeAllAnimationStates();
				m_whale.addAnimationState(m_attack);
				m_whale.getAnimationState(0)->setLooped(true);
				if (prevAnimation == SelectedAnimation::PROCEDURAL) {
					m_whale.scale(0.25f, 0.25f, 0.25f);
					m_whale.translate(0.0f, 20.0f, 0.0f);
					m_whale.applyBindpose();
				}
			}else if(m_animation == SelectedAnimation::SWIM) {
				m_whale.removeAllAnimationStates();
				m_whale.addAnimationState(m_swim);
				m_whale.getAnimationState(0)->setLooped(true);
				m_whale.getAnimationState(0)->setLooped(true);
				if (prevAnimation == SelectedAnimation::PROCEDURAL) {
					m_whale.scale(0.25f, 0.25f, 0.25f);
					m_whale.translate(0.0f, 20.0f, 0.0f);
					m_whale.applyBindpose();
				}
			}else {
				m_fadeValue = 0.0f;
				m_whale.scale(4.0f, 4.0f, 4.0f);
				m_whale.translate(0.0f, -20.0f, 0.0f);
				m_whale.applyBindpose();
			}
		}
		if (m_animation == SelectedAnimation::PROCEDURAL) {
			if (ImGui::SliderFloat("Speed", &m_speed, 10.0f, 100.0f)) {
				m_fade.setTransitionSpeed(m_speed * 0.02f);
			}
			ImGui::SliderFloat("Angle", &m_angle, 0.05f, 0.5f);
		}
	}
	

	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> SkinnedMesh::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 16 * sizeof(float);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = sizeof(unsigned int);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> SkinnedMesh::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_skinBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_skinBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_modeBuffer.getBuffer();
	bindGroupEntries[2].offset = 0u;
	bindGroupEntries[2].size = wgpuBufferGetSize(m_modeBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_ANIMATION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

std::vector<WGPUBindGroupLayout> SkinnedMesh::OnBindGroupLayoutsSkybox() {
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

std::vector<WGPUBindGroup> SkinnedMesh::OnBindGroupsSkybox() {
	std::vector<WGPUBindGroup> bindGroups(1);
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpTextureCube.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_SKYBOX"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

void SkinnedMesh::proceduralSkinning(Bone**& bones, unsigned short numBones, float angle) {
	angle = (angle - 0.5f) * 2.0f * m_angle * 0.5f * m_fade.getTransitionSpeed();

	for (size_t i = 0u; i < numBones; ++i) {
		Bone* bone = bones[i];	

		if (i == 3 || i == 4) {
			bone->rotate(0.0f, 0.0f, angle);
		}else if (i == 5 || i == 6) {
			bone->rotate(0.0f, 0.0f, angle);
		}else if(i == 1 || i == 2) {
			bone->rotate(0.0f, i == 1 ? angle : -angle, 0.0f);
		}
	}
}