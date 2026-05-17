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

class ComputeParticlesLogo : public State, public MouseEventListener, public KeyboardEventListener {
	enum SelectedAnimation {
		ATTACK,
		SWIM,
		PROCEDURAL
	};
	enum SelectedModel {
		VAMPIRE,
		WHALE
	};
	enum SelectedMode {
		NORMAL,
		JOINTS,
		WEIGHT
	};
public:

	ComputeParticlesLogo(StateMachine& machine);
	~ComputeParticlesLogo();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPURenderPassEncoder& renderPass);
	void OnDraw2(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsProbability();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSimulate();

	std::vector<WGPUBindGroup> OnBindGroups();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSkybox();
	std::vector<WGPUBindGroup> OnBindGroupsSkybox();

	WGPUBindGroup createBindGroup();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;
	float m_fadeValue = 0.0f;
	float m_speed = 50.0f;
	float m_angle = 0.2f;
	bool m_skinMode = true;

	Camera m_camera;
	Uniforms m_uniforms;
	Matrix4f m_lightProjection, m_lightView, m_shadow;
	Animation m_attack, m_swim, m_dance;
	AnimatedModel m_whale, m_vampire;
	Fade m_fade;
	Shape m_cube;

	WgpBuffer m_uniformBuffer, m_skinBuffer, m_modeBuffer;
	WgpModel m_wgpWhale, m_wgpVampire, m_wgpCube;
	WgpTexture m_wgpTextureCube;

	SelectedAnimation m_animation = SelectedAnimation::PROCEDURAL;
	SelectedModel m_model = SelectedModel::WHALE;
	SelectedMode m_mode = SelectedMode::NORMAL;

	void proceduralSkinning(Bone**& bones, unsigned short numBones, float angle);

	WgpTexture m_wgpWgpuLogo;

	WgpBuffer m_probabilityBuffer, m_bufferA, m_bufferB, m_simulationBuffer, m_particlesBuffer;

	ParticleData data;
	WGPUBindGroup m_bindGroup;
};