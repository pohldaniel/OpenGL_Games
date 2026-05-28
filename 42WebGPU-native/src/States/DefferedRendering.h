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

#define MAX_NUM_LIGHTS 1024u

class DefferedRendering : public State, public MouseEventListener, public KeyboardEventListener {
	
public:

	DefferedRendering(StateMachine& machine);
	~DefferedRendering();

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
	
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsColor();
	std::vector<WGPUBindGroup> OnBindGroupsColor();

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsGBuffer();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCompute();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDeffered();
	
	std::vector<WGPUBindGroup> OnBindGroupsGBuffer();
	WGPUBindGroup createDefferedBindGroup();
	WGPUBindGroup createLightBindGroup();
	WGPUBindGroup createComputeBindGroup();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	TrackBall m_trackball;
	AssimpModel m_dragon;
	Uniforms m_uniforms;
	Shape m_quad;

	WgpBuffer m_uniformBuffer, _uniformBuffer, m_cameraBuffer, m_lightBuffer, m_configBuffer, m_extentBuffer;
	WgpModel m_wgpDragon, m_wgpQuad;
	WgpTexture m_normalTexture, m_albedoTexture, m_depthTexture;
	WGPUBindGroup m_defferedBindGroup, m_lightBindGroup, m_computeBindGroup;

	std::vector<WGPURenderPassColorAttachment> renderPassColorAttachments;
	WGPURenderPassDepthStencilAttachment renderPassDepthStencilAttachment;
};