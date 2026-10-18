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

class Controller : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Controller(StateMachine& machine);
	~Controller();

	void fixedUpdate() override;
	void update() override;
	void render() override;

	void resize(int deltaW, int deltaH) override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnKeyDown(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroup> OnBindGroups();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	Uniforms m_uniforms;
	Shape m_disk;

	WgpBuffer m_uniformBuffer;
	WgpModel m_wgpDisk;
};