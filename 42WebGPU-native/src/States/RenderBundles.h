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

#define MAX_ASTEROID_COUNT (20000u)

class RenderBundles : public State, public MouseEventListener, public KeyboardEventListener {

	struct Renderable {
		uint32_t geometryIndex;
		WgpBuffer uniformBuffer;
		WGPUBindGroup bindGroup;
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

	std::vector<WGPUBindGroup> OnBindGroups();
	std::vector<WGPUBindGroupLayout> OnBindGroupLayouts();
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);
	std::vector<Shape> m_asteroids;
	std::vector<WgpModel> m_wgpAsteroids;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_replace = false;
	bool m_useRendeBundle = true;

	Camera m_camera;
	TrackBall m_trackball;
	Uniforms m_uniforms;
	Shape m_sphere;
	int m_countAsteroids;

	WgpModel m_wgpSphere;
	WgpBuffer m_uniformBuffer, m_modelBuffer;
	WgpTexture m_saturnTexture, m_moonTexture;
	std::vector<Renderable> m_renderables;
	WGPURenderBundle m_renderBundle;

	void createAsteroid(Renderable& renderable, uint32_t geometryIndex, const Matrix4f& model);
	void placeAsteroids();
	void updateRenderBundle();
};