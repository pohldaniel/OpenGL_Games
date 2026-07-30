#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <Nuklear/NkContext.h>
#include <Nuklear/NkStyle.h>

#include "Isometric.h"
#include "Application.h"
#include "Globals.h"

//For getting this matrices load the model with 
// importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true); 
// and call printAiHierarchy(pScene->mRootNode);.
// The used ones are Gun_$AssimpFbx$_RotationOffset, Gun_$AssimpFbx$_RotationPivot and Gun_$AssimpFbx$_RotationPivotInverse
Matrix4f offset = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	-156.85f, -32.2427f, 144.702f, 1.0f);

Matrix4f pivot = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	130.762f, 70.4033f, -3.52485f, 1.0f);

Matrix4f invPivot = Matrix4f(1.0f, 0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f, 0.0f,
	-130.762f, -70.4033f, 3.52485f, 1.0f);

Isometric::Isometric(StateMachine& machine) : State(machine, States::ISOMETRIC), m_animationController(&m_player) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	nkInit(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	nkInitFont("res/fonts/upheavtt.ttf");

	AnimationManager::Get().getAnimation("full").loadAnimationAssimp("res/models/Player.fbx", "Player", "full", 0u, 245u);
	AnimationManager::Get().getAnimation("idle").loadAnimationAssimp("res/models/Player.fbx", "Player", "idle", 5u, 81u);
	AnimationManager::Get().getAnimation("forward").loadAnimationAssimp("res/models/Player.fbx", "Player", "forward", 85u, 105u);
	AnimationManager::Get().getAnimation("backward").loadAnimationAssimp("res/models/Player.fbx", "Player", "backward", 110u, 130u);
	AnimationManager::Get().getAnimation("backward").shift(10u);
	AnimationManager::Get().getAnimation("right").loadAnimationAssimp("res/models/Player.fbx", "Player", "right", 135u, 155u);
	AnimationManager::Get().getAnimation("right").shift(10u);

	AnimationManager::Get().getAnimation("right2").loadAnimationAssimp("res/models/Player.fbx", "Player", "right", 135u, 155u);
	AnimationManager::Get().getAnimation("left").loadAnimationAssimp("res/models/Player.fbx", "Player", "left", 160u, 180u);
	

	AnimationManager::Get().getAnimation("death").loadAnimationAssimp("res/models/Player.fbx", "Player", "death", 185u, 244u);
	AnimationManager::Get().getAnimation("death").shift(50u);
	AnimationManager::Get().getAnimation("death2").loadAnimationAssimp("res/models/Player.fbx", "Player", "right", 185u, 244u);

	m_player.loadModelAssimp("res/models/Player.fbx", 1u);

	//Add additional nodes to the first Mesh, they are presented inside the Animation channels but not at the bone hierarchy from the model.
	AnimatedMesh* mesh = static_cast<AnimatedMesh*>(m_player.mesh());
	mesh->boneDescriptions().emplace_back();
	mesh->boneDescriptions().back().name = "Gun_$AssimpFbx$_Rotation";
	mesh->boneDescriptions().back().parentIndex = -1;
	mesh->boneDescriptions().back().offsetMatrix = invPivot;

	mesh->boneDescriptions().emplace_back();
	mesh->boneDescriptions().back().name = "Gun_$AssimpFbx$_Translation";
	mesh->boneDescriptions().back().parentIndex = 0;
	mesh->boneDescriptions().back().offsetMatrix = offset * pivot;

	mesh->createBones();

	mesh = static_cast<AnimatedMesh*>(m_player.mesh(1u));
	for (size_t index = 0u; index < mesh->getVertexBuffer().size() / mesh->getStride(); index++) {
		mesh->weights().push_back({ 1.0f, 0.0f, 0.0f, 0.0f });
		mesh->joints().push_back({ 42u, 0u, 0u, 0u });
	}

	m_player.scale(0.0044f, 0.0044f, 0.0044f);
	m_player.rotate(0.0f, 0.0f, 0.0f);
	m_player.translate(0.0f, 0.0f, 0.0f);
	m_player.applyBindpose(true);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f,  -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(-4.0f, 4.3f, 0.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(50.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);

	m_floor.buildQuadXZ({-50.0f, 0.0f, -50.0f}, {100.0f, 100.0f});

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_skinBuffer.createBuffer(sizeof(Matrix4f) * 96u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.color = { 1.0f, 1.0f, 1.0f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	m_uniforms.lightPosition = Vector3f(50.0f, 100.0f, -100.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));

	wgpContext.addSahderModule("ANIMATION", "res/shader/animation_fbx.wgsl");
	wgpContext.createRenderPipeline("ANIMATION", "RP_ANIMATION", VL_PTNWJ, std::bind(&Isometric::OnBindGroupLayouts, this));

	wgpContext.addSahderModule("TEXTURE", "res/shader/texture.wgsl");
	wgpContext.createRenderPipeline("TEXTURE", "RP_TEXTURE", VL_PTN, std::bind(&Isometric::OnBindGroupLayoutsTexture, this));

	m_wgpPlayer.create(m_player);
	m_wgpPlayer.setBindGroups("BG", std::bind(&Isometric::OnBindGroups, this));

	wgpContext.setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	wgpContext.OnDraw = std::bind(&Isometric::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	nkContext.OnFillBuffer = std::bind(&Isometric::OnFillBuffer, this, std::placeholders::_1);
	

	m_wgpFloorD.loadFromFile("res/textures/floor/Floor_D.psd");

	m_wgpFloor.create(m_floor);
	m_wgpFloor.setBindGroups("BG", std::bind(&Isometric::OnBindGroupsTexture, this));

	/*m_player.addAnimationState(AnimationManager::Get().getAnimation("forward"));
	m_player.getAnimationState(0u)->setLooped(true);
	m_player.getAnimationState(0u)->setWeight(0.25f);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("right"));
	m_player.getAnimationState(1u)->setLooped(true);
	m_player.getAnimationState(1u)->setWeight(0.25f);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("backward"));
	m_player.getAnimationState(2u)->setLooped(true);
	m_player.getAnimationState(2u)->setWeight(0.25f);

	m_player.addAnimationState(AnimationManager::Get().getAnimation("left"));
	m_player.getAnimationState(3u)->setLooped(true);
	m_player.getAnimationState(3u)->setWeight(0.25f);*/
}

Isometric::~Isometric() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	nkShutDown();
	m_uniformBuffer.markForDelete();
	m_skinBuffer.markForDelete();
}

void Isometric::fixedUpdate() {

}

void Isometric::update() {

	Keyboard& keyboard = Keyboard::instance();
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

	nkUpdateInput(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT), mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT), Application::ScrollDelta);
	
	const AnimatedMesh* mesh_ = static_cast<const AnimatedMesh*>(m_player.getMesh());
	const Vector3f posistion = mesh_->getBone(0u).getPosition();
	m_camera.lookAt(posistion + Vector3f(-4.0f, 4.3f, 0.0f), posistion, Vector3f(0.0f, 1.0f, 0.0f));
	if ((mouse.xDelta() || mouse.yDelta()) && !m_isDeath) {
		Vector3f coords;	
		if (getWorldPosition(mouse.xPos(), mouse.yPos(), Vector3f(0.0f, 1.0f, 0.0f), coords)) {
			float degrees = (!m_nuclearWidgetResult.isActive && !m_joystickResult.isActive) && mouse.buttonDown(Mouse::BUTTON_LEFT) ? getLookAtYRotation(posistion, coords) : m_nuclearWidgetResult.angle;
			m_nuclearWidgetResult.angle = degrees;
			if(degrees)
				m_player.setRotation(0.0f, degrees, 0.0f);
		}
	}

	float moveX = 0.0f;
	float moveY = 0.0f;

	float magnitude = m_joystickResult.x * m_joystickResult.x + m_joystickResult.y * m_joystickResult.y;
	float deadzone = 0.25f;

	if (magnitude > deadzone * deadzone) {
		if (fabsf(m_joystickResult.x) > fabsf(m_joystickResult.y)) {
			moveX = (m_joystickResult.x > 0.0f) ? 1.0f : -1.0f;
			moveY = 0.0f;
		}else {
			moveX = 0.0f;
			moveY = (m_joystickResult.y > 0.0f) ? 1.0f : -1.0f;
		}
	}else {
		moveX = 0.0f;
		moveY = 0.0f;
	}

	if (keyboard.keyDown(Keyboard::KEY_UP) || moveY > 0.0f) {
		m_animationController.fadeAndPlay("forward", 0.25f);
		playerMove |= true;
		//m_player.translate(2.0f * m_dt, 0.0f, 0.0f);
		m_player.translateRelative(0.0f, 0.0f, 2.0f * m_dt);
	}

	if (keyboard.keyDown(Keyboard::KEY_DOWN) || moveY < 0.0f) {
		m_animationController.fadeAndPlay("backward", 0.25f);
		playerMove |= true;
		//m_player.translate(-2.0f * m_dt, 0.0f, 0.0f);
		m_player.translateRelative(0.0f, 0.0f, -2.0f * m_dt);
	}

	if (keyboard.keyDown(Keyboard::KEY_LEFT) || moveX < 0.0f) {
		m_animationController.fadeAndPlay("left", 0.25f);
		playerMove |= true;
		//m_player.translate(0.0f, 0.0f, -2.0f * m_dt);
		m_player.translateRelative(2.0f * m_dt, 0.0f, 0.0f);
	}

	if (keyboard.keyDown(Keyboard::KEY_RIGHT) || moveX > 0.0f) {
		m_animationController.fadeAndPlay("right", 0.25f);
		playerMove |= true;
		//m_player.translate(0.0f, 0.0f, 2.0f * m_dt);
		m_player.translateRelative(-2.0f * m_dt, 0.0f, 0.0f);
	}

	if (keyboard.keyPressed(Keyboard::KEY_T) || m_nuclearWidgetResult.buttonPressed) {
		m_isDeath = true;
	}

	if (!playerMove && !m_isDeath) {
		m_animationController.fadeAndPlay("idle", 0.2f, 0.25f);
	}

	if (m_isDeath) {
		m_animationController.play("death", false, 2.0f);
	}

	m_animationController.update(m_dt);
	m_player.update(m_dt);
	m_player.updateSkinning();

	const AnimatedMesh* mesh = static_cast<const AnimatedMesh*>(m_player.getMesh());
	mesh->skinMatrices()[42] ^= mesh->getBone(43u).getWorldTransformation() * offset * pivot;
	wgpuQueueWriteBuffer(wgpContext.queue, m_skinBuffer.getBuffer(), 0u, mesh->getSkinMatrices(), mesh->getNumBones() * sizeof(Matrix4f));

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::IDENTITY;
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_uniforms, sizeof(Uniforms));
}

