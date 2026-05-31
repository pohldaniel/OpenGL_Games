#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

class OcclusionQuery : public State, public MouseEventListener, public KeyboardEventListener {

public:

	OcclusionQuery(StateMachine& machine);
	~OcclusionQuery();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsVolume();
	WGPUBindGroup createVolumeBindGroup();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;
	float m_rotation = 0.0f;
	bool m_rotate = true;
	float m_near = 4.3f;
	float m_far = 4.4f;

	Camera m_camera;
	TrackBall m_trackball;

	WgpBuffer m_uniformBuffer;
	WgpTexture m_volumeTexture;
	WGPUBindGroup m_volumeBindGroup;
};