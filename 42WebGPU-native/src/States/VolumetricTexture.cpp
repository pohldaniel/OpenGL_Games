#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <engine/input/GamePad.h>
#include <States/Menu.h>

#include "Globals.h"
#include "Application.h"
#include "VolumetricTexture.h"

VolumetricTexture::VolumetricTexture(StateMachine& machine) : State(machine, States::VOLUMETRIC_TEXTURE) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f , -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, -5.0f), Vector3f(0.0f, 0.0f, 0.0f) + Vector3f(0.0f, 0.0f, -1.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(20.0f);
	m_camera.setRotationSpeed(0.1f);

	m_cube.buildCube({ -1.0f, -1.0f, -1.0f }, { 2.0f, 2.0f, 2.0f }, 1u, 1u, false, false);
	m_trackball.reshape(Application::Width, Application::Height);

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

	m_splatBuffer.createBuffer(sizeof(SplatUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_waveBuffer.createBuffer(sizeof(WaveUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	m_rayMarchBuffer.createBuffer(sizeof(RayMarchUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_current = 0;
	m_state[0] = new VolumeBuffer(wgpContext.device, m_width, m_height, m_depth);
	m_state[1] = new VolumeBuffer(wgpContext.device, m_width, m_height, m_depth);
	m_prevState = new VolumeBuffer(wgpContext.device, m_width, m_height, m_depth);

	wgpContext.addSahderModule("VOL_RAYMARCH", "res/shader/ray_march.wgsl");
	wgpContext.createRenderPipeline("VOL_RAYMARCH", "RP_VOL_RAYMARCH", VL_P,
		{ wgpContext.createBindGroupLayout()
			.addUniformBuffer(0, WGPUShaderStage_Vertex | WGPUShaderStage_Fragment, sizeof(RayMarchUniforms))
			.addTexture3D(1, WGPUShaderStage_Fragment)
			.addSampler(2, WGPUShaderStage_Fragment)
			.build() },
		1u, WGPUPrimitiveTopology_Undefined, WGPUTextureFormat_Undefined, WGPUTextureFormat_Undefined, WGPUCompareFunction_Less,
		{ FRAGMENT_STATE | DEPTH_STENCIL_STATE, ColorMode::WRITE_RGBA, DepthMode::PASS, StencilMode::DEFAULT, BlendMode::ALPHA_BLENDING_ONE, WGPUTextureFormat_Undefined, WGPUCullMode_Front });

	m_bgRayMarch[0] = wgpContext.createRenderBindGroup("RP_VOL_RAYMARCH")
		.bindBuffer(0, m_rayMarchBuffer.getBuffer(), sizeof(RayMarchUniforms))
		.bindTextureView(1, m_state[0]->view)
		.bindSampler(2, wgpContext.getSampler(SS_LINEAR_CLAMP))
		.build();

	m_bgRayMarch[1] = wgpContext.createRenderBindGroup("RP_VOL_RAYMARCH")
		.bindBuffer(0, m_rayMarchBuffer.getBuffer(), sizeof(RayMarchUniforms))
		.bindTextureView(1, m_state[1]->view)
		.bindSampler(2, wgpContext.getSampler(SS_LINEAR_CLAMP))
		.build();

	wgpContext.addSahderModule("VOL_SPLAT", "res/shader/splat.wgsl");
	wgpContext.createComputePipeline("VOL_SPLAT", "cs_splat", "CP_VOL_SPLAT", {
	wgpContext.createBindGroupLayout()
		.addTexture3D(0, WGPUShaderStage_Compute, WGPUTextureSampleType_UnfilterableFloat)
		.addStorageTexture3D(1, WGPUShaderStage_Compute, WGPUTextureFormat_RGBA16Float)
		.addUniformBuffer(2, WGPUShaderStage_Compute, sizeof(SplatUniforms))
		.build()
		});

	m_bgSplat[0] = wgpContext.createComputeBindGroup("CP_VOL_SPLAT")
		.bindTextureView(0, m_state[0]->view)
		.bindTextureView(1, m_state[1]->view)
		.bindBuffer(2, m_splatBuffer.getBuffer(), sizeof(SplatUniforms))
		.build();

	m_bgSplat[1] = wgpContext.createComputeBindGroup("CP_VOL_SPLAT")
		.bindTextureView(0, m_state[1]->view)
		.bindTextureView(1, m_state[0]->view)
		.bindBuffer(2, m_splatBuffer.getBuffer(), sizeof(SplatUniforms))
		.build();

	wgpContext.addSahderModule("VOL_WAVES", "res/shader/waves.wgsl");
	wgpContext.createComputePipeline("VOL_WAVES", "cs_waves", "CP_VOL_WAVES", {
	wgpContext.createBindGroupLayout()
		.addTexture3D(0, WGPUShaderStage_Compute, WGPUTextureSampleType_UnfilterableFloat)
		.addTexture3D(1, WGPUShaderStage_Compute, WGPUTextureSampleType_UnfilterableFloat)
		.addStorageTexture3D(2, WGPUShaderStage_Compute, WGPUTextureFormat_RGBA16Float)
		.addUniformBuffer(3, WGPUShaderStage_Compute, sizeof(WaveUniforms))
		.build()
		});

	m_bgWaves[0] = wgpContext.createComputeBindGroup("CP_VOL_WAVES")
		.bindTextureView(0, m_state[0]->view)
		.bindTextureView(1, m_prevState->view)
		.bindTextureView(2, m_state[1]->view)
		.bindBuffer(3, m_waveBuffer.getBuffer(), sizeof(WaveUniforms))
		.build();

	m_bgWaves[1] = wgpContext.createComputeBindGroup("CP_VOL_WAVES")
		.bindTextureView(0, m_state[1]->view)
		.bindTextureView(1, m_prevState->view)
		.bindTextureView(2, m_state[0]->view)
		.bindBuffer(3, m_waveBuffer.getBuffer(), sizeof(WaveUniforms))
		.build();
	

	WaveUniforms waveUniforms = {};
	waveUniforms.voxelSize = Vector3f(1.0f / m_width, 1.0f / m_height, 1.0f / m_depth);
	waveUniforms.damping = 0.99f;
	wgpuQueueWriteBuffer(wgpContext.queue, m_waveBuffer.getBuffer(), 0u, &waveUniforms, sizeof(WaveUniforms));

	m_wgpCube.create(m_cube);
	wgpContext.setClearColor({ 0.0f, 0.25f, 0.5f, 1.0f });
	wgpContext.OnDraw = std::bind(&VolumetricTexture::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
}

VolumetricTexture::~VolumetricTexture() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_uniformBuffer.markForDelete();
	m_splatBuffer.markForDelete();
	m_waveBuffer.markForDelete();
	m_rayMarchBuffer.markForDelete();

	for (int i = 0; i < 2; ++i) {
		if (m_bgSplat[i]) {
			wgpuBindGroupRelease(m_bgSplat[i]);
			m_bgSplat[i] = nullptr;
		}
		if (m_bgWaves[i]) {
			wgpuBindGroupRelease(m_bgWaves[i]);
			m_bgWaves[i] = nullptr;
		}
		if (m_bgRayMarch[i]) {
			wgpuBindGroupRelease(m_bgRayMarch[i]);
			m_bgRayMarch[i] = nullptr;
		}
	}

	delete m_state[0];
	m_state[0] = nullptr;

	delete m_state[1];
	m_state[1] = nullptr;

	delete m_prevState;
	m_prevState = nullptr;
}

void VolumetricTexture::fixedUpdate() {

}

void VolumetricTexture::update() {
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

void VolumetricTexture::render() {
	wgpDraw();
}

Matrix4f invertRigidBody(const Matrix4f& m) {
	// Oberer 3x3-Block wird transponiert (R^T)
	// Translation wird zu -R^T * t
	// (Passend zu deinem Matrix4f-Layout aus Quaternion::toMatrix4f: Translation in Zeile 3 / Elemente 12,13,14)
	Matrix4f inv;
	inv[0][0] = m[0][0]; inv[0][1] = m[1][0]; inv[0][2] = m[2][0]; inv[0][3] = 0.0f;
	inv[1][0] = m[0][1]; inv[1][1] = m[1][1]; inv[1][2] = m[2][1]; inv[1][3] = 0.0f;
	inv[2][0] = m[0][2]; inv[2][1] = m[1][2]; inv[2][2] = m[2][2]; inv[2][3] = 0.0f;

	float tx = m[3][0], ty = m[3][1], tz = m[3][2];
	inv[3][0] = -(tx * inv[0][0] + ty * inv[1][0] + tz * inv[2][0]);
	inv[3][1] = -(tx * inv[0][1] + ty * inv[1][1] + tz * inv[2][1]);
	inv[3][2] = -(tx * inv[0][2] + ty * inv[1][2] + tz * inv[2][2]);
	inv[3][3] = 1.0f;
	return inv;
}

void VolumetricTexture::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	
	uint32_t wgX = (m_width + 3u) / 4u;
	uint32_t wgY = (m_height + 3u) / 4u;
	uint32_t wgZ = (m_depth + 3u) / 4u;

	{
		WGPUTexelCopyTextureInfo src = {};
		src.texture = m_state[1 - m_current]->texture;
		src.mipLevel = 0u;
		src.origin = { 0u, 0u, 0u };
		src.aspect = WGPUTextureAspect_All;

		WGPUTexelCopyTextureInfo dst = {};
		dst.texture = m_prevState->texture;
		dst.mipLevel = 0u;
		dst.origin = { 0u, 0u, 0u };
		dst.aspect = WGPUTextureAspect_All;

		WGPUExtent3D copySize = { m_width, m_height, m_depth };
		wgpuCommandEncoderCopyTextureToTexture(commandEncoder, &src, &dst, &copySize);
	}

	if ((rand() & 0xff) > 220) {
		splat();
	}

	if (m_pendingSplat) {
		m_pendingSplat = false;

		WGPUComputePassEncoder computePass = wgpuCommandEncoderBeginComputePass(commandEncoder, nullptr);
		wgpuComputePassEncoderSetPipeline(computePass, wgpContext.computePipelines.at("CP_VOL_SPLAT"));
		wgpuComputePassEncoderSetBindGroup(computePass, 0u, m_bgSplat[m_current], 0u, nullptr);
		wgpuComputePassEncoderDispatchWorkgroups(computePass, wgX, wgY, wgZ);
		wgpuComputePassEncoderEnd(computePass);
		wgpuComputePassEncoderRelease(computePass);

		m_current = 1 - m_current;
	}

	{
		WGPUComputePassEncoder computePass = wgpuCommandEncoderBeginComputePass(commandEncoder, nullptr);
		wgpuComputePassEncoderSetPipeline(computePass, wgpContext.computePipelines.at("CP_VOL_WAVES"));
		wgpuComputePassEncoderSetBindGroup(computePass, 0u, m_bgWaves[m_current], 0u, nullptr);
		wgpuComputePassEncoderDispatchWorkgroups(computePass, wgX, wgY, wgZ);
		wgpuComputePassEncoderEnd(computePass);
		wgpuComputePassEncoderRelease(computePass);

		m_current = 1 - m_current;
	}

	{		
		RayMarchUniforms rmUniforms = {};
		rmUniforms.u_projection = m_camera.getPerspectiveMatrix();
		rmUniforms.u_modelView = m_camera.getViewMatrix() * m_trackball.getTransform();
		rmUniforms.u_invModelView = m_trackball.getInvTransform() * m_camera.getInvViewMatrix();
		rmUniforms.boxMin = Vector3f(-1.0f, -1.0f, -1.0f);
		rmUniforms.density = 0.01f;
		rmUniforms.boxMax = Vector3f(1.0f, 1.0f, 1.0f);
		rmUniforms.brightness = 2.0f;
		rmUniforms.steps = 128;
		rmUniforms.threshold = 0.99f;

		wgpuQueueWriteBuffer(wgpContext.queue, m_rayMarchBuffer.getBuffer(), 0u, &rmUniforms, sizeof(RayMarchUniforms));
	}

	{
		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);

		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_VOL_RAYMARCH"));
		wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bgRayMarch[m_current], 0u, nullptr);
		m_wgpCube.drawRaw(renderPassEncoder);
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
	}
}

void VolumetricTexture::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void VolumetricTexture::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void VolumetricTexture::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void VolumetricTexture::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void VolumetricTexture::OnKeyDown(const Event::KeyboardEvent& event) {
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

void VolumetricTexture::OnKeyUp(const Event::KeyboardEvent& event) {

}

void VolumetricTexture::resize(int deltaW, int deltaH) {
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
}

void VolumetricTexture::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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

void VolumetricTexture::splat() {
	auto frand = []() { return rand() / static_cast<float>(RAND_MAX); };

	SplatUniforms splatUniforms = {};
	splatUniforms.center = Vector3f(frand(), frand(), frand());
	splatUniforms.radius = 0.1f;
	splatUniforms.color = Vector4f(frand(), frand(), frand(), 1.0f);

	wgpuQueueWriteBuffer(wgpContext.queue, m_splatBuffer.getBuffer(), 0u, &splatUniforms, sizeof(SplatUniforms));
	m_pendingSplat = true;
}