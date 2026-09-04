#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/video/VideoDecoder.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>
#include <WebGPU/WgpBuffer.h>

class VideoDecode : public State, public MouseEventListener, public KeyboardEventListener {

	struct CameraUniforms {
		Matrix4f viewMatrix;
		float fov = 1.0f;
		float aspect = 1.6f;
		float padding[2];
	};

	struct SliderState {
		float sliderTime = 0.0f;
		bool isUserDragging = false;
	};

public:

	VideoDecode(StateMachine& machine);
	~VideoDecode();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnPostDraw();

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts360();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts360HW();

	WGPUBindGroup createBindGroupRGBA();
	WGPUBindGroup createBindGroup360YUV();
	WGPUBindGroup createBindGroup360HW();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void renderVideoTimeline(const char* label, VideoDecoder& movie, SliderState& state);

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	TrackBall m_trackball;
	
	WgpBuffer m_cameraBuffer;
	VideoDecoder m_movieRGBA, m_movieYUV, m_movieHw;
	bool m_isUserDraggingTimeline = false;
	float m_sliderTime = 0.0f;

	SliderState m_stateHw;
	SliderState m_stateRGBA;
	SliderState m_stateYUV;
};