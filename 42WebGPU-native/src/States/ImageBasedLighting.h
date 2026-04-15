#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>
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
	void OnDraw(const WGPURenderPassEncoder& renderPassEncoder);

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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPBR();
	std::vector<WGPUBindGroup> OnBindGroupsPBR();

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsIrradiance();
	std::vector<WGPUBindGroup> OnBindGroupsIrradiance();

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCube();
	std::vector<WGPUBindGroup> OnBindGroupsCube();

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPrefilter();
	std::vector<WGPUBindGroup> OnBindGroupsPrefilter();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void applyTransformation(TrackBall& arc);
	void initMatrices();
	void initLights();
	void initIrradianceMatrices();
	void renderCube();
	void renderIrradiance();
	void renderPrefilter();

	bool m_initUi = true;
	bool m_drawUi = false;

	TrackBall m_trackball;
	Camera m_camera;
	Uniforms m_uniforms;
	WgpBuffer m_uniformBuffer, m_uniformModelBuffer, m_uniformLightBuffer, m_uniformMVPBuffer, m_roughnessBuffer;

	AssimpModel m_helmet;
	ObjModel m_sphereObj;
	Shape m_cube, m_sphere, m_spherePBR;
	WgpModel m_wgpHelmet, m_wgpCube, m_wgpSphere, m_wgpSpherePBR;
	WgpTexture m_wgpTextureCube, m_wgpTexture;
	Matrix4f m_models[12];
	PBRLightingUniforms m_lights[4];
	Matrix4f m_mvpInvCube[6];
	Matrix4f m_mvpCube[6];

	WGPUTexture irradianceTexture = NULL;
	WGPUTextureView irradianceView = NULL;

	WGPUTexture cubeTexture = NULL;
	WGPUTextureView cubView = NULL;

	WGPUTexture prefilterTexture = NULL;
	WGPUTextureView prefilterView = NULL;

	static void AddBindgroups(const WgpModel& model);
	static void AddBindgroups(const WgpModel& model, const WgpTexture& texture, std::string pipelineName);
};