void Isometric::render() {
	wgpDraw();
}

void Isometric::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);


		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_TEXTURE"));
		m_wgpFloor.draw(renderPassEncoder);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_ANIMATION"));
		m_wgpPlayer.draw(renderPassEncoder);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}

	{
		WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;

		nkDraw(commandEncoder, rndrPssDscrptor);
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

void Isometric::OnFillBuffer(nk_context& nkCntxt) {
	set_transparent_window_style();
	virtual_joystick(nk_rect(20.0f, static_cast<float>(Application::Height) - 200.0f, 180.0f, 180.0f), m_joystickResult);
	//action_button(nk_rect(static_cast<float>(Application::Width) - 180.0f, static_cast<float>(Application::Height) - 180.0f, 140.0f, 140.0f), m_isPressed);
	//virtual_rotation(nk_rect(static_cast<float>(Application::Width) - 180.0f, static_cast<float>(Application::Height) - 180.0f, 140.0f, 140.0f), m_rotationResult);
	virtual_rotation_button(nk_rect(static_cast<float>(Application::Width) - 180.0f, static_cast<float>(Application::Height) - 180.0f, 140.0f, 140.0f), m_nuclearWidgetResult);
	reset_transparent_window_style();
}

void Isometric::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void Isometric::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void Isometric::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void Isometric::OnMouseWheel(const Event::MouseWheelEvent& event) {
	
}

void Isometric::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void Isometric::OnKeyUp(const Event::KeyboardEvent& event) {

}

void Isometric::resize(int deltaW, int deltaH) {
	nkResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);	
}

