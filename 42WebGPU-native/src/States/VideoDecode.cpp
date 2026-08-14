#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "VideoDecode.h"
#include "Application.h"
#include "Globals.h"



#define posix_memalign(p, a, s) (((*(p)) = _aligned_malloc((s), (a))), *(p) ?0 :errno)

VideoDecode::VideoDecode(StateMachine& machine) : State(machine, States::VIDEO_DECODE) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

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
	wgpContext.OnDraw = std::bind(&VideoDecode::OnDraw, this, std::placeholders::_1, std::placeholders::_2);

	wgpContext.addSahderModule("VIDEO_360", "res/shader/video_360_packed.wgsl");
	wgpContext.createRenderPipeline("VIDEO_360", "RP_VIDEO_360", VL_NONE, std::bind(&VideoDecode::OnBindGroupLayouts360, this));
	wgpContext.OnDraw = std::bind(&VideoDecode::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	wgpContext.OnPostDraw = std::bind(&VideoDecode::OnPostDraw, this);

	m_audioSystem = std::make_unique<OpenALAudioSystem>();
	m_audioSystem->init();

	m_movieLeft.open("res/videos/big_buck_bunny.mp4");
	
	m_movieRight.m_isPackedYuv = true;
	//m_movieRight.open("res/videos/underwater_diving_360degrees.mp4");
	m_movieRight.open("res/videos/360_example.mp4");

	m_textureLeft.createEmpty(m_movieLeft.getWidth(), m_movieLeft.getHeight(), 1u, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, WGPUTextureFormat_RGBA8Unorm);
	//m_textureRight.createEmpty(m_movieRight.getWidth(), m_movieRight.getHeight(), 1u, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, WGPUTextureFormat_RGBA8Unorm);
	
	m_textureRight.createEmpty(m_movieRight.getWidth(), m_movieRight.getHeight() + m_movieRight.getHeight()/2, 1u, WGPUTextureUsage_CopyDst | WGPUTextureUsage_TextureBinding, WGPUTextureFormat_R8Unorm);
	m_cameraBuffer.createBuffer(sizeof(CameraUniforms), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);

	m_openALStreamLeft.init();

	m_rtAudioPlayer.init();
	m_rtAudioPlayer.startHardwareStream();

	m_bindGroupLeft = createBindGroupLeft();
	m_bindGroupRight = createBindGroupRight360();
}

VideoDecode::~VideoDecode() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	m_textureLeft.markForDelete();
	wgpuBindGroupRelease(m_bindGroupLeft);
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
		m_movieRight.togglePause();
	}

	if (keyboard.keyPressed(Keyboard::KEY_RIGHT)) {
		m_movieRight.seekTo(m_movieRight.getCurrentTime() + 5.0);
	}

	if (keyboard.keyPressed(Keyboard::KEY_LEFT)) {
		m_movieRight.seekTo(m_movieRight.getCurrentTime() - 5.0);
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
}

void VideoDecode::render() {
	wgpDraw();
}

WGPUTexture VideoDecode::createWebGpuTextureFromD3D12(WGPUDevice device, ID3D12Resource* d3d12Resource) {

	SharedTextureMemoryD3D12ResourceDescriptor d3d12Desc = {};
	d3d12Desc.chain.next = NULL;
	d3d12Desc.chain.sType = WGPUSType_SharedTextureMemoryD3D12ResourceDescriptor;
	d3d12Desc.resource = m_movieRight.m_d3d12Resource;

	WGPUSharedTextureMemoryDescriptor memoryDesc = {};
	memoryDesc.nextInChain = (WGPUChainedStruct*)&d3d12Desc;
	memoryDesc.label = WGPU_STR("FFmpeg Video Shared Memory");

	WGPUSharedTextureMemory sharedMemory = wgpuDeviceImportSharedTextureMemory(device, &memoryDesc);
	if (sharedMemory == NULL) {
		std::cout << "FEHLER: " << std::endl;
		return NULL;
	}

	WGPUTextureDescriptor textureDesc = {};
	textureDesc.nextInChain = NULL;
	textureDesc.label = WGPU_STR("FFmpeg Hardware Video Texture", );
	textureDesc.usage = WGPUTextureUsage_TextureBinding;
	textureDesc.dimension = WGPUTextureDimension::WGPUTextureDimension_2D;
	textureDesc.size.width = m_movieRight.m_width;
	textureDesc.size.height = m_movieRight.m_height;
	textureDesc.size.depthOrArrayLayers = 1u;
	textureDesc.format = WGPUTextureFormat_Undefined;
	textureDesc.mipLevelCount = 1u;
	textureDesc.sampleCount = 1u;
	textureDesc.viewFormatCount = 0u;
	textureDesc.viewFormats = NULL;

	WGPUTexture videoTexture = wgpuSharedTextureMemoryCreateTexture(sharedMemory, &textureDesc);
	wgpuSharedTextureMemoryRelease(sharedMemory);

	return videoTexture;
}

