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

class WaterSimulation : public State, public MouseEventListener, public KeyboardEventListener {
	
public:

	WaterSimulation(StateMachine& machine);
	~WaterSimulation();

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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDrop();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsUpdate();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsNormal();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSphere();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCaustics();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	Uniforms m_uniforms;
	Vector4f m_clearColor;
	WgpBuffer m_uniformBuffer, m_lightBuffer, m_sphereBuffer, m_shadowBuffer, m_dropBuffer, m_updateBuffer;
};