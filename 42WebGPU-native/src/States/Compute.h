#pragma once

#include <webgpu.h>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <engine/TrackBall.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpTexture.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpData.h>


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

struct SettingsC {
	float scale = 0.5f;
};

class Compute : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Compute(StateMachine& machine);
	~Compute();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPURenderPassEncoder& renderPass);
	void OnCompute();

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	WGPUBindGroupLayout OnBindGroupLayout();
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder, bool force = false);
	WGPUBindGroup createBindGroup(const WGPUTextureView& inputTextureView, const WGPUTextureView& outputTextureView, const WGPUBuffer& uniformBuffer);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_shouldCompute = true;
	bool m_force = true;

	Camera m_camera;
	WgpBuffer m_uniformBuffer;
	WGPUTexture m_texture;
	WGPUTextureView m_textureView;
	UniformsCompute m_uniforms;
	Parameters m_parameters;
	SettingsC m_settings;
	WGPUBindGroup m_bindGroup;

	WGPUTexture inputTexture;
	WGPUTexture outputTexture;
	WGPUTextureView inputTextureView;
	WGPUTextureView outputTextureView;
};