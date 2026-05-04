#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

class ShadowMapping : public State, public MouseEventListener, public KeyboardEventListener {

public:

	ShadowMapping(StateMachine& machine);
	~ShadowMapping();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPURenderPassEncoder& renderPass);

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroup> OnBindGroups();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsShadow();
	std::vector<WGPUBindGroup> OnBindGroupsShadow();
	void OnDrawShadow(const WGPURenderPassEncoder& renderPassEncoder);

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	Uniforms m_uniforms;
	AssimpModel m_dragon;
	Shape m_quad;
	Matrix4f m_lightProjection, m_lightView, m_shadow;

	WgpModel m_wgpDragon, m_wgpQuad;
	WgpBuffer m_uniformBuffer;
	WgpTexture m_wgpTextureShadow;
};