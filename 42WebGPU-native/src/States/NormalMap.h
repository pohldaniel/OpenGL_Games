#pragma once
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

struct CameraState2 {
	Vector2f angles = { 0.8f, 0.5f };
	float zoom = -1.2f;
};

struct DragState2 {
	bool active = false;
	Vector2f startMouse;
	CameraState2 startCameraState;
	Vector2f velocity = { 0.0f, 0.0f };
	Vector2f previousDelta;
	float intertia = 0.9f;
	float sensitivity = 0.01f;
	float scrollSensitivity = 0.1f;
};

class NormalMap : public State, public MouseEventListener, public KeyboardEventListener {

public:

	NormalMap(StateMachine& machine);
	~NormalMap();

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

	WGPUBindGroupLayout OnBindGroupLayout();
	WGPUBindGroup OnBindGroup(const WGPUTextureView textureView);

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void updateViewMatrix();
	void updateLightingUniforms();
	void updateDragInertia();

	bool m_initUi = true;
	bool m_drawUi = true;

	Camera m_camera;
	ObjModel m_boat;
	WgpBuffer m_uniformBuffer, m_uniformLigthBuffer;

	WGPUTexture m_texture;
	WGPUTextureView m_textureView;

	Uniforms m_uniforms;
	WgpModel m_wgpBoat;

	CameraState2 m_cameraState;
	DragState2 m_drag;

	LightingUniforms m_lightingUniforms;
	bool m_updateLight = true;
};