#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>

#include <Nuklear/NkJoystick.h>
#include <Nuklear/NkStyle.h>

#include <engine/sound/SoundDevice.h>

#include "AudioDecode.h"
#include "Application.h"
#include "Globals.h"

AudioDecode::AudioDecode(StateMachine& machine) : State(machine, States::AUDIO_DECODE) {
	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	nkInit(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	nkInitFont("res/fonts/upheavtt.ttf");

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 15.0f, -50.0f), Vector3f(0.0f, 15.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(50.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);

	wgpContext.setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	wgpContext.OnDraw = std::bind(&AudioDecode::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	nkContext.OnFillBuffer = std::bind(&AudioDecode::OnFillBuffer, this, std::placeholders::_1);
	m_audioDecoder.init<RtAudioPlayer>();

	btn_w = 300.0f;
	btn_h = 85.0f;
	spacing = 40.0f;
	start_x = (static_cast<float>(Application::Width) - btn_w) / 2.0f;
	total_block_h = (3.0f * btn_h) + (2.0f * spacing);
	start_y = (static_cast<float>(Application::Height) - total_block_h) / 2.0f;

	ctrl_size = 80.0f;
	side_padding = 50.0f;

	bottom_margin = 50.0f;
	ctrl_y = static_cast<float>(Application::Height) - ctrl_size - bottom_margin;
	pause_x = side_padding;
	play_x = static_cast<float>(Application::Width) - ctrl_size * 1.5f - side_padding;
}

AudioDecode::~AudioDecode() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	nkShutDown();
}

void AudioDecode::fixedUpdate() {

}

void AudioDecode::update() {
	m_audioDecoder.update();

	Keyboard& keyboard = Keyboard::instance();
	Mouse& mouse = Mouse::instance();

	nkUpdateInput(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT), mouse.buttonDown(Mouse::MouseButton::BUTTON_RIGHT), Application::ScrollDelta);

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

	if (keyboard.keyPressed(Keyboard::KEY_Z)) {
		Physics::DebugDrawer.toggleWireframe();
	}

	

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
}

void AudioDecode::render() {
	wgpDraw();
}

void AudioDecode::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	nkDraw(commandEncoder, renderPassDescriptor);
}

void AudioDecode::OnFillBuffer(nk_context& nkCntxt) {

	start_x = (static_cast<float>(Application::Width) - btn_w) / 2.0f;
	total_block_h = (3.0f * btn_h) + (2.0f * spacing);
	start_y = (static_cast<float>(Application::Height) - total_block_h) / 2.0f;

	ctrl_y = static_cast<float>(Application::Height) - ctrl_size - bottom_margin;
	pause_x = side_padding;
	play_x = static_cast<float>(Application::Width) - (ctrl_size * 1.5f) - side_padding;

	set_transparent_window_style();
	float y = start_y;
	if (rounded_button(nk_rect(start_x, y, btn_w, btn_h), "Ambient", m_isPressed)) {
		if (m_currentSong != 1) {			
			m_audioDecoder.playTrack("res/sounds/ambient.mp3");
			m_currentSong = 1;
		}
	}

	y = start_y + btn_h + spacing;
	if (rounded_button(nk_rect(start_x, y, btn_w, btn_h), "Paradise Found", m_isPressed)) {
		if (m_currentSong != 2) {			
			m_audioDecoder.playTrack("res/sounds/paradise_found.mp3");
			m_currentSong = 2;
		}
	}

	y = start_y + (btn_h + spacing) * 2.0f;
	if (rounded_button(nk_rect(start_x, y, btn_w, btn_h), "Screen Saver", m_isPressed)) {
		if (m_currentSong != 3) {
			m_audioDecoder.playTrack("res/sounds/screen_saver.mp3");
			m_currentSong = 3;
		}
	}

	if (rounded_button(nk_rect(play_x, ctrl_y, ctrl_size * 1.5f, ctrl_size), "PLAY", m_isPressed)) {
		m_audioDecoder.play();
	}

	if (rounded_button(nk_rect(pause_x, ctrl_y, ctrl_size * 1.5f, ctrl_size), "PAUSE", m_isPressed)) {
		m_audioDecoder.pause();
	}
	reset_transparent_window_style();
}

void AudioDecode::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void AudioDecode::OnMouseButtonDown(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void AudioDecode::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void AudioDecode::OnMouseWheel(const Event::MouseWheelEvent& event) {

}

void AudioDecode::OnKeyDown(const Event::KeyboardEvent& event) {

}

void AudioDecode::OnKeyUp(const Event::KeyboardEvent& event) {

}

void AudioDecode::resize(int deltaW, int deltaH) {
	nkResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), 0.0f, static_cast<float>(Application::Height), -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);
}

void AudioDecode::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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