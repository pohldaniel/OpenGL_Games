#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>

#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <engine/TrackBall.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpTexture.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>
#include <WebGPU/WgpData.h>
#include <WebGPU/WgpModel.h>

class NormalMap : public State, public MouseEventListener, public KeyboardEventListener {
	enum SelectedShape {
		CUBE,
		SPHERE,
		TORUS,
		TORUS_KNOT,
		SPIRAL
	};

	enum TexturePack {
		TP_SPIRAL,
		TP_TOY_BOX
	};

	enum BumpMode {
		ALBEDO_TEXTURE,
		NORMAL_TEXTURE,
		DEPTH_TEXTURE,
		NORMAL_MAP,
		PRALLAX_SCALE,
		STEEP_PRALLAX
	};

	struct NormalUniforms {
		std::array <float, 3> light_pos_vs;
		uint32_t mode;
		float light_intensity;
		float depth_scale;
		float depth_layers;
		float padding;
	};

public:

	NormalMap(StateMachine& machine);
	~NormalMap();

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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	std::vector<WGPUBindGroup> OnBindGroups();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void createBindGroups();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	TrackBall m_trackball;

	Shape m_cube, m_sphere, m_torus, m_torusKnot, m_spiral;
	WgpBuffer m_uniformBuffer, m_normalUniformBuffer;

	Uniforms m_uniforms;
	NormalUniforms m_normalUniforms;
	WgpModel m_wgpCube, m_wgpSphere, m_wgpTorus, m_wgpTorusKnot, m_wgpSpiral;

	WgpTexture m_textureAW, m_textureNT, m_textureHT;
	WgpTexture m_textureNS, m_textureHS;
	WgpTexture m_textureAB, m_textureNB, m_textureHB;

	std::vector<WGPUBindGroup> m_bindgroups;
	std::vector<WGPUBindGroup> m_storedBindgroups;

	TexturePack m_texturePack = TexturePack::TP_SPIRAL;
	SelectedShape m_selectedShape = SelectedShape::CUBE;
	BumpMode m_bumpMode = BumpMode::NORMAL_MAP;

	float m_lightPosX = -1.7f;
	float m_lightPosY = 0.7f;
	float m_lightPosZ = 1.9f;
	int m_dpehtLayers = 16;
};