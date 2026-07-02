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

class DeferredRendering : public State, public MouseEventListener, public KeyboardEventListener {
	
public:

	DeferredRendering(StateMachine& machine);
	~DeferredRendering();

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
	
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsGBuffer();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCompute();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDeferred();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDeferredDebug();

	std::vector<WGPUBindGroup> OnBindGroupsGBuffer();
	WGPUBindGroup createDeferredBindGroup();
	WGPUBindGroup createLightBindGroup();
	WGPUBindGroup createComputeBindGroup();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	float randomFloat(float min, float max);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debug = false;
	int m_numLights = 128;

	Camera m_camera;
	TrackBall m_trackball;
	AssimpModel m_dragon;
	Shape m_quad;

	WgpBuffer m_uniformBuffer, m_cameraBuffer, m_lightBuffer, m_configBuffer, m_extentBuffer;
	WgpModel m_wgpDragon, m_wgpQuad;
	WgpTexture m_normalTexture, m_albedoTexture, m_depthTexture;
	WGPUBindGroup m_deferredBindGroup, m_lightBindGroup, m_computeBindGroup;

	std::vector<WGPURenderPassColorAttachment> renderPassColorAttachments;
	WGPURenderPassDepthStencilAttachment renderPassDepthStencilAttachment;
};