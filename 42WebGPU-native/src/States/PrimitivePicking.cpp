#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <engine/utils/BinaryIO.h>

#include "PrimitivePicking.h"
#include "Application.h"
#include "Globals.h"

void OnMapBuffer(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPUMapAsyncStatus_Success) {
		std::cout << "Success Buffer" << std::endl;		
		bool* ready = (bool*)userdata1;
		*ready = true;
	}else {
		std::cout << "Buffer message: " << message.data << std::endl;
	}
}

void OnQueueWorkDone(WGPUQueueWorkDoneStatus status, WGPUStringView message, void* userdata1, void* userdata2) {
	if (status == WGPUQueueWorkDoneStatus_Success) {
		std::cout << "Success Queue" << std::endl;
		bool* ready = (bool*)userdata1;
		*ready = true;
	}else {
		std::cout << "Queue message: " << message.data << std::endl;
	}
}

PrimitivePicking::PrimitivePicking(StateMachine& machine) : State(machine, States::PRIMITIVE_PICKING) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 12.0f, 25.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(20.0f);
	m_camera.setRotationSpeed(0.1f);

	//Utils::JsonIO jsonIO;
	//jsonIO.jsonToObj("res/models/teapot.json", "res/models/teapot.obj");
	m_teapot.loadModel("res/models/teapot.obj", false, false, true);
	m_trackball.reshape(Application::Width, Application::Height);

	wgpContext.setClearColor({ 0.0f, 0.0f, 1.0f, 1.0f });

	m_uniformBuffer.createBuffer(sizeof(Uniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_computeBuffer.createBuffer(2 * sizeof(Matrix4f) + 4 * sizeof(float), WGPUBufferUsage_CopyDst | WGPUBufferUsage_CopySrc | WGPUBufferUsage_Uniform| WGPUBufferUsage_Storage);

	m_indexTexture.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_R32Uint);

	WGPURenderPassColorAttachment renderPassColorAttachment = {};
	renderPassColorAttachment.view = m_indexTexture.getTextureView();
	renderPassColorAttachment.resolveTarget = NULL;
	renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachment.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachment.clearValue = { 0.0f, 0.0f, 1.0f, 1.0f };
	renderPassColorAttachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	renderPassColorAttachments.push_back(renderPassColorAttachment);

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

	wgpContext.addSahderModule("PICK_COMPUTE", "res/shader/pick_compute.wgsl");
	wgpContext.createComputePipeline("PICK_COMPUTE", "cs_main", "CP_PICK", std::bind(&PrimitivePicking::OnBindGroupLayoutsCompute, this));
	
	wgpContext.addSahderModule("PICK", "res/shader/pick.wgsl");
	wgpContext.createRenderPipeline("PICK", "RP_PICK", VL_PN, std::bind(&PrimitivePicking::OnBindGroupLayoutsPick, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_R32Uint , WGPUCullMode_None }
	);

	wgpContext.addSahderModule("PICK_DEBUG", "res/shader/pick_debug.wgsl");
	wgpContext.createRenderPipeline("PICK_DEBUG", "RP_PICK_DEBUG", VL_NONE, std::bind(&PrimitivePicking::OnBindGroupLayoutsDebug, this));

	m_computeBindGroup = createComputeBindGroup();
	m_debugBindGroup = createDebugBindGroup();

	m_wgpTeapot.create(m_teapot);
	m_wgpTeapot.setBindGroups("BG", std::bind(&PrimitivePicking::OnBindGroupsPick, this));

	wgpContext.OnDraw = std::bind(&PrimitivePicking::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	//readBuffer();
}

PrimitivePicking::~PrimitivePicking() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_uniformBuffer.markForDelete();
	m_computeBuffer.markForDelete();
}

void PrimitivePicking::fixedUpdate() {

}

