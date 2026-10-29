#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <engine/input/GamePad.h>
#include <States/Menu.h>

#include "Globals.h"
#include "Application.h"
#include "Controller.h"

Controller::Controller(StateMachine& machine) : State(machine, States::CONTROLLER) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f , -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(10.0f);
	m_camera.setRotationSpeed(0.1f);

	m_storageBuffer.createBuffer(20u * sizeof(float) * 200u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Storage);
	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniforms.projection = m_camera.getOrthographicMatrix();
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

	wgpContext.addSahderModule("COLOR_PT", "res/shader/controller.wgsl");
	wgpContext.createRenderPipeline("COLOR_PT", "RP_COLOR_PT", VL_PT, std::bind(&Controller::OnBindGroupLayouts, this));

	m_disk.buildSegmentXY({ 0.0f, 0.0f, 0.0f }, 20.0f, 0.0f, 360.0f, 1u, 30u, true, false);
	m_disk.rewind();

	m_quad.buildQuadXY({ -100.0f, -100.0f, 0.0f }, { 200.0f, 200.0f }, 1u, 1u, true, false);
	m_quad.rewind();

	m_wgpDisk.create(m_disk);
	m_wgpDisk.setBindGroups("BG", std::bind(&Controller::OnBindGroups, this));

	m_wgpQuad.create(m_quad);
	m_wgpQuad.setBindGroups("BG", std::bind(&Controller::OnBindGroups, this));

	wgpContext.setClearColor({ 0.494f, 0.686f, 0.796f, 1.0f });
	wgpContext.OnDraw = std::bind(&Controller::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

Controller::~Controller() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
	m_storageBuffer.markForDelete();
}

void Controller::fixedUpdate() {

}

void Controller::update() {
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

	IGamePad* gamePad = XInputGamePad::GetPlayer() ? XInputGamePad::GetPlayer() : DInputGamePad::GetPlayer();
	m_RightStickX = 0.0f;
	m_RightStickY = 0.0f;
	m_LeftStickX = 0.0f;
	m_LeftStickY = 0.0f;
	
	m_state = 0;
	if (gamePad) {

		m_RightStickX = gamePad->getRightStickX();
		m_RightStickY = gamePad->getRightStickY();

		m_LeftStickX = gamePad->getLeftStickX();
		m_LeftStickY = gamePad->getLeftStickY();

		if (gamePad->buttonDown(GamepadButton::DPAD_UP)) {
			m_state = 1;
		}

		if (gamePad->buttonDown(GamepadButton::DPAD_RIGHT)) {
			m_state = 3;
		}

		if (gamePad->buttonDown(GamepadButton::DPAD_DOWN)) {
			m_state = 5;
		}

		if (gamePad->buttonDown(GamepadButton::DPAD_LEFT)) {
			m_state = 7;
		}

		if (gamePad->buttonDown(GamepadButton::DPAD_UP_RIGHT)) {
			m_state = 2;
		}

		if (gamePad->buttonDown(GamepadButton::DPAD_DOWN_RIGHT)) {
			m_state = 4;
		}

		if (gamePad->buttonDown(GamepadButton::DPAD_DOWN_LEFT)) {
			m_state = 6;
		}

		if (gamePad->buttonDown(GamepadButton::DPAD_UP_LEFT)) {
			m_state = 8;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_START)) {
			m_state = 9;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_BACK)) {
			m_state = 10;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_LEFT_THUMB)) {
			m_state = 11;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_RIGHT_THUMB)) {
			m_state = 12;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_LEFT_SHOULDER)) {
			m_state = 13;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_RIGHT_SHOULDER)) {
			m_state = 14;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_A)) {
			m_state = 15;

			if (gamePad->buttonDown(GamepadButton::GAMEPAD_A))
				gamePad->setVibration(1.0f, 1.0f);
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_B)) {
			m_state = 16;

			if (gamePad->buttonDown(GamepadButton::GAMEPAD_B))
				gamePad->setVibration(0.0f, 0.0f);
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_X)) {
			m_state = 17;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_Y)) {
			m_state = 18;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_LEFT_TRIGGER)) {
			m_state = 19;
		}

		if (gamePad->buttonDown(GamepadButton::GAMEPAD_RIGHT_TRIGGER)) {
			m_state = 20;
		}
	}

	m_uniforms.projection = m_camera.getOrthographicMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = Matrix4f::Translate(static_cast<float>(Application::Width) * 0.25f, static_cast<float>(Application::Height) * 0.25f, 0.0f);
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));

	m_instances.clear();

	renderRectangle(static_cast<float>(Application::Width) * 0.5f + 300.0f, static_cast<float>(Application::Height) * 0.5f);
	renderRectangle(static_cast<float>(Application::Width) * 0.5f - 300.0f, static_cast<float>(Application::Height) * 0.5f);

	renderPad(static_cast<float>(Application::Width) * 0.5f - 80.0f, static_cast<float>(Application::Height) * 0.5f + 80.0f, m_state);
	for (int i = 0; i < 12; i++) {
		renderButton(i * 50.0f + 50.0f, 50.0f, m_state, i + 9);
	}
	
	renderStick(static_cast<float>(Application::Width) * 0.5f - 300.0f, static_cast<float>(Application::Height) * 0.5f, m_RightStickX, m_RightStickY);
	renderStick(static_cast<float>(Application::Width) * 0.5f + 300.0f, static_cast<float>(Application::Height) * 0.5f, m_LeftStickX , m_LeftStickY);
	wgpuQueueWriteBuffer(wgpContext.queue, m_storageBuffer.getBuffer(), 0u, m_instances.data(), m_instances.size() * sizeof(float) * 20u);
}

