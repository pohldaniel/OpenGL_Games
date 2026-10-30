#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpData.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpTexture.h>

class ABuffer : public State, public MouseEventListener, public KeyboardEventListener {
	struct SliceInfo {
		uint32_t maxStorableFragments;
		uint32_t targetWidth;
		int32_t  sliceStartY;
		uint32_t padding;
	};
public:

	ABuffer(StateMachine& machine);
	~ABuffer();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnPostDraw();
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;
	void resize(int deltaW, int deltaH) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsTranslucent();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsComposite();

	std::vector<WGPUBindGroup> OnBindGroups();
	WGPUBindGroup createBindGroupTranslucent();
	WGPUBindGroup createBindGroupComposite();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);	
	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_wantResize = false;

	Camera m_camera;
	Uniforms m_uniforms;
	TrackBall m_trackball;
	ObjModel m_teapot;

	WgpBuffer m_uniformBuffer, m_sliceInfoBuffer, m_linkedListBuffer, m_headsInitBuffer, m_headsBuffer;
	WgpModel m_wgpTeapot;
	WGPUBindGroup m_bindGroupTranslucent, m_bindGroupComposite;

	uint32_t num_slices;
	uint32_t slice_height;
	uint32_t minUniformBufferOffsetAlignment;
};