#pragma once

#include <webgpu.h>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpTexture.h>
#include <WebGPU/WgpBuffer.h>

enum class FilterType {
	Sum,
	Maximum,
	Minimum,
};

struct Parameters {
	FilterType filterType = FilterType::Sum;
	Matrix4f kernel = Matrix4f::IDENTITY;
	bool normalize = true;
};

struct UniformsCompute {
	Matrix4f kernel;
	float test = 0.5f;
	uint32_t filterType = 0;
	float _pad[2];
};

class Compute : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Compute(StateMachine& machine);
	~Compute();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPURenderPassEncoder& renderPass);
	void compute();

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	WGPUBindGroupLayout OnBindGroupLayout();
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder, bool force = false);
	WGPUBindGroup createBindGroup(const WGPUTextureView& inputTextureView, const WGPUTextureView& outputTextureView, const WGPUBuffer& uniformBuffer);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_shouldCompute = true;
	bool m_force = true;

	
	UniformsCompute m_uniforms;
	Parameters m_parameters;
	float m_scale = 0.5f;
	WgpBuffer m_uniformBuffer;
	WgpTexture m_inputTexture, m_outputTexture;
	WGPUBindGroup m_bindGroup;
	WGPUTextureView m_inputTextureView;
	WGPUTextureView m_outputTextureView;
};