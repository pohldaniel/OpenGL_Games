#define _USE_MATH_DEFINES
#include <math.h>

#include "NkJoystick.h"
#include "NkContext.h"

void JoystickResult::reset() {
	x = 0.0f;
	y = 0.0f;
	isActive = false;
}

void virtual_joystick(struct nk_rect dimension, JoystickResult& out) {
	if (nk_begin(&nkContext.context, "virtual_joystick", dimension, NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)){
		float joystick_size = std::min(dimension.w, dimension.h);
		nk_layout_row_static(&nkContext.context, joystick_size, joystick_size, 1);
		nk_virtual_joystick(&nkContext.context, joystick_size, out);
	}
	nk_end(&nkContext.context);
}

void nk_virtual_joystick(struct nk_context* ctx, float size_px, JoystickResult& out) {
	struct nk_rect bounds;
	nk_widget(&bounds, ctx);

	float radius_base = bounds.w / 2.0f;
	float radius_stick = radius_base * 0.4f;
	struct nk_vec2 center = nk_vec2(bounds.x + radius_base, bounds.y + radius_base);

	const struct nk_input* input = &ctx->input;
	bool is_touched = input->mouse.buttons[NK_BUTTON_LEFT].down;
	bool touch_started = input->mouse.buttons[NK_BUTTON_LEFT].clicked;
	struct nk_vec2 touch_pos = input->mouse.pos;

	if (touch_started && nkContext.activeWidget == nullptr && nk_input_is_mouse_hovering_rect(input, bounds)) {
		nkContext.activeWidget = ctx->current;
	}

	if (is_touched && nkContext.activeWidget == ctx->current) {
		out.isActive = true;

		float dx = touch_pos.x - center.x;
		float dy = touch_pos.y - center.y;
		float distance = std::sqrt(dx * dx + dy * dy);
		float max_distance = radius_base - radius_stick;

		if (distance > 0.0f) {
			float nx = dx / distance;
			float ny = dy / distance;
			float clamped_dist = (distance > max_distance) ? max_distance : distance;
			out.x = (nx * clamped_dist) / max_distance;
			out.y = -((ny * clamped_dist) / max_distance);
		}else {
			out.x = 0.0f;
			out.y = 0.0f;
		}
	}else {
		out.isActive = false;
		out.x = 0.0f;
		out.y = 0.0f;

		if (nkContext.activeWidget == ctx->current)
			nkContext.activeWidget = nullptr;
	}

	float max_distance = radius_base - radius_stick;
	struct nk_vec2 stick_pos;
	stick_pos.x = center.x + out.x * max_distance;
	stick_pos.y = center.y - out.y * max_distance;

	struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);
	if (canvas) {
		nk_fill_circle(canvas, nk_rect(bounds.x, bounds.y, bounds.w, bounds.h), nk_rgba(50, 50, 50, 150));
		nk_stroke_circle(canvas, nk_rect(bounds.x, bounds.y, bounds.w, bounds.h), 2.0f, nk_rgb(200, 200, 200));
		float sx = stick_pos.x - radius_stick;
		float sy = stick_pos.y - radius_stick;
		float sw = radius_stick * 2.0f;
		nk_fill_circle(canvas, nk_rect(sx, sy, sw, sw), nk_rgb(255, 100, 100));
	}
}

void action_button(struct nk_rect dimension, bool& isPressed) {
	if (nk_begin(&nkContext.context, "action_button", nk_rect(dimension.x, dimension.y, dimension.w, dimension.h), NK_WINDOW_NO_SCROLLBAR)) {

		nk_layout_row_static(&nkContext.context, dimension.h, static_cast<int>(dimension.w), 1);

		if (nk_action_button(&nkContext.context, "A", std::min(dimension.w, dimension.h), isPressed)) {

		}
	}
	nk_end(&nkContext.context);
}

