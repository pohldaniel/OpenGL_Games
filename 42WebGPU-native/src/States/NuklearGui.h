#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AnimatedModel.h>
#include <engine/animation/Animation.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>
#include <Nuklear/NkContext.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

class NuklearGui : public State, public MouseEventListener, public KeyboardEventListener {
	struct JoystickResult {
		float x = 0.0f; // -1.0 bis 1.0 (Links/Rechts)
		float y = 0.0f; // -1.0 bis 1.0 (Oben/Unten)
		bool is_active = false;
	};

public:

	NuklearGui(StateMachine& machine);
	~NuklearGui();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnFillBuffer(nk_context& nkCntxt);

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_isHovered = false;
	float m_uiScale = 1.0f;
	float m_scrollDelta = 0.0f;
	bool m_wasHovered = false;

	Camera m_camera;
	TrackBall m_trackball;

	JoystickResult nk_virtual_joystick(struct nk_context* ctx, float size_px);
	bool nk_circular_action_button(struct nk_context* ctx, const char* label, float size_px);

	struct nk_image playIcon;
	struct nk_vec2 current_pos;

	const float BASE_ROW_DYN = 30.0f;
	const float BASE_ROW_STAT = 32.0f;

	AnimatedModel m_player;
	Animation m_animation;
};