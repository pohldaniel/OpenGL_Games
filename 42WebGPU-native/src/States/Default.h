#pragma once

#include <webgpu.h>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <engine/TrackBall.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpTexture.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>

struct Uniforms {
	Matrix4f projectionMatrix;
	Matrix4f viewMatrix;
	Matrix4f modelMatrix;
	std::array<float, 4> color;
};

class Default : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Default(StateMachine& machine);
	~Default();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPURenderPassEncoder& renderPass);
	

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	WGPUBindGroupLayout OnBindGroupLayoutPTN();
	WGPUBindGroup createBindGroupPTN();
	WGPUBindGroupLayout OnBindGroupLayoutWireframe();
	WGPUBindGroup createBindGroupWireframe();

	void createColorBuffer();


	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void applyTransformation(TrackBall& arc);

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	ObjModel m_model;
	std::list<WgpMesh> m_mammoth;
	std::list<WgpTexture> m_textures;
	std::list<WgpBuffer> m_vertexBuffer;
	std::list<WgpBuffer> m_indexBuffer;	
	WgpBuffer m_colorBuffer, m_uniformBuffer;
	std::vector<float> m_colors;

	WGPUTexture m_texture;
	WGPUTextureView m_textureView;
	WGPUBindGroup m_bindGroupPTN, m_bindGroupWireframe;

	TrackBall m_trackball;
	Uniforms m_uniforms;
};