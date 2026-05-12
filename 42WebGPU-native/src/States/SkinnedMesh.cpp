#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <engine/utils/BinaryIO.h>

#include "SkinnedMesh.h"
#include "Application.h"
#include "Globals.h"

SkinnedMesh::SkinnedMesh(StateMachine& machine) : State(machine, States::SKINNED_MESH) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, -50.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setRotationSpeed(0.1f);
	m_camera.setMovingSpeed(50.0f);

	Utils::MdlcIO mdlcIO;
	m_attack.loadAnimationAssimp("res/models/whale/whale.glb", "ATTACK", "attack");
	m_swim.loadAnimationAssimp("res/models/whale/whale.glb", "swim", "swim");

	//mdlcIO.animationToAnic("res/models/whale/swim.anic", m_swim.getAnimationName(), m_swim.getLength(), m_swim.getAnimationTracks());
	//mdlcIO.animationToAnic("res/models/whale/attack.anic", m_attack.getAnimationName(), m_attack.getLength(), m_attack.getAnimationTracks());

	m_whale.loadModelAssimp("res/models/whale/whale.glb", 1u);
	m_whale.scale(10.0f, 10.0f, 10.0f);
	m_whale.rotate(-90.0f, 0.0f, 0.0f);
	m_whale.rotate(0.0f, 0.0f, 180.0f);
	m_whale.translate(0.0f, -5.0f, 0.0f);
	m_whale.addAnimationState(m_attack);
	m_whale.getAnimationState(0)->setLooped(true);
	
	//const AnimatedMesh* mesh = static_cast<const AnimatedMesh*>(m_whale.getMesh());
	//mdlcIO.animatedModelToMdlc("res/whale.mdlc", mesh->getVertexBuffer(), mesh->getIndexBuffer(), mesh->getStride(), mesh->getWeights(), mesh->getJoints(), mesh->getBoneDescriptions());

	m_dance.loadAnimationAssimp("res/models/vampire/dancing_vampire.dae", "Hips", "vampire_dance");
	m_dance.setPositionOfTrack("Hips", 0.0f, 0.0f, 0.0f);
	m_dance.scaleTrack("Hips", 0.1f, 0.1f, 0.1f);

	m_vampire.loadModelAssimp("res/models/vampire/dancing_vampire.dae", 1u);
	m_vampire.rotate(0.0f, 180.0f, 0.0f);
	m_vampire.translate(0.0f, 0.0f, -25.0f);
	m_vampire.addAnimationState(m_dance);
	m_vampire.getAnimationState(0)->setLooped(true);

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_skinBuffer.createBuffer(sizeof(Matrix4f) * 96u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);

	wgpContext.setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });
	wgpContext.addSahderModule("ANIMATION", "res/shader/animation.wgsl");
	wgpContext.createRenderPipeline("ANIMATION", "RP_ANIMATION", VL_PTNWJ, std::bind(&SkinnedMesh::OnBindGroupLayouts, this));

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

	m_wgpVampire.create(m_vampire);
	m_wgpVampire.setBindGroups("BG", std::bind(&SkinnedMesh::OnBindGroups, this));

	m_wgpWhale.create(m_whale);
	m_wgpWhale.setBindGroups("BG", std::bind(&SkinnedMesh::OnBindGroups, this));

	wgpContext.OnDraw = std::bind(&SkinnedMesh::OnDraw, this, std::placeholders::_1);
}

SkinnedMesh::~SkinnedMesh() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
	m_skinBuffer.markForDelete();
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
		m_whale.update(m_dt);
		m_whale.updateSkinning();
		mesh = static_cast<const AnimatedMesh*>(m_whale.getMesh());
	}else {
		m_vampire.update(m_dt);
		m_vampire.updateSkinning();
		mesh = static_cast<const AnimatedMesh*>(m_vampire.getMesh());
	}

	wgpuQueueWriteBuffer(wgpContext.queue, m_skinBuffer.getBuffer(), 0u, mesh->getSkinMatrices(), mesh->getNumBones() * sizeof(Matrix4f));
}

void SkinnedMesh::render() {
	wgpDraw();
}

void SkinnedMesh::OnDraw(const WGPURenderPassEncoder& renderPassEncoder) {

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, projection), &m_uniforms.projection, sizeof(Uniforms::projection));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, view), &m_uniforms.view, sizeof(Uniforms::view));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, env), &m_uniforms.env, sizeof(Uniforms::env));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, model), &m_uniforms.model, sizeof(Uniforms::model));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, normal), &m_uniforms.normal, sizeof(Uniforms::normal));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, camPosition), &m_uniforms.camPosition, sizeof(Uniforms::camPosition));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, lightVP), &m_uniforms.lightVP, sizeof(Uniforms::lightVP));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), offsetof(Uniforms, shadow), &m_uniforms.shadow, sizeof(Uniforms::shadow));

	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_ANIMATION"));
	m_model == SelectedModel::WHALE ? m_wgpWhale.draw(renderPassEncoder) : m_wgpVampire.draw(renderPassEncoder);
	
	if (m_drawUi)
		renderUi(renderPassEncoder);
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
	int currentModel = m_model;
	if (ImGui::Combo("Model", &currentModel, "Vampire\0Whale\0\0")) {
		m_model = static_cast<SelectedModel>(currentModel);
	}

	if (m_model == SelectedModel::WHALE) {
		int currentAnimation = m_animation;
		if (ImGui::Combo("Animation", &currentAnimation, "Attack\0Swim\0\0")) {
			m_animation = static_cast<SelectedAnimation>(currentAnimation);
			if (m_animation == SelectedAnimation::ATTACK) {
				m_whale.removeAnimationState(m_swim);
				m_whale.addAnimationState(m_attack);
				m_whale.getAnimationState(0)->setLooped(true);
			}else {
				m_whale.removeAnimationState(m_attack);
				m_whale.addAnimationState(m_swim);
				m_whale.getAnimationState(0)->setLooped(true);
			}
		}
	}

	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> SkinnedMesh::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 16 * sizeof(float);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> SkinnedMesh::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_skinBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_skinBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_ANIMATION"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}