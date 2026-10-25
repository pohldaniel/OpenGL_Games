#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpData.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>

class HDRBloom : public State, public MouseEventListener, public KeyboardEventListener {

public:

	HDRBloom(StateMachine& machine);
	~HDRBloom();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void resize(int deltaW, int deltaH) override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnReEnter(unsigned int prevState) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroup> OnBindGroups();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void renderRectangle(float x, float y);
	void renderPad(float x, float y, long value);
	void renderButton(float x, float y, long value, int index);
	void renderStick(float x, float y, float stickX, float stickY);
	
	bool m_initUi = true;
	bool m_drawUi = false;
	long m_state = 0;
	float m_RightStickX = 0.0f;
	float m_RightStickY = 0.0f;
	float m_LeftStickX = 0.0f;
	float m_LeftStickY = 0.0f;

	Camera m_camera;
	Uniforms m_uniforms;
	Shape m_disk, m_quad;

	WgpBuffer m_uniformBuffer, m_storageBuffer;
	WgpModel m_wgpDisk, m_wgpQuad;
	std::vector<std::array<float, 20>> m_instances;
};