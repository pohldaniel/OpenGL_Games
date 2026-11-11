#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <engine/Transform.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpData.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpTexture.h>

#define NUM_OBJECTS 200u

class WireframeOverlay : public State, public MouseEventListener, public KeyboardEventListener {

	struct Object {
		WgpBuffer uniformBuffer;
		WgpBuffer uniformBufferLine;
		WGPUBindGroup bindGroup;
		WGPUBindGroup bindGroupWireframe;
		uint32_t geometryIndex;
	};

	struct LineUniforms {
		uint32_t stride;
		float thickness;
		float alpha_threshold;
		uint32_t padding;
	};

public:

	WireframeOverlay(StateMachine& machine);
	~WireframeOverlay();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;
	void resize(int deltaW, int deltaH) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsWireframe();
	std::vector<WGPUBindGroup> OnBindGroups();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	float randFloat();
	void rebuildPipeline();
	void updateUniforms();

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_animate = true;
	bool m_darawModels = true;
	bool m_drawWireframe = true;
	bool m_barycentric = false;
	int m_depthbias = 1;
	float m_depthBiasSlopeScale = 0.5f;
	float m_thickness = 2.0f;
	float m_alphaTthreshold = 0.5f;
	float m_animationTime = 0.0f;

	Camera m_camera;
	TrackBall m_trackball;
	Uniforms m_uniforms;

	ObjModel m_teapot;
	Shape m_sphere, m_jem, m_rock;
	std::vector<Object> m_objects;

	std::vector<WgpModel> m_wgpModels;
	WgpBuffer m_uniformBuffer;
};