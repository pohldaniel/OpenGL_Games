#pragma once
#include <vector>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/shape/Shape.h>
#include <engine/AssimpModel.h>
#include <engine/TrackBall.h>
#include <engine/Camera.h>

#include <States/StateMachine.h>

#include <WebGPU/WgpBuffer.h>
#include <WebGPU/WgpModel.h>
#include <WebGPU/WgpData.h>

class RenderBundles : public State, public MouseEventListener, public KeyboardEventListener {

	struct Scene {
		WgpBuffer uniformBuffer;
		WgpModel model;
		bool isVisible;
		std::array<float, 4> color;
	};

public:

	RenderBundles(StateMachine& machine);
	~RenderBundles();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void OnDraw(const WGPUCommandEncoder& commandEncoder, const WGPURenderPassDescriptor& renderPassDescriptor);
	void OnPostDraw();

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(const Event::MouseMoveEvent& event) override;
	void OnMouseWheel(const Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(const Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(const Event::MouseButtonEvent& event) override;
	void OnKeyDown(const Event::KeyboardEvent& event) override;
	void OnKeyUp(const Event::KeyboardEvent& event) override;

private:

	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	WGPUQuerySet createQuerySet();

	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_animate = true;
	float m_time = 0.0f;

	Camera m_camera;
	TrackBall m_trackball;
	Uniforms m_uniforms;
	Shape m_cube;

	WgpBuffer m_uniformBuffer;
	WgpBuffer m_resolveBuffer, m_resultBuffer;
	WGPUQuerySet m_querySet;
	std::vector<Scene> m_scenes;

	static float PingPongSine(float t);
	static void InitScene(Scene& scene, Shape& shape, const WgpBuffer& uniformBuffer, std::array<float, 4> color);
	static void UpdateScene(Scene& scene, const Vector3f& position, float time);
	static void OnMapBuffer(WGPUMapAsyncStatus status, WGPUStringView message, void* userdata1, void* userdata2);
};