void VideoDecode::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {

	bool newFrameLeft = m_movieLeft.updateOpenAL(m_dt, m_pixelBufferLeft, m_audioBufferLeft);
	bool newFrameRight = false;
	m_hasActiveAccess = false;

	if (!m_isUserDraggingTimeline) {
		newFrameRight = m_movieRight.update(m_dt, m_pixelBufferRight, m_rtAudioPlayer.getRingBuffer());
	}

	if (!m_audioBufferLeft.empty())
		m_openALStreamLeft.streamAudio(m_audioBufferLeft);

	if (newFrameLeft) {
		uint32_t width = static_cast<uint32_t>(m_movieLeft.getWidth());
		uint32_t height = static_cast<uint32_t>(m_movieLeft.getHeight());

		WGPUTexelCopyTextureInfo destination = {};
		destination.texture = m_textureLeft.getTexture();
		destination.mipLevel = 0u;
		destination.origin = { 0u, 0u, 0u };
		destination.aspect = WGPUTextureAspect_All;

		WGPUTexelCopyBufferLayout source = {};
		source.offset = 0u;
		source.bytesPerRow = width * 4u;
		source.rowsPerImage = height;

		WGPUExtent3D size = { width, height, 1u };
		wgpuQueueWriteTexture(wgpContext.queue, &destination, m_pixelBufferLeft.data(), m_pixelBufferLeft.size(), &source, &size);
	}

	if (newFrameRight) {
		WGPUSharedTextureMemoryBeginAccessDescriptor accessDesc = {};
		accessDesc.nextInChain = NULL;
		accessDesc.initialized = true; // Textur enthält bereits die FFmpeg-Daten
		accessDesc.fenceCount = 0;
		accessDesc.fences = NULL;
		//accessDesc.fenceSignaledValues = NULL;

		WGPUStatus status = wgpuSharedTextureMemoryBeginAccess(m_movieRight.m_sharedTextureMemory, m_movieRight.m_videoTexture, &accessDesc);
		if (status == WGPUStatus_Success) {
			m_hasActiveAccess = true;
		}
		else {
			std::cerr << "Fehler: BeginAccess fehlgeschlagen!" << std::endl;
		}

		if (m_bindGroupRight) {
			wgpuBindGroupRelease(m_bindGroupRight);
		}

		std::vector<WGPUBindGroupEntry> entries(3);

		entries[0].binding = 0u;
		entries[0].buffer = m_cameraBuffer.getBuffer();
		entries[0].offset = 0u;
		entries[0].size = sizeof(CameraUniforms);

		entries[1].binding = 1u;
		entries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

		entries[2].binding = 2u;
		entries[2].textureView = m_movieRight.m_textureViewY;

		WGPUBindGroupDescriptor bindGroupDesc = {};
		bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_360"), 0u);
		bindGroupDesc.entryCount = (uint32_t)entries.size();
		bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
		m_bindGroupRight =  wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
	}

	float screenWidth = static_cast<float>(Application::Width);
	float screenHeight = static_cast<float>(Application::Height);
	float halfWidth = screenWidth / 2.0f;

	WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_VIDEO_2D"));

	// ==========================================
	// HÄLFTE 1: LINKES VIDEO RENDERN
	// ==========================================
	// Viewport auf die linke Bildschirmhälfte einschränken (von Pixel 0 bis halfWidth)
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, 0.0f, 0.0f, halfWidth, screenHeight, 0.0f, 1.0f);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupLeft, 0u, NULL);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 3u, 1u, 0u, 0u); // Zeichnet das Quad links gestaucht

	// ==========================================
	// HÄLFTE 2: RECHTES VIDEO RENDERN
	// ==========================================
	// Viewport auf die rechte Bildschirmhälfte verschieben (startet bei halfWidth)
	wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_VIDEO_360"));
	wgpuRenderPassEncoderSetViewport(renderPassEncoder, halfWidth, 0.0f, halfWidth, screenHeight, 0.0f, 1.0f);
	wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindGroupRight, 0u, NULL);
	wgpuRenderPassEncoderDraw(renderPassEncoder, 3u, 1u, 0u, 0u); // Zeichnet dasselbe Quad rechts gestaucht

	if (m_drawUi)
		renderUi(renderPassEncoder);

	wgpuRenderPassEncoderEnd(renderPassEncoder);
	wgpuRenderPassEncoderRelease(renderPassEncoder);

	
}

