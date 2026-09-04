#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include <engine/sound/SoundDevice.h>

#include "VideoDecode.h"
#include "Application.h"
#include "Globals.h"

VideoDecode::VideoDecode(StateMachine& machine) : State(machine, States::VIDEO_DECODE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	SoundDevice::Init();

	//wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_RGBA8Unorm, Application::OnSurfaceChange);
	//wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	m_camera.perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.5f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 0.0f, 5.0f), Vector3f(0.0f, 0.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(20.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	
	wgpContext.setClearColor({ 0.5f, 0.5f, 0.5f, 1.0f });

	wgpContext.addSahderModule("VIDEO_2D", "res/shader/video_2d.wgsl");
	wgpContext.createRenderPipeline("VIDEO_2D", "RP_VIDEO_2D", VL_NONE, std::bind(&VideoDecode::OnBindGroupLayouts, this));

	wgpContext.addSahderModule("VIDEO_360_HW", "res/shader/video_360_hw.wgsl");
	wgpContext.createRenderPipeline("VIDEO_360_HW", "RP_VIDEO_360_HW", VL_NONE, std::bind(&VideoDecode::OnBindGroupLayouts360HW, this));

	wgpContext.addSahderModule("VIDEO_360", "res/shader/video_360_yuv.wgsl");
	wgpContext.createRenderPipeline("VIDEO_360", "RP_VIDEO_360", VL_NONE, std::bind(&VideoDecode::OnBindGroupLayouts360, this));

	m_cameraBuffer.createBuffer(sizeof(CameraUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_movieRGBA.open<RGBADecoder, OpenALPlayer>("res/videos/big_buck_bunny.mp4");	
	m_movieRGBA.getDecoder<RGBADecoder>()->setBindGroup(createBindGroupRGBA());
	m_movieRGBA.queryFirstFrame();

	m_movieYUV.open<YUVDecoder, OpenALPlayer>("res/videos/underwater_diving_360degrees.mp4");
	m_movieYUV.getDecoder<YUVDecoder>()->setBindGroup(createBindGroup360YUV());
	m_movieYUV.queryFirstFrame();

	m_movieHw.open<VulkanDecoder, RtAudioPlayer>("res/videos/360_example.mp4");
	m_movieHw.getDecoder()->setBuffer(m_cameraBuffer.getBuffer());
	m_movieHw.getDecoder<VulkanDecoder>()->setBindGroup(createBindGroup360HW());
	m_movieHw.queryFirstFrame();

	//m_movieHw.open<D3D12Decoder, RtAudioPlayer>("res/videos/360_example.mp4");
	//m_movieHw.getDecoder()->setBuffer(m_cameraBuffer.getBuffer());
	//m_movieHw.getDecoder<D3D12Decoder>()->setBindGroupLayout(wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_360_HW"), 0u));
	//m_movieHw.queryFirstFrame();

	m_movieRGBA.setVolume(0.25f);
	m_movieYUV.setVolume(0.1f);
	m_movieHw.setVolume(0.5f);

	wgpContext.OnDraw = std::bind(&VideoDecode::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	wgpContext.OnPostDraw = std::bind(&VideoDecode::OnPostDraw, this);
}

VideoDecode::~VideoDecode() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_cameraBuffer.markForDelete();
	SoundDevice::ShutDown();
}

void VideoDecode::fixedUpdate() {

}

void VideoDecode::update() {
	
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

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		m_movieHw.togglePause();
	}

	if (keyboard.keyPressed(Keyboard::KEY_RIGHT)) {
		m_movieHw.seekTo(m_movieHw.getCurrentTime() + 5.0);
	}

	if (keyboard.keyPressed(Keyboard::KEY_LEFT)) {
		m_movieHw.seekTo(m_movieHw.getCurrentTime() - 5.0);
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

	CameraUniforms ubo;
	ubo.aspect = static_cast<float>(Application::Width) / static_cast<float>(Application::Height);
	ubo.fov = m_camera.getFovXRad();
	ubo.viewMatrix = m_camera.getViewMatrix();
	wgpuQueueWriteBuffer(wgpContext.queue, m_cameraBuffer.getBuffer(), 0, &ubo, sizeof(CameraUniforms));
	wgpuQueueWriteBuffer(wgpContext.queue, m_movieHw.getDecoder()->getBuffer(), 0, &ubo, sizeof(CameraUniforms));
}

void VideoDecode::render() {
	wgpDraw();
}

void VideoDecode::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	m_movieRGBA.update(m_dt);
	m_movieYUV.update(m_dt);
	m_movieHw.update(m_dt);

	
	float screenWidth = static_cast<float>(Application::Width);
	float screenHeight = static_cast<float>(Application::Height);
	float thirdWidth = screenWidth / 3.0f;

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_VIDEO_2D"));
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, thirdWidth, screenHeight, 0.0f, 1.0f);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_movieRGBA.getDecoder()->getBindGroup(), 0u, NULL);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 3u, 1u, 0u, 0u);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_VIDEO_360"));
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, thirdWidth, 0.0f, thirdWidth, screenHeight, 0.0f, 1.0f);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_movieYUV.getDecoder()->getBindGroup(), 0u, NULL);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 3u, 1u, 0u, 0u);

	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_VIDEO_360_HW"));
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, thirdWidth * 2.0f, 0.0f, thirdWidth, screenHeight, 0.0f, 1.0f);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_movieHw.getDecoder()->getBindGroup(), 0u, NULL);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 3u, 1u, 0u, 0u);

	if (m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);
}