bool nk_action_button(struct nk_context* ctx, const char* label, float size_px, bool& isPressed) {
	isPressed = false;
	
	struct nk_rect bounds;
	nk_widget(&bounds, ctx);

	float radius = bounds.w / 2.0f;
	struct nk_vec2 center = nk_vec2(bounds.x + radius, bounds.y + radius);

	const struct nk_input* input = &ctx->input;
	bool is_touched = input->mouse.buttons[NK_BUTTON_LEFT].down;
	struct nk_vec2 touch_pos = input->mouse.pos;
	bool is_hovered = false;
	if (is_touched) {
		float dx = touch_pos.x - center.x;
		float dy = touch_pos.y - center.y;
		if ((dx * dx + dy * dy) <= (radius * radius)) {
			is_hovered = true;
		}
	}

	static bool button_was_down = false;
	if (is_hovered && is_touched) {
		if (!button_was_down) {
			isPressed = true;
			button_was_down = true;
		}
	}else if (!is_touched) {
		button_was_down = false;
	}

	struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);
	if (canvas) {
		struct nk_color btn_color = is_hovered ? nk_rgb(180, 50, 50) : nk_rgb(255, 80, 80);
		struct nk_color border_color = is_hovered ? nk_rgb(255, 255, 255) : nk_rgb(200, 200, 200);

		nk_fill_circle(canvas, bounds, btn_color);
		nk_stroke_circle(canvas, bounds, 3.0f, border_color);

		const struct nk_user_font* font = ctx->style.font;
		float text_width = font->width(font->userdata, font->height, label, nk_strlen(label));

		struct nk_vec2 text_pos;
		text_pos.x = center.x - (text_width / 2.0f);
		text_pos.y = center.y - (font->height / 2.0f);

		nk_draw_text(canvas, nk_rect(text_pos.x, text_pos.y, text_width, font->height),
			label, nk_strlen(label), font, nk_rgb(0, 0, 0), nk_rgb(255, 255, 255));
	}
	return isPressed;
}

