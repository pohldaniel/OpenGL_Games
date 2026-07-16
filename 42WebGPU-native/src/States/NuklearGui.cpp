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

	nkInit();
	nkInitFont("res/fonts/upheavtt.ttf");
	nkInitIcon("res/textures/ui-icons-buttons-set-blue.png");
	playIcon = nk_subimage_ptr(nkContext.bindgroupIcon, 960, 560, nk_rect(30.0f, 25.0f, 120.0f, 122.0f));


	m_player.loadModelAssimp("res/models/player.fbx");
	m_animation.loadAnimationAssimp("res/models/player.fbx", "Player", "full");

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f,  -1.0f, 1.0f);
	m_camera.lookAt(4.0f, 0.1f * 180.0f, 0.0f, 0.1f * 180.0f);
	m_camera.setMovingSpeed(5.0f);
	m_camera.setRotationSpeed(0.1f);

	m_trackball.reshape(Application::Width, Application::Height);
	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
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

	nkUpdateInput(mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT), m_scrollDelta);
	m_scrollDelta = 0.0f;
}

void NuklearGui::render() {
	wgpDraw();
}

void NuklearGui::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	{
		nkDraw(commandEncoder, renderPassDescriptor);
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

	nk_style_push_style_item(&nkCntxt, &nkContext.context.style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));
	nk_style_push_color(&nkCntxt, &nkContext.context.style.window.background, nk_rgba(0, 0, 0, 0));
	nk_style_push_color(&nkCntxt, &nkContext.context.style.window.border_color, nk_rgba(0, 0, 0, 0));

	nk_style_push_vec2(&nkCntxt, &nkContext.context.style.window.padding, nk_vec2(0, 0));
	nk_style_push_vec2(&nkCntxt, &nkContext.context.style.window.group_padding, nk_vec2(0, 0));
	nk_style_push_vec2(&nkCntxt, &nkContext.context.style.window.spacing, nk_vec2(0, 0));
	nk_style_push_float(&nkCntxt, &nkContext.context.style.window.border, 0.0f);


	if (nk_begin(&nkCntxt, "HUD_Controls", nk_rect(20, static_cast<float>(Application::Height) - 200, 180, 180), NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR))
	{

		float joystick_size = 180.0f;
		nk_layout_row_static(&nkCntxt, joystick_size, joystick_size, 1);

		JoystickResult input_vector = nk_virtual_joystick(&nkCntxt, joystick_size);

		if (input_vector.is_active) {

		}
	}
	nk_end(&nkCntxt);

	nk_style_pop_float(&nkCntxt);
	nk_style_pop_vec2(&nkCntxt);
	nk_style_pop_vec2(&nkCntxt);
	nk_style_pop_vec2(&nkCntxt);

	nk_style_pop_color(&nkCntxt);
	nk_style_pop_color(&nkCntxt);
	nk_style_pop_style_item(&nkCntxt);
	nkContext.context.style.window.background = old_background;
	nkContext.context.style.window.border_color = old_border;

	nk_style_push_vec2(&nkCntxt, &nkContext.context.style.window.padding, nk_vec2(0, 0));
	nk_style_push_vec2(&nkCntxt, &nkContext.context.style.window.group_padding, nk_vec2(0, 0));
	nk_style_push_vec2(&nkCntxt, &nkContext.context.style.window.spacing, nk_vec2(0, 0));
	nk_style_push_float(&nkCntxt, &nkContext.context.style.window.border, 0.0f);

	nk_style_push_style_item(&nkCntxt, &nkContext.context.style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));
	nk_style_push_color(&nkCntxt, &nkContext.context.style.window.background, nk_rgba(0, 0, 0, 0));
	nk_style_push_color(&nkCntxt, &nkContext.context.style.window.border_color, nk_rgba(0, 0, 0, 0));

	float btn_box_size = 140.0f;
	float margin = 40.0f;

	float btn_x = static_cast<float>(Application::Width) - btn_box_size - margin;
	float btn_y = static_cast<float>(Application::Height) - btn_box_size - margin;

	if (nk_begin(&nkCntxt, "HUD_ActionButton", nk_rect(btn_x, btn_y, btn_box_size, btn_box_size), NK_WINDOW_NO_SCROLLBAR)) {

		nk_layout_row_static(&nkCntxt, btn_box_size, btn_box_size, 1);

		if (nk_circular_action_button(&nkCntxt, "A", btn_box_size)) {

		}
	}
	nk_end(&nkCntxt);


	nk_style_pop_color(&nkCntxt);
	nk_style_pop_color(&nkCntxt);
	nk_style_pop_style_item(&nkCntxt);
	nk_style_pop_float(&nkCntxt);
	nk_style_pop_vec2(&nkCntxt);
	nk_style_pop_vec2(&nkCntxt);
	nk_style_pop_vec2(&nkCntxt);

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
	m_scrollDelta = event.delta;
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
	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f, -1.0f, 1.0f);
	m_trackball.reshape(Application::Width, Application::Height);
	nkResize(static_cast<float>(Application::Width), static_cast<float>(Application::Height));
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

