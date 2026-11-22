#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>
#include <engine/ObjModel.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpData.h>
#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpTexture.h>

#define WATER_WIDTH 256
#define WATER_HEIGHT 256
#define CAUSTICS_SIZE 1024
#define SPHERE_RADIUS 0.25f
#define WATER_SURFACE_DETAIL 200

class WaterSimulation : public State, public MouseEventListener, public KeyboardEventListener {
	
public:

	WaterSimulation(StateMachine& machine);
	~WaterSimulation();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;
	void resize(int deltaW, int deltaH) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsDrop();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsUpdate();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsNormal();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSphere();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsCaustics();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsPool();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsWater();

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsSkybox();

	std::vector<WGPUBindGroup> OnBindGroupsSkybox();
	void createBindGroupPool();
	void createBindCaustic();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void run_simulation_pass(WGPURenderPipeline pipeline, WGPUBuffer uniform_buffer, uint32_t uniform_size);
	

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useA = true;

	Camera m_camera;
	Uniforms m_uniforms;
	Shape m_cube;

	WgpBuffer m_uniformBuffer, m_lightBuffer, m_sphereBuffer, m_shadowBuffer, m_dropBuffer, m_updateBuffer;
	WgpBuffer m_poolVertex, m_poolIndex, m_waterVertex, m_waterIndex;
	WgpTexture m_wgpTextureCube, m_wgpTextureTiles, m_textureA, m_textureB, m_textureCaustics;
	WgpModel m_wgpCube;

	WGPUBindGroup m_bindGoupPoolA, m_bindGoupPoolB, m_bindGroupCausticA, m_bindGroupCausticB;
};