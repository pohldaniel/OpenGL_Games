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
#include <WebGPU/WgpRenderer.h>

class ImageBasedLighting : public State, public MouseEventListener, public KeyboardEventListener {
	enum Scene {
		SPHERE,
		HELMET
	};
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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPBR();
	std::vector<WGPUBindGroup> OnBindGroupsPBR();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPBRHelmet();
	std::vector<WGPUBindGroup> OnBindGroupsPBRHelmet();

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsIrradiance();
	std::vector<WGPUBindGroup> OnBindGroupsIrradiance();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCube();
	std::vector<WGPUBindGroup> OnBindGroupsCube();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPrefilter();
	std::vector<WGPUBindGroup> OnBindGroupsPrefilter();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSkybox();
	std::vector<WGPUBindGroup> OnBindGroupsSkybox();
	std::vector<WGPUBindGroup> OnBindGroupsSkyboxHelmet();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void applyTransformation(TrackBall& arc);
	void initMatrices();
	void initLights();
	void initIrradianceMatrices();
	
	void OnDrawBrdf(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip);
	void OnDrawCube(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip);
	void OnDrawIrradiance(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip);
	void OnDrawPrefilter(const WGPURenderPassEncoder& renderPassEncoder, uint32_t layer, uint32_t mip);

	bool m_initUi = true;
	bool m_drawUi = true;
	const uint32_t ROUGHNESS_LEVELS = 5u;

	TrackBall m_trackball;
	Camera m_camera;
	Uniforms m_uniforms;

	AssimpModel m_helmet;
	Shape m_cube, m_spherePBR, m_quad;
	
	Matrix4f m_models[12];
	PBRLightingUniforms m_lights[4];
	Matrix4f m_mvpInvCube[6];
	Matrix4f m_mvpCube[6];
	MaterialUniforms m_material;
	Matrix4f m_lightProjection, m_lightView;
	Scene m_scene = Scene::HELMET;

	WgpModel m_wgpHelmet, m_wgpCube, m_wgpSphere, m_wgpSpherePBR, m_wgpQuad;
	WgpTexture m_wgpTextureHDRCube, m_wgpTextureHDR;
	WgpTexture m_wgpTextureCube, m_wgpTextureIrradiance, m_wgpTexturePrefilter, m_wgpTextureBrdf, m_wgpTextureShadow;
	WgpTexture m_wgpTextutreNormal, m_wgpTextutreEmission, m_wgpTextutreMetalness, m_wgpTextutreLightmap;

	WgpBuffer m_uniformBuffer, m_uniformModelBuffer, m_uniformLightBuffer, m_uniformMVPBuffer, m_roughnessBuffer;
	WgpBuffer m_uniformMaterial, m_instanceBuffer;
};