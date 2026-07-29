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
#include <iostream>
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

	if (touch_started && nk_input_is_mouse_hovering_rect(input, bounds)) {
		ctx->active = ctx->current;
	}

	if (is_touched && ctx->active == ctx->current) {
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
		ctx->active = nullptr;
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

		if (nk_circular_action_button(&nkContext.context, "A", std::min(dimension.w, dimension.h), isPressed)) {

		}
	}
	nk_end(&nkContext.context);
}

bool nk_circular_action_button(struct nk_context* ctx, const char* label, float size_px, bool& isPressed) {
	isPressed = false;
	
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
			isPressed = true; // Genau in diesem Frame gedrückt
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
	return isPressed;
}