void VideoDecode::OnPostDraw() {
	m_movieHw.OnPostDraw();
}

void VideoDecode::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void VideoDecode::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void VideoDecode::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void VideoDecode::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void VideoDecode::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void VideoDecode::OnKeyUp(const Event::KeyboardEvent& event) {

}

void VideoDecode::resize(int deltaW, int deltaH) {
	m_camera.perspective(30.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.5f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void VideoDecode::renderVideoTimeline(const char* label, VideoDecoder& movie, SliderState& state) {
	double currentSec = movie.getCurrentTime();
	double totalSec = movie.getDuration();

	if (!state.isUserDragging) {
		state.sliderTime = static_cast<float>(currentSec);
	}
	float maxTime = static_cast<float>(totalSec);

	int curMin = static_cast<int>(state.sliderTime) / 60;
	int curTimeSec = static_cast<int>(state.sliderTime) % 60;
	int totMin = static_cast<int>(totalSec) / 60;
	int totTimeSec = static_cast<int>(totalSec) % 60;

	char timeString[64];
	sprintf(timeString, "%02d:%02d / %02d:%02d", curMin, curTimeSec, totMin, totTimeSec);

	//ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.25f);

	// Eindeutige ID für ImGui durch das Label sichern
	if (ImGui::SliderFloat(label, &state.sliderTime, 0.0f, maxTime, timeString)) {
		state.isUserDragging = true;
	}

	if (state.isUserDragging && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		movie.seekTo(static_cast<double>(state.sliderTime));
		state.isUserDragging = false;
	}
}

void VideoDecode::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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
	std::string buttonText = m_movieRGBA.isPaused() ? "Play" : "Pause";
	if (ImGui::Button(buttonText.c_str(), ImVec2(70, 0))) {
		m_movieRGBA.togglePause();
		m_movieYUV.togglePause();
		m_movieHw.togglePause();	
	}
	ImGui::NewLine();

	ImGui::Indent(15.0f);
	renderVideoTimeline("##TimelineRgba", m_movieRGBA, m_stateRGBA);
	ImGui::Spacing();
	renderVideoTimeline("##TimelinePacked", m_movieYUV, m_stateYUV);
	ImGui::Spacing();
	renderVideoTimeline("##TimelineHw", m_movieHw, m_stateHw);
	ImGui::Unindent(15.0f);
	ImGui::NewLine();
	float volLeft = m_movieRGBA.getVolume();
	if (ImGui::SliderFloat("Left", &volLeft, 0.0f, 1.0f, "%.2f")) {
		m_movieRGBA.setVolume(volLeft);
	}

	float volMiddle = m_movieYUV.getVolume();
	if (ImGui::SliderFloat("Middle", &volMiddle, 0.0f, 1.0f, "%.2f")) {
		m_movieYUV.setVolume(volMiddle);
	}

	float volRight = m_movieHw.getVolume();
	if (ImGui::SliderFloat("Right", &volRight, 0.0f, 1.0f, "%.2f")) {
		m_movieHw.setVolume(volRight);
	}

	//ImGui::PopItemWidth();
	ImGui::End();

	ImGui::Render();
	ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderPassEncoder);
}

std::vector<WGPUBindGroupLayout> VideoDecode::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(2);
	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[1].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

std::vector<WGPUBindGroupLayout> VideoDecode::OnBindGroupLayouts360() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(CameraUniforms);

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

std::vector<WGPUBindGroupLayout> VideoDecode::OnBindGroupLayouts360HW() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(4);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType::WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(CameraUniforms);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;

	bindingLayoutEntries[3].binding = 3u;
	bindingLayoutEntries[3].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[3].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[3].texture.sampleType = WGPUTextureSampleType_Float;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);

	return bindingLayouts;
}

WGPUBindGroup VideoDecode::createBindGroupRGBA() {
	std::vector<WGPUBindGroupEntry> entries(2);

	entries[0].binding = 0u;
	entries[0].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	entries[1].binding = 1u;
	entries[1].textureView = m_movieRGBA.getDecoder()->getTextureViewY();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_2D"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup VideoDecode::createBindGroup360YUV() {
	std::vector<WGPUBindGroupEntry> entries(3);

	entries[0].binding = 0u;
	entries[0].buffer = m_cameraBuffer.getBuffer();
	entries[0].offset = 0u;
	entries[0].size = sizeof(CameraUniforms);

	entries[1].binding = 1u;
	entries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	entries[2].binding = 2u;
	entries[2].textureView = m_movieYUV.getDecoder()->getTextureViewY();
	
	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_360"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup VideoDecode::createBindGroup360HW() {
	std::vector<WGPUBindGroupEntry> entries(4u);

	entries[0].binding = 0u;
	entries[0].buffer = m_cameraBuffer.getBuffer();
	entries[0].offset = 0u;
	entries[0].size = sizeof(CameraUniforms);

	entries[1].binding = 1u;
	entries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	entries[2].binding = 2u;
	entries[2].textureView = m_movieHw.getDecoder()->getTextureViewY();

	entries[3].binding = 3u;
	entries[3].textureView = m_movieHw.getDecoder()->getTextureViewUV();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_360_HW"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}