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

class RenderBundles : public State, public MouseEventListener, public KeyboardEventListener {

public:

	RenderBundles(StateMachine& machine);
	~RenderBundles();

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

	std::vector<WGPUBindGroup> OnBindGroups();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	std::vector<Shape> m_asteroids;

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	TrackBall m_trackball;
	Uniforms m_uniforms;
	Shape m_sphere;
	
	WgpModel m_wgpSphere;
	WgpBuffer m_uniformBuffer, m_modelBuffer;
	WgpTexture m_saturnTexture, m_moonTexture;
};