#pragma once

#include <webgpu.h>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpTexture.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>

struct MyUniforms {
	Matrix4f projectionMatrix;
	Matrix4f viewMatrix;
	Matrix4f modelMatrix;
	std::array<float, 4> color;
	float time;
	float _pad[3];
};

class Default : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Default(StateMachine& machine);
	~Default();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPURenderPassEncoder& renderPass);
	WGPUPipelineLayout OnPipelineLayout();

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	ObjModel m_model;
	std::list<WgpMesh> m_mammoth;
	std::list<WgpTexture> m_textures;
	std::list<WgpBuffer> m_vertexBuffer;
	std::list<WgpBuffer> m_indexBuffer;	

	WGPUTexture m_texture;
	WGPUBuffer m_uniformBuffer;

	MyUniforms m_uniforms;
	float angle1 = 2.0f;
	Matrix4f S;
	Matrix4f T1;
	Matrix4f R1;
	Matrix4f R2;
	Matrix4f T2;

};