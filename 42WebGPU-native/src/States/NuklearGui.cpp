#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define  NK_INCLUDE_STANDARD_VARARGS

#include <nuklear.h>

#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_wgpu.h>
#include <imgui_internal.h>

#include <WebGPU/WgpContext.h>
#include <WebGPU/WgpRenderer.h>

#include "NuklearGui.h"
#include "Application.h"
#include "Globals.h"

struct nk_context ctx;
//struct nk_user_font font;
struct nk_buffer commands;
struct nk_convert_config config;
static uint8_t cpu_vertex_linear_buffer[MAX_VERTEX_MEMORY];
static uint8_t cpu_index_linear_buffer[MAX_INDEX_MEMORY];
const struct nk_draw_command* cmd = NULL;
struct nk_buffer vbuf, ibuf;
struct nk_font_atlas atlas;
struct nk_font* default_font;
struct nk_font* custom_font;
struct nk_image playIcon;
struct nk_vec2 current_pos;

const float BASE_ROW_DYN = 30.0f;  // Basis-Höhe für den dynamischen Button
const float BASE_ROW_STAT = 32.0f; // Basis-Größe für den statischen Button
const float BASE_FONT_SIZE = 16.0f;

NuklearGui::NuklearGui(StateMachine& machine) : State(machine, States::NUKLEAR_GUI) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);
	Mouse::instance().attach(Application::GetWindow(), false, true);

	wgpSetSurfaceColorFormat(WGPUTextureFormat::WGPUTextureFormat_BGRA8Unorm, Application::OnSurfaceChange);
	wgpSetSurfaceDepthFormat(WGPUTextureFormat::WGPUTextureFormat_Depth24Plus, Application::OnSurfaceChange);

	m_camera.perspective(72.0f, static_cast<float>(Application::Width) / static_cast<float>(Application::Height), 0.1f, 100.0f);
	m_camera.orthographic(0.0f, static_cast<float>(Application::Width), static_cast<float>(Application::Height), 0.0f,  -1.0f, 1.0f);
	m_camera.lookAt(4.0f, 0.1f * 180.0f, 0.0f, 0.1f * 180.0f);
	m_camera.setMovingSpeed(5.0f);
	m_camera.setRotationSpeed(0.1f);

	m_vertexBuffer.createBuffer(MAX_VERTEX_MEMORY, WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst);
	m_indexBuffer.createBuffer(MAX_INDEX_MEMORY, WGPUBufferUsage_Index | WGPUBufferUsage_CopyDst);
	m_texture.createEmpty(1u, 1u, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat_RGBA8Unorm);
	m_textureIcon.loadFromFile("res/textures/ui-icons-buttons-set-blue.png", true);

	uint8_t white_pixel[4] = { 255, 255, 255, 255 };

	WGPUTexelCopyTextureInfo destination = {};
	destination.texture = m_texture.getTexture();
	destination.mipLevel = 0u;
	destination.origin = { 0u, 0u, 0u };
	destination.aspect = WGPUTextureAspect_All;

	WGPUTexelCopyBufferLayout source = {};
	source.offset = 0u;
	source.bytesPerRow = 4u;
	source.rowsPerImage = 1u;

	WGPUExtent3D size = { 1u , 1u , 1u };
	wgpuQueueWriteTexture(wgpContext.queue, &destination, white_pixel, 4u, &source, &size);

	memset(&config, 0, sizeof(config));
	config.shape_AA = NK_ANTI_ALIASING_ON;
	config.line_AA = NK_ANTI_ALIASING_ON;
	config.circle_segment_count = 22;
	config.curve_segment_count = 22;
	config.arc_segment_count = 22;
	config.global_alpha = 1.0f;
	config.null.texture.ptr = m_texture.getTextureView();
	config.null.uv = nk_vec2(0.5f, 0.5f);

	static const struct nk_draw_vertex_layout_element vertex_layout[] = {
		{NK_VERTEX_POSITION, NK_FORMAT_FLOAT, 0},
		{NK_VERTEX_TEXCOORD, NK_FORMAT_FLOAT, 2 * sizeof(float)},
		{NK_VERTEX_COLOR, NK_FORMAT_R8G8B8A8, 4 * sizeof(float)},
		{NK_VERTEX_LAYOUT_END}
	};

	config.vertex_layout = vertex_layout;
	config.vertex_size = 20;
	config.vertex_alignment = 4;

	nk_init_default(&ctx, NULL);
	nk_buffer_init_default(&commands);
	nk_buffer_init_fixed(&vbuf, cpu_vertex_linear_buffer, MAX_VERTEX_MEMORY);
	nk_buffer_init_fixed(&ibuf, cpu_index_linear_buffer, MAX_INDEX_MEMORY);

	nk_font_atlas_init_default(&atlas);
	nk_font_atlas_begin(&atlas);

	//default_font = nk_font_atlas_add_default(&atlas, BASE_FONT_SIZE, NULL);

	struct nk_font_config config_font = nk_font_config(0.0f);
	config_font.oversample_h = 3;
	config_font.oversample_v = 3;
	custom_font = nk_font_atlas_add_from_file(&atlas, "res/fonts/upheavtt.ttf", BASE_FONT_SIZE, &config_font);

	const void* image_pixels;
	int atlas_width, atlas_height;
	image_pixels = nk_font_atlas_bake(&atlas, &atlas_width, &atlas_height, NK_FONT_ATLAS_RGBA32);

	m_textureFont.createEmpty(atlas_width, atlas_height, 1u, WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst, WGPUTextureFormat_RGBA8Unorm);

	WGPUTexelCopyTextureInfo font_destination = {};
	font_destination.texture = m_textureFont.getTexture();
	font_destination.mipLevel = 0u;
	font_destination.origin = { 0u, 0u, 0u };
	font_destination.aspect = WGPUTextureAspect_All;

	WGPUTexelCopyBufferLayout font_source = {};
	font_source.offset = 0u;
	font_source.bytesPerRow = (uint32_t)atlas_width * 4;
	font_source.rowsPerImage = (uint32_t)atlas_height;

	WGPUExtent3D font_size = { (uint32_t)atlas_width, (uint32_t)atlas_height, 1u };
	wgpuQueueWriteTexture(wgpContext.queue, &font_destination, image_pixels, (size_t)(atlas_width * atlas_height * 4), &font_source, &font_size);

	nk_handle font_handle;
	font_handle.ptr = m_textureFont.getTextureView();

	nk_font_atlas_end(&atlas, font_handle, &config.null);
	nk_style_set_font(&ctx, &custom_font->handle);
	nk_style_default(&ctx);

	playIcon = nk_subimage_ptr(m_textureIcon.getTextureView(), 960, 560, nk_rect(30.0f, 25.0f, 120.0f, 122.0f));

	wgpContext.addSahderModule("NUKLEAR", "res/shader/nuklear.wgsl");
	wgpContext.createRenderPipeline("NUKLEAR", "RP_NUKLEAR", VL_GUI, std::bind(&NuklearGui::OnBindGroupLayouts, this), 1u,
		WGPUPrimitiveTopology_TriangleList,
		WGPUTextureFormat_Undefined,
		WGPUTextureFormat_Undefined,
		WGPUCompareFunction_Always,
		{ DEPTH_STENCIL_STATE | BLEND_STATE | FRAGMENT_STATE, BlendMode::ALPHA_BLENDING, WGPUTextureFormat_Undefined, WGPUCullMode_None });

	m_trackball.reshape(Application::Width, Application::Height);

	m_uniformBuffer.createBuffer(sizeof(Matrix4f), WGPUBufferUsage_CopyDst | WGPUBufferUsage_Uniform);
	wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_camera.getOrthographicMatrix(), sizeof(Matrix4f));

	wgpContext.setClearColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	
	wgpContext.OnDraw = std::bind(&NuklearGui::OnDraw, this, std::placeholders::_1, std::placeholders::_2);
	m_bindgroup = createBindGroup();
	m_bindgroupFont = createBindGroupFont();
	m_bindgroupIcon = createBindGroupIcon();
}

