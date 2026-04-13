#pragma once
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
#include <WebGPU/WgpTexture.h>

class ImageBasedLighting : public State, public MouseEventListener, public KeyboardEventListener {

public:

	ImageBasedLighting(StateMachine& machine);
	~ImageBasedLighting();

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
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsEnvCube();
	std::vector<WGPUBindGroup> OnBindGroupsEnvCube();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsEnvSphere();
	std::vector<WGPUBindGroup> OnBindGroupsEnvSphere();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void applyTransformation(TrackBall& arc);

	bool m_initUi = true;
	bool m_drawUi = false;

	TrackBall m_trackball;
	Camera m_camera;
	Uniforms m_uniforms;
	WgpBuffer m_uniformBuffer;

	AssimpModel m_helmet;
	Shape m_cube, m_sphere;
	WgpModel m_wgpHelmet, m_wgpCube, m_wgpSphere;
	WgpTexture m_wgpTextureCube, m_wgpTexture;

	static void AddBindgroups(const WgpModel& model);
	static void AddBindgroups(const WgpModel& model, const WgpTexture& texture, std::string pipelineName);
};