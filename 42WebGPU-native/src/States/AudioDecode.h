#pragma once

#include <vector>
#include <webgpu.h>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/sound/AudioDecoder.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <Nuklear/NkContext.h>
#include <States/StateMachine.h>
#include <WebGPU/WgpData.h>

class AudioDecode : public State, public MouseEventListener, public KeyboardEventListener {

public:

	AudioDecode(StateMachine& machine);
	~AudioDecode();

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

	Camera m_camera;
	Uniforms m_uniforms;
	TrackBall m_trackball;
	AudioDecoder m_audioDecoder;

	float btn_w;
	float btn_h;
	float spacing;
	float start_x;
	float total_block_h;
	float start_y;

	float ctrl_size;
	float side_padding;

	float bottom_margin;
	float ctrl_y;
	float play_x;
	float pause_x;

	int m_currentSong = 0;
	bool m_isPressed = false;
};