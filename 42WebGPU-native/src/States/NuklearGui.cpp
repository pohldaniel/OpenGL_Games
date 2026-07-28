#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "NuklearGui.h"
#include "Application.h"
#include "Globals.h"

NuklearGui::NuklearGui(StateMachine& machine) : State(machine, States::NUKLEAR_GUI) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	nkInit(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	nkInitFont("res/fonts/upheavtt.ttf");
	nkInitIcon("res/textures/ui-icons-buttons-set-blue.png");
	playIcon = nk_subimage_ptr(nkContext.bindgroupIcon, 960, 560, nk_rect(30.0f, 25.0f, 120.0f, 122.0f));

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f,  -1.0f, 1.0f);
	m_camera.lookAt(Vector3f(0.0f, 15.0f, -50.0f), Vector3f(0.0f, 15.0f, 0.0f), Vector3f(0.0f, 1.0f, 0.0f));
	m_camera.setMovingSpeed(50.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);

	wgpContext.setClearColor({ 0.2f, 0.2f, 0.2f, 1.0f });
	wgpContext.OnDraw = std::bind(&NuklearGui::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	nkContext.OnFillBuffer = std::bind(&NuklearGui::OnFillBuffer, this, std::placeholders::_1);
}

NuklearGui::~NuklearGui() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
	nkShutDown();
}

void NuklearGui::fixedUpdate() {

}

void NuklearGui::update() {

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

	nkUpdateInput(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT), Application::ScrollDelta);
}

void NuklearGui::render() {
	wgpDraw();
}

void NuklearGui::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	{
		WGPURenderPassColorAttachment renderPassColorAttachment = renderPassDescriptor.colorAttachments[0];
		renderPassColorAttachment.loadOp = WGPULoadOp::WGPULoadOp_Load;

		WGPURenderPassDescriptor rndrPssDscrptor = renderPassDescriptor;
		rndrPssDscrptor.colorAttachments = &renderPassColorAttachment;

		nkDraw(commandEncoder, rndrPssDscrptor);
	}
}

void NuklearGui::OnFillBuffer(nk_context& nkCntxt) {
	nkContext.font->handle.height = BASE_FONT_SIZE * m_uiScale;
	nkContext.font->scale = m_uiScale;

	if (m_initUi) {
		current_pos = nk_vec2(static_cast<float>(Application::Width) * 0.25f, static_cast<float>(Application::Height) * 0.25f);
		m_initUi = false;
	}

	struct nk_rect scaled_bounds = nk_rect(
		current_pos.x,
		current_pos.y,
		static_cast<float>(Application::Width) * 0.5f * m_uiScale,
		static_cast<float>(Application::Height) * 0.5f * m_uiScale

	);
	nk_window_set_bounds(&nkCntxt, "Nuklear Window", scaled_bounds);

	if (nk_begin(&nkCntxt, "Nuklear Window", scaled_bounds, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
		nk_layout_row_dynamic(&nkCntxt, BASE_ROW_DYN * m_uiScale, 1);
		if (nk_button_label(&nkCntxt, "Click Me!")) {
			printf("Button was pressed!\n");
		}

		nk_layout_row_static(&nkCntxt, BASE_ROW_STAT * m_uiScale, BASE_ROW_STAT * m_uiScale, 1);
		if (nk_button_image(&nkCntxt, playIcon)) {
			printf("Play was pressed!\n");
		}
		m_wasHovered = nk_window_is_hovered(&nkCntxt);
		current_pos = nk_window_get_position(&nkCntxt);
	}
	nk_end(&nkCntxt);

	nkContext.font->handle.height = BASE_FONT_SIZE;
	nkContext.font->scale = 1.0f;

	if (nk_begin(&nkCntxt, "Scroll", nk_rect(50, 50, 300, 200), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
		nk_layout_row_dynamic(&nkCntxt, 40, 1);
		for (int i = 0; i < 20; i++) {
			nk_labelf(&nkCntxt, NK_TEXT_LEFT, "Element %d", i);
		}
	}
	nk_end(&nkCntxt);

	struct nk_color old_background = nkContext.context.style.window.background;
	nkContext.context.style.window.background = nk_rgba(0, 0, 0, 0);

	struct nk_color old_border = nkContext.context.style.window.border_color;
	nkContext.context.style.window.border_color = nk_rgba(0, 0, 0, 0);

	
	nkContext.context.style.window.background = old_background;
	nkContext.context.style.window.border_color = old_border;
	
	m_isHovered = nk_window_is_any_hovered(&nkCntxt);
}

void NuklearGui::OnMouseMotion(const Event::MouseMoveEvent& event) {
	m_trackball.motion(event.x, event.y);
}

void NuklearGui::OnMouseButtonDown(const Event::MouseButtonEvent& event) {	
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT && !m_isHovered) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, true, event.x, event.y);
		Mouse::instance().detach();
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT && !m_isHovered)
		Mouse::instance().attach(Application::GetWindow(), true, true, true);
}

void NuklearGui::OnMouseButtonUp(const Event::MouseButtonEvent& event) {
	if (event.button == Event::MouseButtonEvent::BUTTON_LEFT && !m_isHovered) {
		m_trackball.mouse(TrackBall::Button::ELeftButton, TrackBall::Modifier::ENoModifier, false, event.x, event.y);
		Mouse::instance().attach(Application::GetWindow(), false, true);
	}

	if (event.button == Event::MouseButtonEvent::BUTTON_RIGHT && !m_isHovered)
		Mouse::instance().attach(Application::GetWindow(), false, false, true);
}

void NuklearGui::OnMouseWheel(const Event::MouseWheelEvent& event) {
	if (event.direction == 1u && m_wasHovered) {
		m_uiScale = m_uiScale - 0.05f;
		m_uiScale = Math::Clamp(m_uiScale, 0.0f, 5.0f);
	}

	if (event.direction == 0u && m_wasHovered) {
		m_uiScale = m_uiScale + 0.05f;
		m_uiScale = Math::Clamp(m_uiScale, 0.0f, 5.0f);
	}
}

void NuklearGui::OnKeyDown(const Event::KeyboardEvent& event) {
#if DEVBUILD
	if (event.keyCode == VK_LMENU) {
		m_drawUi = !m_drawUi;
	}
#endif

	if (event.keyCode == VK_ESCAPE) {
		m_isRunning = false;
	}
}

void NuklearGui::OnKeyUp(const Event::KeyboardEvent& event) {

}

void NuklearGui::resize(int deltaW, int deltaH) {
	nkResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 1000.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);	
}

void NuklearGui::renderUi(const WGPURenderPassEncoder& renderPassEncoder) {
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