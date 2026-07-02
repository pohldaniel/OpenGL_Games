#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "DeferredRendering.h"
#include "Application.h"
#include "Globals.h"

DeferredRendering::DeferredRendering(StateMachine& machine) : State(machine, States::DEFFERED_RENDERING) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 50.0f, -100.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(20.0f);
	m_camera.setRotationSpeed(0.1f);

	m_dragon.loadModel("res/models/dragon_vrip_res4.ply", Vector3f(0.0f, 1.0f, 0.0f), 0.0f, Vector3f(0.0f, -45.0f, 0.0f), 500.0f);
	m_dragon.generateNormals();
	m_dragon.generateUVs();

	m_quad.buildQuadXZ({ -100.0f, -25.0f, -100.0f }, { 200.0f, 200.0f }, 1u, 1u, true, true);

	m_trackball.reshape(Application::Width, Application::Height);

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });

	m_uniformBuffer.createBuffer(2u * sizeof(Matrix4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &Matrix4f::IDENTITY, sizeof(Matrix4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 16u * sizeof(float), &Matrix4f::IDENTITY, sizeof(Matrix4f));

	m_cameraBuffer.createBuffer(2u * sizeof(Matrix4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpuQueueWriteBuffer(wgpContext.queue, m_cameraBuffer.getBuffer(), 0u, &(m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix()), sizeof(Matrix4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_cameraBuffer.getBuffer(), 64u, &(m_camera.getInvViewMatrix() * m_camera.getInvPerspectiveMatrix()), sizeof(Matrix4f));

	m_configBuffer.createBuffer(sizeof(uint32_t), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpuQueueWriteBuffer(wgpContext.queue, m_configBuffer.getBuffer(), 0u, &m_numLights, sizeof(uint32_t));

	m_lightBuffer.createBuffer(sizeof(float) * 8u * MAX_NUM_LIGHTS, WGPUBufferUsage_Storage, true);

	Vector3f  light_extent_min = { -50.f, -30.f, -50.f };
	Vector3f  light_extent_max = { 50.f, 100.f, 50.f };
	Vector3f extent = light_extent_max - light_extent_min;

	float* light_data = (float*)wgpuBufferGetMappedRange(m_lightBuffer.getBuffer(), 0u, sizeof(float) * 8u * MAX_NUM_LIGHTS);
	Vector4f tmp_vec4 = {0.0f, 0.0f, 0.0f, 0.0f};

	for (uint32_t i = 0, offset = 0; i < MAX_NUM_LIGHTS; ++i) {
		offset = 8u * i;
		// position
		for (uint8_t j = 0; j < 3; j++) {
			tmp_vec4[j] = randomFloat(0.0f, 1.0f) * extent[j] + light_extent_min[j];
		}
		tmp_vec4[3] = 1.0f;
		memcpy(&light_data[offset], &tmp_vec4[0], sizeof(Vector4f));
		// color
		tmp_vec4[0] = randomFloat(0.0f, 1.0f) * 2.0f;
		tmp_vec4[1] = randomFloat(0.0f, 1.0f) * 2.0f;
		tmp_vec4[2] = randomFloat(0.0f, 1.0f) * 2.0f;
		// radius
		tmp_vec4[3] = 20.0f;
		memcpy(&light_data[offset + 4], &tmp_vec4[0], sizeof(Vector4f));
	}
	wgpuBufferUnmap(m_lightBuffer.getBuffer());

	m_extentBuffer.createBuffer(4u * 8u, WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	float light_extent_data[8] = { 0 };
	memcpy(&light_extent_data[0], &light_extent_min, sizeof(Vector3f));
	memcpy(&light_extent_data[4], &light_extent_max, sizeof(Vector3f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_extentBuffer.getBuffer(), 0u, &light_extent_data, 32u);

	wgpContext.addSahderModule("DEFERRED", "res/shader/deferred.wgsl");
	wgpContext.createRenderPipeline("DEFERRED", "RP_DEFERRED", VL_NONE, std::bind(&DeferredRendering::OnBindGroupLayoutsDeferred, this));

	wgpContext.addSahderModule("DEFERRED_DEBUG", "res/shader/deferred_debug.wgsl");
	wgpContext.createRenderPipeline("DEFERRED_DEBUG", "RP_DEFERRED_DEBUG", VL_NONE, std::bind(&DeferredRendering::OnBindGroupLayoutsDeferredDebug, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined , WGPUCullMode_Undefined,  DEFAULT ,  
		  { 
			{ NULL, STRVIEW("canvasSizeWidth"), static_cast<double>(Application::Width)   },
		    { NULL, STRVIEW("canvasSizeHeight"), static_cast<double>(Application::Height) } 
		  } 
		}
	);

	wgpContext.addSahderModule("GBUFFER", "res/shader/deferred_gbuffer.wgsl");
	wgpContext.createRenderPipeline("GBUFFER", "RP_GBUFFER", VL_PTN, std::bind(&DeferredRendering::OnBindGroupLayoutsGBuffer, this),
		1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_BGRA8Unorm,
		WGPUTextureFormat_Depth24Plus,
		WGPUCompareFunction_Less,
		{ WRITE_DEPTH | DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_RGBA16Float , WGPUCullMode_Undefined,  DEFAULT }
	);

	wgpContext.addSahderModule("COMPUTE", "res/shader/deferred_compute.wgsl");
	wgpContext.createComputePipeline("COMPUTE", "main", "CP_DEFFERED", std::bind(&DeferredRendering::OnBindGroupLayoutsCompute, this));

	wgpContext.OnDraw = std::bind(&DeferredRendering::OnDraw, this, std::placeholders::_1, std::placeholders::_2);

	m_wgpDragon.create(m_dragon);
	m_wgpDragon.setBindGroups("BG", std::bind(&DeferredRendering::OnBindGroupsGBuffer, this));

	m_wgpQuad.create(m_quad);
	m_wgpQuad.setBindGroups("BG", std::bind(&DeferredRendering::OnBindGroupsGBuffer, this));

	m_normalTexture.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_RGBA16Float);
	m_albedoTexture.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_BGRA8Unorm);
	m_depthTexture.createEmpty(Application::Width, Application::Height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_RenderAttachment, WGPUTextureFormat_Depth24Plus);
	
	WGPURenderPassColorAttachment renderPassColorAttachmentNormal = {};
	renderPassColorAttachmentNormal.view = m_normalTexture.getTextureView();
	renderPassColorAttachmentNormal.resolveTarget = NULL;
	renderPassColorAttachmentNormal.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachmentNormal.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachmentNormal.clearValue = { 0.0f, 0.0f, 1.0f, 1.0f };
	renderPassColorAttachmentNormal.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	renderPassColorAttachments.push_back(renderPassColorAttachmentNormal);

	WGPURenderPassColorAttachment renderPassColorAttachmentAlbedo = {};
	renderPassColorAttachmentAlbedo.view = m_albedoTexture.getTextureView();
	renderPassColorAttachmentAlbedo.resolveTarget = NULL;
	renderPassColorAttachmentAlbedo.loadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassColorAttachmentAlbedo.storeOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassColorAttachmentAlbedo.clearValue = { 0.0f, 0.0f, 0.0f, 1.0f };
	renderPassColorAttachmentAlbedo.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;
	renderPassColorAttachments.push_back(renderPassColorAttachmentAlbedo);

	renderPassDepthStencilAttachment = {};
	renderPassDepthStencilAttachment.view = m_depthTexture.getTextureView();
	renderPassDepthStencilAttachment.depthClearValue = 1.0f;
	renderPassDepthStencilAttachment.depthLoadOp = WGPULoadOp::WGPULoadOp_Clear;
	renderPassDepthStencilAttachment.depthStoreOp = WGPUStoreOp::WGPUStoreOp_Store;
	renderPassDepthStencilAttachment.depthReadOnly = WGPUOptionalBool::WGPUOptionalBool_False;
	renderPassDepthStencilAttachment.stencilClearValue = 0u;
	renderPassDepthStencilAttachment.stencilLoadOp = WGPULoadOp::WGPULoadOp_Undefined;
	renderPassDepthStencilAttachment.stencilStoreOp = WGPUStoreOp::WGPUStoreOp_Undefined;
	renderPassDepthStencilAttachment.stencilReadOnly = WGPUOptionalBool::WGPUOptionalBool_True;

	m_deferredBindGroup = createDeferredBindGroup();
	m_lightBindGroup = createLightBindGroup();
	m_computeBindGroup = createComputeBindGroup();
}

DeferredRendering::~DeferredRendering() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete(); 
	m_cameraBuffer.markForDelete(); 
	m_lightBuffer.markForDelete(); 
	m_configBuffer.markForDelete(); 
	m_extentBuffer.markForDelete();

	m_normalTexture.markForDelete();
	m_albedoTexture.markForDelete();
	m_depthTexture.markForDelete();

	wgpuBindGroupRelease(m_deferredBindGroup);
	wgpuBindGroupRelease(m_lightBindGroup);
	wgpuBindGroupRelease(m_computeBindGroup);
}

void DeferredRendering::fixedUpdate() {

}

void DeferredRendering::update() {
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

	Vector3f::RotateY(position, m_dt * 90.0f);
	m_camera.setPosition(position, true);

	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0, &m_trackball.getTransform(), sizeof(Matrix4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 64u, &Matrix4f::IDENTITY, sizeof(Matrix4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_cameraBuffer.getBuffer(), 0, &(m_camera.getPerspectiveMatrix() * m_camera.getViewMatrix()), sizeof(Matrix4f));
	wgpuQueueWriteBuffer(wgpContext.queue, m_cameraBuffer.getBuffer(), 64u, &(m_camera.getInvViewMatrix() * m_camera.getInvPerspectiveMatrix()), sizeof(Matrix4f));
}

void DeferredRendering::render() {
	wgpDraw();
}

void DeferredRendering::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	{	
		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = renderPassColorAttachments.data();
		rndrPssDscrptor.colorAttachmentCount = renderPassColorAttachments.size();
		rndrPssDscrptor.depthStencilAttachment = &renderPassDepthStencilAttachment;

		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &rndrPssDscrptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_GBUFFER"));

		m_wgpDragon.draw(renderPassEncoder);
		m_wgpQuad.draw(renderPassEncoder);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}

	{
		WGPUComputePassEncoder computePassEncoder = wgpuCommandEncoderBeginComputePass(commandEncoder, NULL);
		wgpuComputePassEncoderSetPipeline(computePassEncoder, wgpContext.computePipelines.at("CP_DEFFERED"));
		wgpuComputePassEncoderSetBindGroup(computePassEncoder, 0, m_computeBindGroup, 0, NULL);
		wgpuComputePassEncoderDispatchWorkgroups(computePassEncoder, (uint32_t)ceilf(m_numLights / 64.f), 1u, 1u);
		wgpuComputePassEncoderEnd(computePassEncoder);
		wgpuComputePassEncoderRelease(computePassEncoder);
	}

	if (m_debug) {		
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_DEFERRED_DEBUG"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_deferredBindGroup, 0u, 0u);

		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);
		wgpuRenderPassEncoderEnd(renderPassEncoder);

		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}else {
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, 1.0f);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_DEFERRED"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_deferredBindGroup, 0u, NULL);
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 1u, m_lightBindGroup, 0u, NULL);

		wgpuRenderPassEncoderDraw(renderPassEncoder, 6u, 1u, 0u, 0u);

		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
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

void DeferredRendering::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void DeferredRendering::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);

}

void DeferredRendering::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void DeferredRendering::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void DeferredRendering::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void DeferredRendering::OnKeyUp(const Event::KeyboardEvent& event) {

}

void DeferredRendering::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 1.0f, 2000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);

	m_normalTexture.resize(Application::Width, Application::Height);
	m_albedoTexture.resize(Application::Width, Application::Height);
	m_depthTexture.resize(Application::Width, Application::Height);

	wgpuBindGroupRelease(m_deferredBindGroup);
	m_deferredBindGroup = createDeferredBindGroup();

	renderPassColorAttachments[0].view = m_normalTexture.getTextureView();
	renderPassColorAttachments[1].view = m_albedoTexture.getTextureView();
	renderPassDepthStencilAttachment.view = m_depthTexture.getTextureView();
}

