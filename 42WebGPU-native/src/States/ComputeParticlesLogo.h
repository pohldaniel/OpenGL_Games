#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AnimatedModel.h>
#include <engine/animation/Animation.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/Camera.h>
#include <Utils/Fade.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

#define PARTICLE_NUM (50000u)

class ComputeParticlesLogo : public State, public MouseEventListener, public KeyboardEventListener {

	struct RenderParams {
		Matrix4f model_view_projection_matrix;
		Vector3f right;
		float pad1;
		Vector3f up;
		float pad2;
	};

	struct Seed {
		float x;
		float y;
		float z;
		float w;
	};

	struct ParticleData {
		float delta_time;
		float brightness_factor;
		float pad[2];
		Seed seed;
	};

public:

	ComputeParticlesLogo(StateMachine& machine);
	~ComputeParticlesLogo();

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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsProbability();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSimulate();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsParticle();

	WGPUBindGroup createComputeBindGroup();
	WGPUBindGroup createBindGroup();
	void updateSimulation();
	float randomFloat(float min, float max);
	float randomFloat();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	ParticleData particleData;
	RenderParams m_renderParams;

	WgpTexture m_wgpWgpuLogo;
	WgpBuffer m_probabilityBuffer, m_bufferA, m_bufferB, m_simulationBuffer, m_particlesBuffer, m_renderParamsBuffer, m_quadVerticesBuffer;
	WGPUBindGroup m_computeBindGroup, m_bindGroup;
	
};