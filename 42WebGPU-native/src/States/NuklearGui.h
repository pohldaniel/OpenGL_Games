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

#define MAX_VERTEX_MEMORY (1024u * 1024u)
#define MAX_INDEX_MEMORY (256u * 1024u)

class NuklearGui : public State, public MouseEventListener, public KeyboardEventListener {

	struct nk_webgpu_vertex {
		float position[2];
		float uv[2];
		uint8_t col[4];
	};

	struct JoystickResult {
		float x = 0.0f; // -1.0 bis 1.0 (Links/Rechts)
		float y = 0.0f; // -1.0 bis 1.0 (Oben/Unten)
		bool is_active = false;
	};

public:

	NuklearGui(StateMachine& machine);
	~NuklearGui();

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
	void renderUi(const WGPURenderPassEncoder& renderPassEncoder);

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_isHovered = false;
	float m_uiScale = 1.0f;
	float m_scrollDelta = 0.0f;
	bool m_wasHovered1 = false;
	bool m_wasHovered2 = false;

	Camera m_camera;
	TrackBall m_trackball;

	WgpBuffer m_vertexBuffer, m_indexBuffer, m_uniformBuffer;
	WgpTexture m_texture, m_textureFont, m_textureIcon;

	WGPURenderBundle m_renderBundle;
	WGPUBindGroup m_bindgroup, m_bindgroupFont, m_bindgroupIcon;

	WGPUBindGroup createBindGroup();
	WGPUBindGroup createBindGroupFont();
	WGPUBindGroup createBindGroupIcon();
	JoystickResult nk_virtual_joystick(struct nk_context* ctx, float size_px);
	bool nk_circular_action_button(struct nk_context* ctx, const char* label, float size_px);
};