void virtual_rotation(struct nk_rect dimension, RotationResult& out) {
	if (nk_begin(&nkContext.context, "nuclear_rotation", dimension, NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {
		float widget_size = std::min(dimension.w, dimension.h);
		nk_layout_row_static(&nkContext.context, widget_size, widget_size, 1);
		nk_virtual_rotation(&nkContext.context, widget_size, out);
	}
	nk_end(&nkContext.context);
}

void nk_virtual_rotation(struct nk_context* ctx, float size_px, RotationResult& out) {

	struct nk_rect bounds;
	nk_widget(&bounds, ctx);

	float radius_base = bounds.w / 2.0f;
	float outer_radius = radius_base * 0.85f;
	float inner_radius = outer_radius * 0.35f;
	float knob_radius = radius_base * 0.08f;
	struct nk_vec2 center = nk_vec2(bounds.x + radius_base, bounds.y + radius_base);

	const struct nk_input* input = &ctx->input;
	bool is_touched = input->mouse.buttons[NK_BUTTON_LEFT].down;
	bool touch_started = input->mouse.buttons[NK_BUTTON_LEFT].clicked;
	struct nk_vec2 touch_pos = input->mouse.pos;

	if (touch_started && nkContext.activeWidget == nullptr && nk_input_is_mouse_hovering_rect(input, bounds)) {
		nkContext.activeWidget = ctx->current;
	}

	if (is_touched && nkContext.activeWidget == ctx->current) {
		out.isActive = true;

		float dx = touch_pos.x - center.x;
		float dy = touch_pos.y - center.y;
		float distance = std::sqrt(dx * dx + dy * dy);
		if (distance > 0.0f) {
			float angle_rad = std::atan2(dy, dx);
			if (angle_rad < 0.0f) {
				angle_rad += 2.0f * M_PI;
			}
			out.degrees = angle_rad * (180.0f / M_PI);
		}
	}
	else {
		out.isActive = false;
		if (nkContext.activeWidget == ctx->current)
			nkContext.activeWidget = nullptr;
	}

	float current_rad = out.degrees * (M_PI / 180.0f);
	struct nk_vec2 knob_pos;
	knob_pos.x = center.x + outer_radius * std::cos(current_rad);
	knob_pos.y = center.y + outer_radius * std::sin(current_rad);

	struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);
	if (canvas) {
		struct nk_color color_dark_bg = nk_rgba(20, 20, 25, 200);
		struct nk_color color_neon_green = nk_rgb(57, 255, 20);
		struct nk_color color_cyan = nk_rgb(0, 240, 255);

		nk_fill_circle(canvas, bounds, color_dark_bg);
		nk_stroke_circle(canvas, nk_rect(center.x - outer_radius, center.y - outer_radius, outer_radius * 2.0f, outer_radius * 2.0f), 2.0f, color_neon_green);

		float kx = knob_pos.x - knob_radius;
		float ky = knob_pos.y - knob_radius;
		float kw = knob_radius * 2.0f;
		nk_fill_circle(canvas, nk_rect(kx, ky, kw, kw), color_cyan);
		nk_stroke_circle(canvas, nk_rect(kx, ky, kw, kw), 1.5f, color_neon_green);

		nk_stroke_circle(canvas, nk_rect(center.x - inner_radius, center.y - inner_radius, inner_radius * 2.0f, inner_radius * 2.0f), 2.0f, color_neon_green);

		for (int i = 0; i < 3; ++i) {
			float wing_angle = current_rad + (i * 2.0f * M_PI / 3.0f);
			float line_end_x = center.x + inner_radius * std::cos(wing_angle);
			float line_end_y = center.y + inner_radius * std::sin(wing_angle);
			nk_stroke_line(canvas, center.x, center.y, line_end_x, line_end_y, 3.0f, color_neon_green);
		}
	}
}

void virtual_rotation_button(struct nk_rect dimension, RotationButtonResult& out) {
	if (nk_begin(&nkContext.context, "nuclear_widget", dimension, NK_WINDOW_NO_INPUT | NK_WINDOW_NO_SCROLLBAR)) {
		float widget_size = std::min(dimension.w, dimension.h);
		nk_layout_row_static(&nkContext.context, widget_size, widget_size, 1);
		nk_virtual_rotation_button(&nkContext.context, widget_size, out);
	}
	nk_end(&nkContext.context);
}

void nk_virtual_rotation_button(struct nk_context* ctx, float size_px, RotationButtonResult& out) {
	struct nk_rect bounds;
	nk_widget(&bounds, ctx);

	float radius_base = bounds.w / 2.0f;
	float outer_radius = radius_base * 0.85f;
	float inner_radius = outer_radius * 0.35f;
	float knob_radius = radius_base * 0.08f;
	struct nk_vec2 center = nk_vec2(bounds.x + radius_base, bounds.y + radius_base);

	const struct nk_input* input = &ctx->input;
	bool is_touched = input->mouse.buttons[NK_BUTTON_LEFT].down;
	bool touch_started = input->mouse.buttons[NK_BUTTON_LEFT].clicked;
	struct nk_vec2 touch_pos = input->mouse.pos;

	float dx = touch_pos.x - center.x;
	float dy = touch_pos.y - center.y;
	float distance = std::sqrt(dx * dx + dy * dy);

	out.buttonPressed = false;

	if (touch_started && nkContext.activeWidget == nullptr && nk_input_is_mouse_hovering_rect(input, bounds)) {
		nkContext.activeWidget = ctx->current;
	}

	if (is_touched && nkContext.activeWidget == ctx->current) {
		out.isActive = true;

		if (touch_started && distance <= inner_radius) {
			out.buttonDown = true;
			out.buttonPressed = true;
			out.isRotating = false;
		}else if (touch_started && distance > inner_radius) {
			out.isRotating = true;
			out.buttonDown = false;
		}

		if (out.isRotating && distance > 5.0f) {
			float angle_rad = std::atan2(dx, dy);
			out.degrees = angle_rad * (180.0f / M_PI);
		}else if (out.buttonDown) {
			out.buttonDown = (distance <= inner_radius);
		}
	}else {
		out.isRotating = false;
		out.buttonDown = false;
		out.isActive = false;
		if (nkContext.activeWidget == ctx->current)
			nkContext.activeWidget = nullptr;
	}

	float current_rad = out.degrees * (M_PI / 180.0f);

	struct nk_vec2 knob_pos;
	knob_pos.x = center.x + outer_radius * std::sinf(current_rad);
	knob_pos.y = center.y + outer_radius * std::cosf(current_rad);

	struct nk_command_buffer* canvas = nk_window_get_canvas(ctx);
	if (canvas) {
		struct nk_color color_dark_bg = nk_rgba(20, 20, 25, 200);
		struct nk_color color_neon_green = nk_rgb(57, 255, 20);
		struct nk_color color_cyan = nk_rgb(0, 240, 255);
		struct nk_color button_body_color = out.buttonDown ? nk_rgb(180, 50, 50) : nk_rgb(30, 40, 35);
		struct nk_color button_line_color = out.buttonDown ? nk_rgb(255, 100, 100) : color_neon_green;


		nk_fill_circle(canvas, bounds, color_dark_bg);
		nk_stroke_circle(canvas, nk_rect(center.x - outer_radius, center.y - outer_radius, outer_radius * 2.0f, outer_radius * 2.0f), 2.0f, color_neon_green);

		float kx = knob_pos.x - knob_radius;
		float ky = knob_pos.y - knob_radius;
		float kw = knob_radius * 2.0f;
		nk_fill_circle(canvas, nk_rect(kx, ky, kw, kw), color_cyan);
		nk_stroke_circle(canvas, nk_rect(kx, ky, kw, kw), 1.5f, color_neon_green);

		struct nk_rect inner_bounds = nk_rect(center.x - inner_radius, center.y - inner_radius, inner_radius * 2.0f, inner_radius * 2.0f);
		nk_fill_circle(canvas, inner_bounds, button_body_color);
		nk_stroke_circle(canvas, inner_bounds, 2.0f, button_line_color);

		for (int i = 0; i < 3; ++i) {
			float wing_angle = current_rad + (i * 2.0f * M_PI / 3.0f);
			float line_end_x = center.x + inner_radius * std::sinf(wing_angle);
			float line_end_y = center.y + inner_radius * std::cosf(wing_angle);
			nk_stroke_line(canvas, center.x, center.y, line_end_x, line_end_y, 3.0f, button_line_color);
		}
	}
}