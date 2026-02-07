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
#include <WebGPU/WgpData.h>
#include <WebGPU/WgpModel.h>

enum Model {
	MAMMOTH,
	DRAGON
};

class Wireframe : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Wireframe(StateMachine& machine);
	~Wireframe();

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

	WGPUBindGroupLayout OnBindGroupLayoutPTN();
	WGPUBindGroupLayout OnBindGroupLayoutWF();
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void applyTransformation(TrackBall& arc);

	bool m_initUi = true;
	bool m_drawUi = true;
	Model m_model = Model::DRAGON;
	Camera m_camera;
	ObjModel m_mammoth, m_dragon;
	WgpBuffer m_uniformBuffer;

	WGPUTexture m_texture;
	WGPUTextureView m_textureView;

	TrackBall m_trackball;
	Uniforms m_uniforms;
	WgpModel m_wgpDragon, m_wgpMammoth;
};