NuklearGui::~NuklearGui() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
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

	nk_input_begin(&ctx);	
	nk_input_motion(&ctx, mouse.xPos(), mouse.yPos());
	nk_input_button(&ctx, NK_BUTTON_LEFT, mouse.xPos(), mouse.yPos(), mouse.buttonDown(Mouse::MouseButton::BUTTON_LEFT));
	
	if (m_wasHovered1)
		nk_input_scroll(&ctx, nk_vec2(0.0f, m_scrollDelta));
	nk_input_end(&ctx);	
	
	m_scrollDelta = 0.0f;
	m_isHovered = nk_window_is_any_hovered(&ctx);
}

void NuklearGui::render() {
	wgpDraw();
}

void NuklearGui::OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor) {
	{
		wgpuQueueWriteBuffer(wgpContext.queue, m_uniformBuffer.getBuffer(), 0u, &m_camera.getOrthographicMatrix(), sizeof(Matrix4f));

		custom_font->handle.height = BASE_FONT_SIZE * m_uiScale;
		custom_font->scale = m_uiScale;

		if(m_initUi)
			current_pos = nk_vec2(static_cast<float>(Application::Width) * 0.25f, static_cast<float>(Application::Height) * 0.25f);

		struct nk_rect scaled_bounds = nk_rect(
			current_pos.x,
			current_pos.y,
			static_cast<float>(Application::Width) * 0.5f * m_uiScale,
			static_cast<float>(Application::Height) * 0.5f * m_uiScale
			
		);
		nk_window_set_bounds(&ctx, "Nuklear Window", scaled_bounds);

		if (nk_begin(&ctx, "Nuklear Window", scaled_bounds, NK_WINDOW_BORDER | NK_WINDOW_MOVABLE | NK_WINDOW_TITLE)) {
			nk_layout_row_dynamic(&ctx, BASE_ROW_DYN * m_uiScale, 1);
			if (nk_button_label(&ctx, "Click Me!")) {
				printf("Button was pressed!\n");
			}

			nk_layout_row_static(&ctx, BASE_ROW_STAT * m_uiScale, BASE_ROW_STAT * m_uiScale, 1);
			if (nk_button_image(&ctx, playIcon)) {
				printf("Play was pressed!\n");
			}
			m_wasHovered2 = nk_window_is_hovered(&ctx);
			current_pos = nk_window_get_position(&ctx);
		}
		nk_end(&ctx);

		custom_font->handle.height = BASE_FONT_SIZE;
		custom_font->scale = 1.0f;

		if (nk_begin(&ctx, "Scroll", nk_rect(50, 50, 300, 200), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE)) {
			nk_layout_row_dynamic(&ctx, 40, 1);
			for (int i = 0; i < 20; i++) {
				nk_labelf(&ctx, NK_TEXT_LEFT, "Element %d", i);
			}
			m_wasHovered1 = nk_window_is_hovered(&ctx);
		}
		nk_end(&ctx);

		struct nk_color old_background = ctx.style.window.background;
		ctx.style.window.background = nk_rgba(0, 0, 0, 0);

		struct nk_color old_border = ctx.style.window.border_color;
		ctx.style.window.border_color = nk_rgba(0, 0, 0, 0);

		nk_style_push_style_item(&ctx, &ctx.style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));
		nk_style_push_color(&ctx, &ctx.style.window.background, nk_rgba(0, 0, 0, 0));
		nk_style_push_color(&ctx, &ctx.style.window.border_color, nk_rgba(0, 0, 0, 0));

		nk_style_push_vec2(&ctx, &ctx.style.window.padding, nk_vec2(0, 0));
		nk_style_push_vec2(&ctx, &ctx.style.window.group_padding, nk_vec2(0, 0));
		nk_style_push_vec2(&ctx, &ctx.style.window.spacing, nk_vec2(0, 0));
		nk_style_push_float(&ctx, &ctx.style.window.border, 0.0f);


		if (nk_begin(&ctx, "HUD_Controls", nk_rect(20, static_cast<float>(Application::Height) - 200 * m_uiScale, 180 * m_uiScale, 180 * m_uiScale),
			NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) // Keine Ränder, reines Overlay
		{
			// Quadratische Zeile passend zur UI-Skalierung
			float joystick_size = 180.0f * m_uiScale;
			nk_layout_row_static(&ctx, joystick_size, joystick_size, 1);

			// Unser Custom Widget aufrufen
			JoystickResult input_vector = nk_virtual_joystick(&ctx, joystick_size);

			if (input_vector.is_active) {
				// Werte direkt an deine Spiellogik übergeben!
				//my_player.move(input_vector.x, input_vector.y);
			}
		}
		nk_end(&ctx);

		nk_style_pop_float(&ctx);
		nk_style_pop_vec2(&ctx);
		nk_style_pop_vec2(&ctx);
		nk_style_pop_vec2(&ctx);

		nk_style_pop_color(&ctx);
		nk_style_pop_color(&ctx);
		nk_style_pop_style_item(&ctx);
		ctx.style.window.background = old_background;
		ctx.style.window.border_color = old_border;

		nk_style_push_vec2(&ctx, &ctx.style.window.padding, nk_vec2(0, 0));
		nk_style_push_vec2(&ctx, &ctx.style.window.group_padding, nk_vec2(0, 0));
		nk_style_push_vec2(&ctx, &ctx.style.window.spacing, nk_vec2(0, 0));
		nk_style_push_float(&ctx, &ctx.style.window.border, 0.0f);

		nk_style_push_style_item(&ctx, &ctx.style.window.fixed_background, nk_style_item_color(nk_rgba(0, 0, 0, 0)));
		nk_style_push_color(&ctx, &ctx.style.window.background, nk_rgba(0, 0, 0, 0));
		nk_style_push_color(&ctx, &ctx.style.window.border_color, nk_rgba(0, 0, 0, 0));

		float btn_box_size = 140.0f * m_uiScale;
		float margin = 40.0f * m_uiScale;

		float btn_x = static_cast<float>(Application::Width) - btn_box_size - margin;
		float btn_y = static_cast<float>(Application::Height) - btn_box_size - margin;

		if (nk_begin(&ctx, "HUD_ActionButton", nk_rect(btn_x, btn_y, btn_box_size, btn_box_size), NK_WINDOW_NO_SCROLLBAR)) {

			nk_layout_row_static(&ctx, btn_box_size, btn_box_size, 1);

			// Button aufrufen
			if (nk_circular_action_button(&ctx, "A", btn_box_size)) {
				// ECHTES ACTION-EVENT AUSLÖSEN!
				//my_player.jump_or_attack();
			}
		}
		nk_end(&ctx);


		nk_style_pop_color(&ctx);
		nk_style_pop_color(&ctx);
		nk_style_pop_style_item(&ctx);
		nk_style_pop_float(&ctx);
		nk_style_pop_vec2(&ctx);
		nk_style_pop_vec2(&ctx);
		nk_style_pop_vec2(&ctx);

		nk_buffer_clear(&vbuf);
		nk_buffer_clear(&ibuf);

		nk_convert(&ctx, &commands, &vbuf, &ibuf, &config);
		m_initUi = false;

		uint32_t vertex_count = vbuf.needed / sizeof(nk_webgpu_vertex);
		nk_webgpu_vertex* vertices = (nk_webgpu_vertex*)cpu_vertex_linear_buffer;
		ibuf.needed = (ibuf.needed + 3) & ~3;
		wgpuQueueWriteBuffer(wgpContext.queue, m_vertexBuffer.getBuffer(), 0, cpu_vertex_linear_buffer, vbuf.needed);
		wgpuQueueWriteBuffer(wgpContext.queue, m_indexBuffer.getBuffer(), 0, cpu_index_linear_buffer, ibuf.needed);

		WGPURenderPassEncoder renderPassEncoder = wgpuCommandEncoderBeginRenderPass(commandEncoder, &renderPassDescriptor);
		wgpuRenderPassEncoderSetPipeline(renderPassEncoder, wgpContext.renderPipelines.at("RP_NUKLEAR"));

		wgpuRenderPassEncoderSetVertexBuffer(renderPassEncoder, 0, m_vertexBuffer.getBuffer(), 0, MAX_VERTEX_MEMORY);
		wgpuRenderPassEncoderSetIndexBuffer(renderPassEncoder, m_indexBuffer.getBuffer(), WGPUIndexFormat_Uint16, 0, MAX_INDEX_MEMORY);

		uint32_t index_offset = 0;
		nk_draw_foreach(cmd, &ctx, &commands) {
			if (!cmd->elem_count) continue;
			WGPUTextureView active_view = (WGPUTextureView)cmd->texture.ptr;
			if (active_view == m_textureFont.getTextureView()) {
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindgroupFont, 0u, NULL);
			}else if (active_view == m_textureIcon.getTextureView()) {
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindgroupIcon, 0u, NULL);
			}else {
				wgpuRenderPassEncoderSetBindGroup(renderPassEncoder, 0u, m_bindgroup, 0u, NULL);
			}

			float scissor_x = cmd->clip_rect.x < 0.0f ? 0.0f : cmd->clip_rect.x;
			float scissor_y = cmd->clip_rect.y < 0.0f ? 0.0f : cmd->clip_rect.y;
			float scissor_w = cmd->clip_rect.w;
			float scissor_h = cmd->clip_rect.h;

			if (scissor_x + scissor_w > static_cast<float>(Application::Width)) {
				scissor_w = static_cast<float>(Application::Width) - scissor_x;
			}

			if (scissor_y + scissor_h > static_cast<float>(Application::Height)) {
				scissor_h = static_cast<float>(Application::Height) - scissor_y;
			}

			if (scissor_w <= 0.0f || scissor_h <= 0.0f) {
				continue;
			}

			wgpuRenderPassEncoderSetScissorRect(renderPassEncoder, (uint32_t)scissor_x, (uint32_t)scissor_y, (uint32_t)scissor_w, (uint32_t)scissor_h);
			wgpuRenderPassEncoderDrawIndexed(renderPassEncoder, cmd->elem_count, 1, index_offset, 0, 0);
			index_offset += cmd->elem_count;
		}
		wgpuRenderPassEncoderEnd(renderPassEncoder);
		wgpuRenderPassEncoderRelease(renderPassEncoder);
		nk_clear(&ctx);
		nk_buffer_clear(&commands);
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
	if (event.direction == 1u && m_wasHovered2) {
		m_uiScale = m_uiScale - 0.05f;
		m_uiScale = Math::Clamp(m_uiScale, 0.0f, 5.0f);
	}

	if (event.direction == 0u && m_wasHovered2) {
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

std::vector<WGPUBindGroupLayout> NuklearGui::OnBindGroupLayouts() {
	std::vector<WGPUBindGroupLayout> bindingLayouts(1);

	std::vector<WGPUBindGroupLayoutEntry> bindingLayoutEntries(3);

	bindingLayoutEntries[0].binding = 0u;
	bindingLayoutEntries[0].visibility = WGPUShaderStage_Vertex;
	bindingLayoutEntries[0].buffer.type = WGPUBufferBindingType_Uniform;
	bindingLayoutEntries[0].buffer.minBindingSize = sizeof(Matrix4f);

	bindingLayoutEntries[1].binding = 1u;
	bindingLayoutEntries[1].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[1].sampler.type = WGPUSamplerBindingType_Filtering;

	bindingLayoutEntries[2].binding = 2u;
	bindingLayoutEntries[2].visibility = WGPUShaderStage_Fragment;
	bindingLayoutEntries[2].texture.sampleType = WGPUTextureSampleType_Float;
	bindingLayoutEntries[2].texture.viewDimension = WGPUTextureViewDimension_2D;
	bindingLayoutEntries[2].texture.multisampled = WGPU_FALSE;

	WGPUBindGroupLayoutDescriptor bindGroupLayoutDescriptor = {};
	bindGroupLayoutDescriptor.entryCount = (uint32_t)bindingLayoutEntries.size();
	bindGroupLayoutDescriptor.entries = bindingLayoutEntries.data();

	bindingLayouts[0] = wgpuDeviceCreateBindGroupLayout(wgpContext.device, &bindGroupLayoutDescriptor);
	return bindingLayouts;
}

WGPUBindGroup NuklearGui::createBindGroup() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Matrix4f);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_texture.getTextureView();


	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_NUKLEAR"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup NuklearGui::createBindGroupFont() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Matrix4f);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_textureFont.getTextureView();


	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_NUKLEAR"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
}

WGPUBindGroup NuklearGui::createBindGroupIcon() {
	std::vector<WGPUBindGroupEntry> bindGroupEntries(3);
	bindGroupEntries[0].binding = 0u;
	bindGroupEntries[0].buffer = m_uniformBuffer.getBuffer();
	bindGroupEntries[0].offset = 0u;
	bindGroupEntries[0].size = sizeof(Matrix4f);

	bindGroupEntries[1].binding = 1u;
	bindGroupEntries[1].sampler = wgpContext.getSampler(SS_LINEAR_CLAMP);

	bindGroupEntries[2].binding = 2u;
	bindGroupEntries[2].textureView = m_textureIcon.getTextureView();


	WGPUBindGroupDescriptor bindGroupDesc = {};
	bindGroupDesc.layout = wgpuRenderPipelineGetBindGroupLayout(wgpContext.renderPipelines.at("RP_NUKLEAR"), 0u);
	bindGroupDesc.entryCount = (uint32_t)bindGroupEntries.size();
	bindGroupDesc.entries = bindGroupEntries.data();

	return wgpuDeviceCreateBindGroup(wgpContext.device, &bindGroupDesc);
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