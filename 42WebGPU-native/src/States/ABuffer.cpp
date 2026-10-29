#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <engine/input/GamePad.h>
#include <WebGPU/WgpRenderer.h>
#include <States/Menu.h>

#include "Globals.h"
#include "Application.h"
#include "ABuffer.h"

const uint32_t average_layers_per_fragment = 4u;
uint32_t maxStorageBufferBindingSize = 0u;

ABuffer::ABuffer(StateMachine& machine) : State(machine, States::ABUFFER) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24PlusStencil8);

	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f , -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 10.0f, 250.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(100.0f);
	m_camera.setRotationSpeed(0.1f);

	WGPULimits supportedLimits = {};
	wgpuDeviceGetLimits(wgpContext.device, &supportedLimits);
	
	const uint32_t canvas_width = Application::Width;
	const uint32_t canvas_height = Application::Height;
	const uint32_t linked_list_element_size = 5 * sizeof(float) + 1 * sizeof(uint32_t);

	maxStorageBufferBindingSize = supportedLimits.maxStorageBufferBindingSize;
	const uint32_t bytes_per_line = canvas_width * average_layers_per_fragment * linked_list_element_size;
	const uint32_t max_lines_supported = (uint32_t)floorf(maxStorageBufferBindingSize / (float)bytes_per_line);
	const uint32_t num_slices = (uint32_t)ceilf(canvas_height / (float)max_lines_supported);
	const uint32_t slice_height= (uint32_t)ceilf(canvas_height / (float)num_slices);
	const uint32_t linked_list_buffer_size= slice_height * bytes_per_line;
	
	uint64_t totalBufferSize = num_slices * supportedLimits.minUniformBufferOffsetAlignment;	
	m_sliceInfoBuffer.createBuffer(sizeof(SliceInfo) * supportedLimits.minUniformBufferOffsetAlignment, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform, true);
	uint8_t* byteData = (uint8_t*)wgpuBufferGetMappedRange(m_sliceInfoBuffer.getBuffer(), 0, totalBufferSize);
	const int32_t byteStride = supportedLimits.minUniformBufferOffsetAlignment;
	for (int32_t i = 0; i < (int32_t)num_slices; ++i) {
		SliceInfo* currentSlice = (SliceInfo*)(byteData + (i * byteStride));
		currentSlice->targetWidth = Application::Height;
		currentSlice->maxStorableFragments = average_layers_per_fragment * Application::Width * slice_height;
		currentSlice->sliceStartY = i * slice_height;
		currentSlice->padding = 0;
	}
	wgpuBufferUnmap(m_sliceInfoBuffer.getBuffer());

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_uniforms.projection = m_camera.getPerspectiveMatrix();
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

	wgpContext.addSahderModule("COLOR_PN", "res/shader/color_PN.wgsl");
	wgpContext.createRenderPipeline("COLOR_PN", "RP_COLOR_PN", VL_PN, std::bind(&ABuffer::OnBindGroupLayouts, this));

	wgpContext.addSahderModule("ABUFFER_OPAQUE", "res/shader/abuffer_opaque.wgsl");
	wgpContext.createRenderPipeline("ABUFFER_OPAQUE", "RP_ABUFFER_OPAQUE", VL_PN, std::bind(&ABuffer::OnBindGroupLayouts, this));

	m_trackball.reshape(Application::Width, Application::Height);
	m_teapot.loadModel("res/models/teapot.obj", false, false, true);
	m_wgpTeapot.create(m_teapot);
	m_wgpTeapot.setBindGroups("BG", std::bind(&ABuffer::OnBindGroups, this));

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	wgpContext.OnDraw = std::bind(&ABuffer::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	wgpContext.OnPostDraw = std::bind(&ABuffer::OnPostDraw, this);	
}

ABuffer::~ABuffer() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
}

void ABuffer::fixedUpdate() {

}

void ABuffer::update() {
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
	m_trackball.idle();

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = m_trackball.getTransform();
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_camera.getViewMatrix() * m_uniforms.model);
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::BIAS * m_uniforms.lightVP;
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));	
}

void ABuffer::render() {
	wgpDraw();
}

void ABuffer::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	
	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_ABUFFER_OPAQUE"));

	m_wgpTeapot.draw(renderPassEncoder, 8u);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
}

void ABuffer::OnPostDraw() {
	
}

void ABuffer::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void ABuffer::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void ABuffer::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void ABuffer::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void ABuffer::OnKeyDown(const Event::KeyboardEvent& event) {
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

void ABuffer::OnKeyUp(const Event::KeyboardEvent& event) {

}

void ABuffer::resize(int deltaW, int deltaH) {
	m_camera.perspective(45.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
	m_wantResize = true;
}

void ABuffer::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> ABuffer::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Uniforms);

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> ABuffer::OnBindGroups() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindings(1);

	bindings[0].binding = 0u;
	bindings[0].buffer = m_uniformBuffer.getBuffer();
	bindings[0].offset = 0u;
	bindings[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_COLOR_PN"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
	return bindGroups;
}