NuklearGui::JoystickResult NuklearGui::nk_virtual_joystick(struct nk_context* ctx, float size_px) {
	JoystickResult result;

	// 1. Platz im aktuellen Nuklear-Layout reservieren (quadratisch)
	struct nk_rect bounds;
	nk_widget(&bounds, ctx);

	// 2. Mathematische Zentren und Radien berechnen
	float radius_base = bounds.w / 2.0f;
	float radius_stick = radius_base * 0.4f; // Stick ist 40% der Basisgröße
	struct nk_vec2 center = nk_vec2(bounds.x + radius_base, bounds.y + radius_base);

	// 3. Touch/Maus-Input auswerten
	const struct nk_input* input = &ctx->input;
	// Android NDK mappt Screen-Touch meistens auf die primäre Maus-Taste
	bool is_touched = input->mouse.buttons[NK_BUTTON_LEFT].down;
	struct nk_vec2 touch_pos = input->mouse.pos;

	struct nk_vec2 stick_pos = center;

	if (is_touched) {
		// Prüfen, ob der Touch innerhalb des Widgets gestartet ist oder gehalten wird
		if (nk_input_is_mouse_hovering_rect(input, bounds) || ctx->active == ctx->current) {
			result.is_active = true;

			// Vektor vom Zentrum zum Touch-Punkt berechnen
			float dx = touch_pos.x - center.x;
			float dy = touch_pos.y - center.y;
			float distance = std::sqrt(dx * dx + dy * dy);

			// Maximaler physikalischer Bewegungsweg für den Stick
			float max_distance = radius_base - radius_stick;

			if (distance > 0.0f) {
				// Normalisierter Richtungsvektor
				float nx = dx / distance;
				float ny = dy / distance;

				// Wenn der Finger weiter zieht als die Basis groß ist, begrenzen (Clamp)
				float clamped_dist = (distance > max_distance) ? max_distance : distance;

				stick_pos.x = center.x + nx * clamped_dist;
				stick_pos.y = center.y + ny * clamped_dist;

				// Das Endergebnis für dein Spiel/deine Engine berechnen (-1.0 bis 1.0)
				result.x = (nx * clamped_dist) / max_distance;
				// Android-Y zeigt nach unten, für Spiele-Engines invertieren wir oft Y
				result.y = -((ny * clamped_dist) / max_distance);
			}
		}
	}

	// 4. Zeichnen über das Nuklear-Command-Buffer (wird an WebGPU übergeben)
	struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);
	if (canvas) {
		// Basis zeichnen (Transparenter Kreis mit Rand)
		nk_fill_circle(canvas, nk_rect(bounds.x, bounds.y, bounds.w, bounds.h), nk_rgba(50, 50, 50, 150));
		nk_stroke_circle(canvas, nk_rect(bounds.x, bounds.y, bounds.w, bounds.h), 2.0f, nk_rgb(200, 200, 200));

		// Stick/Knopf zeichnen (Gefüllter Kreis an berechneter Stick-Position)
		float sx = stick_pos.x - radius_stick;
		float sy = stick_pos.y - radius_stick;
		float sw = radius_stick * 2.0f;
		nk_fill_circle(canvas, nk_rect(sx, sy, sw, sw), nk_rgb(255, 100, 100));
	}

	return result;
}

bool NuklearGui::nk_circular_action_button(struct nk_context* ctx, const char* label, float size_px) {
	bool is_pressed = false;

	// 1. Platz im Layout reservieren (quadratisch)
	struct nk_rect bounds;
	nk_widget(&bounds, ctx);

	float radius = bounds.w / 2.0f;
	struct nk_vec2 center = nk_vec2(bounds.x + radius, bounds.y + radius);

	const struct nk_input* input = &ctx->input;
	bool is_touched = input->mouse.buttons[NK_BUTTON_LEFT].down;
	struct nk_vec2 touch_pos = input->mouse.pos;


	// Wir prüfen mathematisch, ob der Touch innerhalb des Kreises liegt
	bool is_hovered = false;
	if (is_touched) {
		float dx = touch_pos.x - center.x;
		float dy = touch_pos.y - center.y;
		if ((dx * dx + dy * dy) <= (radius * radius)) {
			is_hovered = true;
		}
	}

	// Zustand für "Geklickt" im Immediate-Mode ermitteln
	static bool button_was_down = false;
	if (is_hovered && is_touched) {
		if (!button_was_down) {
			is_pressed = true; // Genau in diesem Frame gedrückt
			button_was_down = true;
		}
	}
	else if (!is_touched) {
		button_was_down = false; // Finger wieder angehoben
	}

	// 3. Zeichnen auf dem Canvas
	struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);
	if (canvas) {
		// Farb-Feedback: Wenn gedrückt, dunkler färben
		struct nk_color btn_color = is_hovered ? nk_rgb(180, 50, 50) : nk_rgb(255, 80, 80);
		struct nk_color border_color = is_hovered ? nk_rgb(255, 255, 255) : nk_rgb(200, 200, 200);

		// Kreis füllen und umranden
		nk_fill_circle(canvas, bounds, btn_color);
		nk_stroke_circle(canvas, bounds, 3.0f, border_color);

		// Text in der Mitte des Kreises platzieren
		// (Nutzt die aktuelle Font-Höhe, um den Text vertikal zu zentrieren)
		const struct nk_user_font* font = ctx->style.font;
		float text_width = font->width(font->userdata, font->height, label, nk_strlen(label));

		struct nk_vec2 text_pos;
		text_pos.x = center.x - (text_width / 2.0f);
		text_pos.y = center.y - (font->height / 2.0f);

		nk_draw_text(canvas, nk_rect(text_pos.x, text_pos.y, text_width, font->height),
			label, nk_strlen(label), font, nk_rgb(0, 0, 0), nk_rgb(255, 255, 255));
	}

	return is_pressed;
}