void DeferredRendering::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	if (ImGui::SliderInt("Num Lights", &m_numLights, 0, MAX_NUM_LIGHTS)) {
		wgpuQueueWriteBuffer(wgpContext.queue, m_configBuffer.getBuffer(), 0u, &m_numLights, sizeof(uint32_t));
	}	
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> DeferredRendering::OnBindGroupLayoutsGBuffer() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Matrix4f) * 2u;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(Matrix4f) * 2u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> DeferredRendering::OnBindGroupLayoutsCompute() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Storage;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(float) * 8u * MAX_NUM_LIGHTS;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[1].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[1].buffer.minBindingSize = sizeof(uint32_t);

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Compute;
	bindingLayoutEntries[2].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[2].buffer.minBindingSize = 0u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> DeferredRendering::OnBindGroupLayoutsDeferred() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(2);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries0(3);
	bindingLayoutEntries0[0].binding = 0u;
	bindingLayoutEntries0[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries0[0].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries0[0].texture.sampleType = WGPUTextureSampleType_UnfilterableFloat;

	bindingLayoutEntries0[1].binding = 1u;
	bindingLayoutEntries0[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries0[1].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries0[1].texture.sampleType = WGPUTextureSampleType_UnfilterableFloat;

	bindingLayoutEntries0[2].binding = 2u;
	bindingLayoutEntries0[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries0[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries0[2].texture.sampleType = WGPUTextureSampleType_Depth;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor0 = {};
	bindGroupLayoutDescriptor0.entryCount = (uint32_t)bindingLayoutEntries0.size();
	bindGroupLayoutDescriptor0.entries = bindingLayoutEntries0.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor0);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries1(3);

	bindingLayoutEntries1[0].binding = 0u;
	bindingLayoutEntries1[0].visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
	bindingLayoutEntries1[0].buffer.type = WGPUBufferBindingType_ReadOnlyStorage;
	bindingLayoutEntries1[0].buffer.minBindingSize = sizeof(float) * 8u * MAX_NUM_LIGHTS;

	bindingLayoutEntries1[1].binding = 1u;
	bindingLayoutEntries1[1].visibility = WGPUShaderStage_Fragment | WGPUShaderStage_Compute;
	bindingLayoutEntries1[1].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries1[1].buffer.minBindingSize = sizeof(uint32_t);

	bindingLayoutEntries1[2].binding = 2u;
	bindingLayoutEntries1[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries1[2].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries1[2].buffer.minBindingSize = sizeof(Matrix4f) * 2u;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor1 = {};
	bindGroupLayoutDescriptor1.entryCount = (uint32_t)bindingLayoutEntries1.size();
	bindGroupLayoutDescriptor1.entries = bindingLayoutEntries1.data();

	bindingLayouts[1] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor1);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> DeferredRendering::OnBindGroupLayoutsDeferredDebug() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[0].texture.sampleType = WGPUTextureSampleType_UnfilterableFloat;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType_UnfilterableFloat;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Depth;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroup> DeferredRendering::OnBindGroupsGBuffer() {
	std::vector<WGPUBindGroup> bindGroups(1);

	std::vector<WGPUBindGroupEntry> bindGroupEntries(2);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = 2u * sizeof(Matrix4f);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_cameraBuffer.getBuffer();
	bindGroupEntries[1].offset = 0u;
	bindGroupEntries[1].size = 2u * sizeof(Matrix4f);

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_GBUFFER"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	bindGroups[0] = wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);

	return bindGroups;
}

WGPUBindGroup DeferredRendering::createDeferredBindGroup() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].textureView = m_normalTexture.getTextureView();

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].textureView = m_albedoTexture.getTextureView();

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_depthTexture.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_DEFERRED"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup DeferredRendering::createLightBindGroup() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_lightBuffer.getBuffer();
	bindGroupEntries[0].size = wgpuBufferGetSize(m_lightBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_configBuffer.getBuffer();
	bindGroupEntries[1].size = wgpuBufferGetSize(m_configBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_cameraBuffer.getBuffer();
	bindGroupEntries[2].size = wgpuBufferGetSize(m_cameraBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_DEFERRED"), 1u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup DeferredRendering::createComputeBindGroup() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);

	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_lightBuffer.getBuffer();
	bindGroupEntries[0].size = wgpuBufferGetSize(m_lightBuffer.getBuffer());

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].buffer = m_configBuffer.getBuffer();
	bindGroupEntries[1].size = wgpuBufferGetSize(m_configBuffer.getBuffer());

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].buffer = m_extentBuffer.getBuffer();
	bindGroupEntries[2].size = wgpuBufferGetSize(m_extentBuffer.getBuffer());

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuComputePipelineGetBindGroupLayout(wgpContext.computePipelines.at("CP_DEFFERED"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

float DeferredRendering::randomFloat(float min, float max) {
	return ((max - min) * ((float)rand() / (float)RAND_MAX)) + min;
}