void VideoDecode::OnPostDraw() {
	if (m_hasActiveAccess) {
		WGPUSharedTextureMemoryEndAccessState endState = {};
		endState.nextInChain = NULL;

		wgpuSharedTextureMemoryEndAccess(m_movieRight.m_sharedTextureMemory, m_movieRight.m_videoTexture, &endState);
		if (m_movieRight.m_textureViewY) {
			wgpuTextureViewRelease(m_movieRight.m_textureViewY);
			m_movieRight.m_textureViewY = nullptr;
		}
		if (m_movieRight.m_videoTexture) {
			wgpuTextureRelease(m_movieRight.m_videoTexture);
			m_movieRight.m_videoTexture = nullptr;
		}
		if (m_movieRight.m_sharedTextureMemory) {
			wgpuSharedTextureMemoryRelease(m_movieRight.m_sharedTextureMemory);
			m_movieRight.m_sharedTextureMemory = nullptr;
		}
	}
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
	std::string buttonText = m_movieRight.isPaused() ? "Play" : "Pause";
	if (ImGui::Button(buttonText.c_str(), ImVec2(70, 0))) {
		m_movieRight.togglePause();
		m_movieLeft.togglePause();
	}

	ImGui::SameLine();

	double currentSec = m_movieRight.getCurrentTime();
	double totalSec = m_movieRight.getDuration();

	static float sliderTime = 0.0f;
	if (!m_isUserDraggingTimeline) {
		sliderTime = static_cast<float>(currentSec);
	}
	float maxTime = static_cast<float>(totalSec);

	// Zeit-Text formatieren
	int curMin = static_cast<int>(sliderTime) / 60;
	int curTimeSec = static_cast<int>(sliderTime) % 60;
	int totMin = static_cast<int>(totalSec) / 60;
	int totTimeSec = static_cast<int>(totalSec) % 60;

	char timeString[64];
	sprintf(timeString, "%02d:%02d / %02d:%02d", curMin, curTimeSec, totMin, totTimeSec);

	ImGui::PushItemWidth(-ImGui::GetContentRegionAvail().x * 0.25f);
	if (ImGui::SliderFloat("##Timeline", &sliderTime, 0.0f, maxTime, timeString)) {
		m_isUserDraggingTimeline = true;
	}

	if (m_isUserDraggingTimeline && ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
		m_movieRight.seekTo(static_cast<double>(sliderTime));
		m_isUserDraggingTimeline = false;
	}

	float volRight = m_rtAudioPlayer.getMixer().getVolume();
	if (ImGui::SliderFloat("Rechts (RtAudio)", &volRight, 0.0f, 1.0f, "%.2f")) {
		m_rtAudioPlayer.getMixer().setVolume(volRight);
	}

	ImGui::PopItemWidth();
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

WGPUBindGroup VideoDecode::createBindGroupLeft() {
	std::vector<WGPUBindGroupEntry> entries(2);

	entries[0].binding = 0u;
	entries[0].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	entries[1].binding = 1u;
	entries[1].textureView = m_textureLeft.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_2D"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup VideoDecode::createBindGroupRight() {
	std::vector<WGPUBindGroupEntry> entries(2);

	entries[0].binding = 0u;
	entries[0].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	entries[1].binding = 1u;
	entries[1].textureView = m_textureRight.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_2D"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup VideoDecode::createBindGroupRight360() {
	std::vector<WGPUBindGroupEntry> entries(3);

	entries[0].binding = 0u;
	entries[0].buffer = m_cameraBuffer.getBuffer();
	entries[0].offset = 0u;
	entries[0].size = sizeof(CameraUniforms);

	entries[1].binding = 1u;
	entries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	entries[2].binding = 2u;
	entries[2].textureView = m_textureRight.getTextureView();

	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_VIDEO_360"), 0u);
	bindGroupDesc.entryCount = (uint32_t)entries.size();
	bindGroupDesc.entries = (WGPUBindGroupEntry*)entries.data();
	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}