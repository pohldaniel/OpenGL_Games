#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/animation/AnimatedModel.h>
#include <engine/animation/Animation.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/ObjModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>
#include <Utils/Fade.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpMesh.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

class StencilMask : public State, public MouseEventListener, public KeyboardEventListener {
	
	struct Object {
		float uniformValues[16 + 4];
		WgpBuffer uniformBuffer;
		WGPUBindGroup bindGroup;
		uint32_t geometryIndex;
	};

	struct Scene {
		uint32_t numObjects;
		WGPUBuffer sharedUniformBuffer;
		float sharedUniformValues[16 + 4];
		std::vector<Object> objects;		
	};

public:

	StencilMask(StateMachine& machine);
	~StencilMask();

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

	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsColor();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayoutsStencil();
	std::vector<WGPUBindGroup> OnBindGroupsColor();
	

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	void draw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor, const Scene& scene, uint32_t stencilRef);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_debug = false;

	Camera m_camera;
	Uniforms m_uniforms;
	TrackBall m_trackball;
	Shape m_quad, m_sphere, m_jem, m_cylinder, m_cone, m_torus, m_dice;

	WgpModel m_wgpSphere, m_wgpQuad, m_wgpJem, m_wgpCylinder, m_wgpCone, m_wgpTorus, m_wgpDice;
	WgpBuffer m_uniformBuffer;
	std::vector<Scene> m_scenes;
	std::vector<WgpModel> m_geometries;

	static void InitScene(Scene& scene, uint32_t numInstances, float hue, uint32_t geometryIndex, uint32_t geometryIndexCount, const WgpBuffer& sharedUniformBuffer);	
	static void updateScene0(float time, Scene& scene);
	static void hsl_to_rgba(float h, float s, float l, float* rgba);
	static float randf(float min_val, float max_val);
	static uint32_t rand_elem(uint32_t count);
};