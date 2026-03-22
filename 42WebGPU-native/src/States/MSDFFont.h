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

enum TexturePack2 {
	SPIRAL2,
	TOY_BOX2
};

class MSDFFont : public State, public MouseEventListener, public KeyboardEventListener {

public:

	MSDFFont(StateMachine& machine);
	~MSDFFont();

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

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void applyTransformation(TrackBall& arc);

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

	std::vector <WGPUBindGroup> m_bindgroups;
	TexturePack2 m_texturePack = TexturePack2::SPIRAL2;
};