void PrimitivePicking::update() {
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

	Vector3f position = m_camera.getPosition();

	Vector3f::RotateY(position, m_dt * 180.0f * 0.2f);
	m_camera.setPosition(position, true);

	m_uniforms.projection = m_camera.getPerspectiveMatrix();
	m_uniforms.view = m_camera.getViewMatrix();
	m_uniforms.env = m_camera.getRotationMatrix();
	m_uniforms.model = m_trackball.getTransform();
	m_uniforms.normal = Matrix4f::GetNormalMatrix(m_uniforms.view * m_uniforms.model);
	m_uniforms.color = { 0.0f, 1.0f, 0.4f, 1.0f };
	m_uniforms.camPosition = m_camera.getPosition();
	m_uniforms.lightVP = Matrix4f::IDENTITY;
	m_uniforms.shadow = Matrix4f::IDENTITY;
	m_uniforms.lightPosition = Vector3f(0.0f, 0.0f, 0.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_uniforms, sizeof(Uniforms));
	
	Matrix4f matrices[2] = { m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix() , Matrix4f::IDENTITY };
	float pickData[2] = { static_cast<float>(mouse.xPos()),  static_cast<float>(mouse.yPos()) };

	wgpuQueueWriteBuffer(wgpContext.queue, m_computeBuffer.getBuffer(), 0u, matrices, 2 * sizeof(Matrix4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_computeBuffer.getBuffer(), 2 * sizeof(Matrix4f), pickData, 2 * sizeof(float));
}

void PrimitivePicking::render() {
	wgpDraw();
}

void PrimitivePicking::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	{	
		renderPassColorAttachments.push_back(renderPassDescriptor.colorAttachments[0]);

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = renderPassColorAttachments.data();
		rndrPssDscrptor.colorAttachmentCount = renderPassColorAttachments.size();

		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PICK"));
		m_wgpTeapot.draw(renderPassEncoder);		
		
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);

		renderPassColorAttachments.pop_back();	
	}

	if(m_debug)
	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_PICK_DEBUG"));
		
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_debugBindGroup, 0u, NULL);
		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0, 0);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}

	if(Mouse::instance().isVisibile())
	{
		WGPUComputePassEncoder computePassEncoder = wgpuCommandEncoderBeginComputePass(commandEncoder, NULL);
		wgpuComputePassEncoderSetPipeline(computePassEncoder, wgpContext.computePipelines.at("CP_PICK"));

		wgpuComputePassEncoderSetBindGroup(computePassEncoder, 0u, m_computeBindGroup, 0u, NULL);
		wgpuComputePassEncoderDispatchWorkgroups(computePassEncoder, 1u, 1u, 1u);
		wgpuComputePassEncoderEnd(computePassEncoder);
		wgpuComputePassEncoderRelease(computePassEncoder);
	}

	if (m_drawUi) {
		//wgpSubmitQueue();
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

void PrimitivePicking::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void PrimitivePicking::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();	
	}else if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
	}

	uint32_t primitveId = 0u;
	wgpuQueueWriteBuffer(wgpContext.queue, m_computeBuffer.getBuffer(), 2 * sizeof(Matrix4f) + 2 * sizeof(float), &primitveId, sizeof(uint32_t));
}

void PrimitivePicking::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}else if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT) {
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
	}
}

void PrimitivePicking::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void PrimitivePicking::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void PrimitivePicking::OnKeyUp(const Event::KeyboardEvent& event) {

}

void PrimitivePicking::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
}

void PrimitivePicking::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

std::vector<WGPUBindGroupLayout> PrimitivePicking::OnBindGroupLayoutsPick() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = wgpuBufferGetSize(m_computeBuffer.getBuffer());

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> PrimitivePicking::OnBindGroupLayoutsCompute() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Storage;
	bindingLayoutEntries[0].buffer.minBindingSize = wgpuBufferGetSize(m_computeBuffer.getBuffer());

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension::WGPUTextureViewDimension_2D;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType::WGPUTextureSampleType_Uint;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> PrimitivePicking::OnBindGroupLayoutsDebug() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(1);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[0].texture.sampleType = WGPUTextureSampleType_Uint;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	return bindingLayouts;
}

std::vector<WGPUBindGroup> PrimitivePicking::OnBindGroupsPick() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindings(2);

	bindings[0].binding = 0u;
	bindings[0].buffer = m_uniformBuffer.getBuffer();
	bindings[0].offset = 0u;
	bindings[0].size = wgpuBufferGetSize(m_uniformBuffer.getBuffer());

	bindings[1].binding = 1u;
	bindings[1].buffer = m_computeBuffer.getBuffer();
	bindings[1].offset = 0u;
	bindings[1].size = wgpuBufferGetSize(m_computeBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PICK"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindings.size();
	bindGroupDesc.entries = bindings.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
	return bindGroups;
}