void Controller::render() {
	wgpDraw();
}

void Controller::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	 {
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_COLOR_PT"));
		
		m_wgpQuad.draw(renderPassEncoder, 4u, 0u);
		m_wgpDisk.draw(renderPassEncoder, m_instances.size() - 4u, 4u);
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}

	if (m_drawUi) {
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

void Controller::OnKeyDown(const Event::KeyboardEvent& event) {
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

void Controller::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
}

void Controller::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> Controller::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries[1].buffer.minBindingSize = 20u * sizeof(float);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> Controller::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Uniforms);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_storageBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = wgpuBufferGetSize(m_storageBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_COLOR_PT"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();
	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

void Controller::renderRectangle(float x, float y) {
	std::array<float, 20> instance;
	instance[0] = 1.03f; instance[1] = 0.0f; instance[2] = 0.0f; instance[3] = 0.0f;
	instance[4] = 0.0f; instance[5] = 1.03f; instance[6] = 0.0f; instance[7] = 0.0f;
	instance[8] = 0.0f; instance[9] = 0.0f; instance[10] = 1.03f; instance[11] = 0.0f;
	instance[12] = x; instance[13] = y; instance[14] = 0.0f; instance[15] = 1.0f;
	instance[16] = 0.0f; instance[17] = 0.0f; instance[18] = 0.0f; instance[19] = 1.0f;
	m_instances.push_back(instance);

	instance[0] = 1.0f; instance[1] = 0.0f; instance[2] = 0.0f; instance[3] = 0.0f;
	instance[4] = 0.0f; instance[5] = 1.0f; instance[6] = 0.0f; instance[7] = 0.0f;
	instance[8] = 0.0f; instance[9] = 0.0f; instance[10] = 1.0f; instance[11] = 0.0f;
	instance[12] = x; instance[13] = y; instance[14] = 0.0f; instance[15] = 1.0f;
	instance[16] = 1.0f; instance[17] = 1.0f; instance[18] = 1.0f; instance[19] = 1.0f;
	m_instances.push_back(instance);
}

void Controller::renderPad(float x, float y, long value) {
	for (int i = 1; i < 9; i++) {
		float xPos = (sinf(M_PI_4 * i - M_PI_4) * 80.0f) + 80.0f;
		float yPos = (cosf(M_PI_4 * i - M_PI_4) * 80.0f) + 80.0f;

		std::array<float, 20> instance;
		instance[0] = 1.15f; instance[1] = 0.0f; instance[2] = 0.0f; instance[3] = 0.0f;
		instance[4] = 0.0f; instance[5] = 1.15f; instance[6] = 0.0f; instance[7] = 0.0f;
		instance[8] = 0.0f; instance[9] = 0.0f; instance[10] = 1.15f; instance[11] = 0.0f;
		instance[12] = x + xPos; instance[13] = y - yPos; instance[14] = 0.0f; instance[15] = 1.0f;
		instance[16] = 0.0f; instance[17] = 0.0f; instance[18] = 0.0f; instance[19] = 1.0f;
		m_instances.push_back(instance);

		instance[0] = 1.0f; instance[1] = 0.0f; instance[2] = 0.0f; instance[3] = 0.0f;
		instance[4] = 0.0f; instance[5] = 1.0f; instance[6] = 0.0f; instance[7] = 0.0f;
		instance[8] = 0.0f; instance[9] = 0.0f; instance[10] = 1.0f; instance[11] = 0.0f;
		instance[12] = x + xPos; instance[13] = y - yPos; instance[14] = 0.0f; instance[15] = 1.0f;
		instance[16] = 1.0f; instance[17] = value == i ? 0.0f : 1.0f; instance[18] = value == i ? 0.0f : 1.0f; instance[19] = 1.0f;
		m_instances.push_back(instance);
	}
}

void Controller::renderButton(float x, float y, long value, int index) {
	std::array<float, 20> instance;

	instance[0] = 1.15f; instance[1] = 0.0f; instance[2] = 0.0f; instance[3] = 0.0f;
	instance[4] = 0.0f; instance[5] = 1.15f; instance[6] = 0.0f; instance[7] = 0.0f;
	instance[8] = 0.0f; instance[9] = 0.0f; instance[10] = 1.15f; instance[11] = 0.0f;
	instance[12] = x; instance[13] = y; instance[14] = 0.0f; instance[15] = 1.0f;
	instance[16] = 0.0f; instance[17] = 0.0f; instance[18] = 0.0f; instance[19] = 1.0f;
	m_instances.push_back(instance);

	instance[0] = 1.0f; instance[1] = 0.0f; instance[2] = 0.0f; instance[3] = 0.0f;
	instance[4] = 0.0f; instance[5] = 1.0f; instance[6] = 0.0f; instance[7] = 0.0f;
	instance[8] = 0.0f; instance[9] = 0.0f; instance[10] = 1.0f; instance[11] = 0.0f;
	instance[12] = x; instance[13] = y; instance[14] = 0.0f; instance[15] = 1.0f;
	instance[16] = 1.0f; instance[17] = value == index ? 0.0f : 1.0f; instance[18] = value == index ? 0.0f : 1.0f; instance[19] = 1.0f;
	m_instances.push_back(instance);
}

void Controller::renderStick(float x, float y, float stickX, float stickY) {
	std::array<float, 20> instance;
	instance[0] = 1.0f; instance[1] = 0.0f; instance[2] = 0.0f; instance[3] = 0.0f;
	instance[4] = 0.0f; instance[5] = 1.0f; instance[6] = 0.0f; instance[7] = 0.0f;
	instance[8] = 0.0f; instance[9] = 0.0f; instance[10] = 1.0f; instance[11] = 0.0f;
	instance[12] = x + stickX * 50.0f; instance[13] = y - stickY * 50.0f; instance[14] = 0.0f; instance[15] = 1.0f;
	instance[16] = 1.0f; instance[17] = 0.0f; instance[18] = 0.0f; instance[19] = 1.0f;
	m_instances.push_back(instance);
}

void Controller::OnReEnter(unsigned int prevState) {
	update();
}