void Isometric::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	if (ImGui::SliderFloat("Weight Right", &m_weightRight, 0.0f, 1.0f)) {
		m_player.getAnimationState(0u)->setWeight(m_weightRight);
	}

	if (ImGui::SliderFloat("Weight Left", &m_weightLeft, 0.0f, 1.0f)) {
		m_player.getAnimationState(1u)->setWeight(m_weightLeft);
	}
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayouts() {
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

std::vector<WGPUBindGroupLayout> Isometric::OnBindGroupLayoutsTexture() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType::WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> Isometric::OnBindGroups() {
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

std::vector<WGPUBindGroup> Isometric::OnBindGroupsTexture() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_REPEAT);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_wgpFloorD.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_TEXTURE"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

bool Isometric::getWorldPosition(int xPos, int yPos, const Vector3f& planeNormal, Vector3f& outIntersection) {
	float mouseXndc = (2.0f * xPos) / static_cast<float>(Application::Width) - 1.0f;
	float mouseYndc = 1.0f - (2.0f * yPos) / static_cast<float>(Application::Height);

	float tanfov = m_camera.getInvPerspectiveMatrixNew()[1][1];
	float aspect = (static_cast<float>(Application::Width) / static_cast<float>(Application::Height));

	Vector3f rayStartWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getNear();
	Vector3f rayEndWorld = m_camera.getPosition() + (m_camera.getCamX() * mouseXndc * tanfov * aspect + m_camera.getCamY() * mouseYndc * tanfov + m_camera.getViewDirection()) * m_camera.getFar();
	Vector3f direction = Vector3f::Normalize(rayEndWorld - rayStartWorld);
	float denom = Vector3f::Dot(direction, planeNormal);

	if (std::abs(denom) > 1e-6f) {
		float t = Vector3f::Dot(-rayStartWorld, planeNormal) / denom;
		if (t >= 0.0f) { 
			outIntersection = rayStartWorld + direction * t;
			return true;
		}
	}
	return false;
}

float Isometric::getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos) {
	float dx = targetPos[0] - objectPos[0];
	float dz = targetPos[2] - objectPos[2];

	if (abs(dx) < 0.01f && abs(dz) < 0.01f)
		return 0.0f;

	return std::atan2(dx, dz) * _180_ON_PI;
}