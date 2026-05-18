#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>

#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <engine/CharacterSet.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpTexture.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>
#include <WebGPU/WgpData.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpFontRenderer.h>

class MSDFFont : public State, public MouseEventListener, public KeyboardEventListener {

public:

	MSDFFont(StateMachine& machine);
	~MSDFFont();

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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCube();
	std::vector<WGPUBindGroup> OnBindGroupsCube();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void initTextTransforms();

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	WgpBuffer m_uniformBuffer;
	Uniforms m_uniforms;

	CharacterSet m_characterSet;

	float largeScale = 0.0078125f;
	float smallScale = 0.00390625f;
	Matrix4f m_model;
	Matrix4f m_textTransforms[6];

	FormatedText m_formatedText;

	Shape m_cube;
	WgpModel m_wgpCube;
};