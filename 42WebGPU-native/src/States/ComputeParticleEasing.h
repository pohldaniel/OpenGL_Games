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

#define PARTICLE_NUM_EASING (60000u)
#define PROPERTY_NUM (40u)
#define WORKGROUP_SIZE (256)

class ComputeParticleEasing : public State, public MouseEventListener, public KeyboardEventListener {
	struct SimParam{
		float time;
		float min_life;
		float max_life;
		float padding;
	};
public:

	ComputeParticleEasing(StateMachine& machine);
	~ComputeParticleEasing();

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
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCompute();

	WGPUBindGroup createBindGroupParticle();
	WGPUBindGroup createBindGroup();
	
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;

	WgpBuffer m_storageBuffer, m_simBuffer;
	WgpBuffer m_quadVerticesBuffer;
	WgpTexture m_wgpParticle;

	WGPUBindGroup m_bindGroupParticle, m_bindGroup;
	
	float m_initialParticle[PARTICLE_NUM_EASING * PROPERTY_NUM];
	void initParticle();
	float random_float_min_max(float min, float max);
	float random_float();
	float m_time = 0.0f;
	float m_speed = 3000.0f;
};