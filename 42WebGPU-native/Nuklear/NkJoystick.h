#pragma once

struct JoystickResult {
	void reset();

	float x = 0.0f;
	float y = 0.0f;
	bool isActive = false;

};

extern "C" {

	void virtual_joystick(struct nk_rect dimension, JoystickResult& out);
	void nk_virtual_joystick(struct nk_context* ctx, float size_px, JoystickResult& out);

	void action_button(struct nk_rect dimension, bool& isPressed);
	bool nk_circular_action_button(struct nk_context* ctx, const char* label, float size_px, bool& isPressed);
}