WGPUBindGroup PrimitivePicking::createComputeBindGroup() {
	std::vector<WGPUBindGroupEntry> entries(2);

	entries[0].binding = 0u;
	entries[0].buffer = m_computeBuffer.getBuffer();
	entries[0].offset = 0u;
	entries[0].size = wgpuBufferGetSize(m_computeBuffer.getBuffer());

	entries[1].binding = 1u;
	entries[1].textureView = m_indexTexture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuComputePipelineGetBindGroupLayout(wgpContext.computePipelines.at("CP_PICK"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup PrimitivePicking::createDebugBindGroup() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(1);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].textureView = m_indexTexture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_PICK_DEBUG"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

void PrimitivePicking::readBuffer() {
	m_stagingBuffer.createBuffer(2 * sizeof(Matrix4f) + 4 * sizeof(float), WGPUBufferUsage_MapRead | WGPUBufferUsage_CopyDst);

	Matrix4f matrices[2] = { m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix() , Matrix4f::IDENTITY };
	float pickData[2] = { static_cast<float>(550),  static_cast<float>(600) };
	uint32_t id = 100;
	wgpuQueueWriteBuffer(wgpContext.queue, m_computeBuffer.getBuffer(), 0u, matrices, 2 * sizeof(Matrix4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_computeBuffer.getBuffer(), 2 * sizeof(Matrix4f), pickData, 2 * sizeof(float));
	wgpuQueueWriteBuffer(wgpContext.queue, m_computeBuffer.getBuffer(), 2 * sizeof(Matrix4f) + 2 * sizeof(float), &id, 2 * sizeof(float));

	bool readyQueue = false;
	WGPUQueueWorkDoneCallbackInfo queueWorkDoneCallbackInfo = {};
	queueWorkDoneCallbackInfo.callback = OnQueueWorkDone;
	queueWorkDoneCallbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
	queueWorkDoneCallbackInfo.userdata1 = &readyQueue;

	WGPUFuture futureQueue = wgpuQueueOnSubmittedWorkDone(wgpContext.queue, queueWorkDoneCallbackInfo);


	WGPUCommandEncoder commandEncoder = wgpuDeviceCreateCommandEncoder(wgpContext.device, NULL);
	wgpuCommandEncoderCopyBufferToBuffer(commandEncoder, m_computeBuffer.getBuffer(), 0u, m_stagingBuffer.getBuffer(), 0u, 2 * sizeof(Matrix4f) + 4 * sizeof(float));

	WGPUCommandBuffer commandBuffer = wgpuCommandEncoderFinish(commandEncoder, NULL);

	wgpuQueueSubmit(wgpContext.queue, 1, &commandBuffer);

	wgpuCommandEncoderRelease(commandEncoder);
	wgpuCommandBufferRelease(commandBuffer);

	WGPUFutureWaitInfo waitQueue = {};
	waitQueue.future = futureQueue;
	WGPUWaitStatus statusQueue = wgpuInstanceWaitAny(wgpContext.instance, 1, &waitQueue, 0);

	while(!readyQueue)
		wgpuDeviceTick(wgpContext.device);

	if (statusQueue == WGPUWaitStatus_Success) {

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		bool readyBuffer = false;
		WGPUBufferMapCallbackInfo bufferMapCallbackInfo = {};
		bufferMapCallbackInfo.callback = OnMapBuffer;
		bufferMapCallbackInfo.mode = WGPUCallbackMode_WaitAnyOnly;
		bufferMapCallbackInfo.userdata1 = &readyBuffer;



		WGPUFuture futureBuffer = wgpuBufferMapAsync(m_stagingBuffer.getBuffer(), WGPUMapMode_Read, 0u, 2 * sizeof(Matrix4f) + 4 * sizeof(float), bufferMapCallbackInfo);
		WGPUFutureWaitInfo waitBuffer = {};
		waitBuffer.future = futureBuffer;
		WGPUWaitStatus statusBuffer = wgpuInstanceWaitAny(wgpContext.instance, 1, &waitBuffer, 0);

		while (!readyBuffer)
			wgpuDeviceTick(wgpContext.device);


		if (statusBuffer == WGPUWaitStatus_Success) {
			uint8_t* bufferData = (uint8_t*)wgpuBufferGetConstMappedRange(m_stagingBuffer.getBuffer(), 0, 2 * sizeof(Matrix4f) + 4 * sizeof(float));
			std::cout << "X: " << Utils::bytesToFloatLE(bufferData[128], bufferData[129], bufferData[130], bufferData[131]) << std::endl;
			std::cout << "Y: " << Utils::bytesToFloatLE(bufferData[132], bufferData[133], bufferData[134], bufferData[135]) << std::endl;
			std::cout << "ID: " << Utils::bytesToUIntLE(bufferData[136], bufferData[137], bufferData[138], bufferData[139]) << std::endl;
		}
	}
	wgpuBufferUnmap(m_stagingBuffer.getBuffer());
	m_stagingBuffer.markForDelete();
}