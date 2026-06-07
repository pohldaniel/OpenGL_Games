#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/ObjModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

class PrimitivePicking : public State, public MouseEventListener, public KeyboardEventListener {
	struct Frame {
		Matrix4f viewProjectionMatrix;
		Matrix4f invViewProjectionMatrix;
		Vector2f pickCoord;
		uint32_t pickedPrimitive;
	};

public:

	PrimitivePicking(StateMachine& machine);
	~PrimitivePicking();

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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPick();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCompute();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDebug();

	std::vector<WGPUBindGroup> OnBindGroupsPick();
	WGPUBindGroup createComputeBindGroup();
	WGPUBindGroup createDebugBindGroup();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debug = false;

	Camera m_camera;
	ObjModel m_teapot;
	Uniforms m_uniforms;
	TrackBall m_trackball;

	WgpModel m_wgpTeapot;
	WgpBuffer m_uniformBuffer, m_computeBuffer, m_stagingBuffer;

	WGPUBindGroup m_computeBindGroup, m_debugBindGroup;
	WgpTexture m_indexTexture;

	std::vector<WGPURenderPassColorAttachment> renderPassColorAttachments;
};