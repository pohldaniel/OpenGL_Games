#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>
#include <engine/ZoomableQuad.h>

#include "StateMachine.h"
#include "Background.h"

enum Filter {
	NEAREST,
	LINEAR
};

enum Mode {
	NONE,
	BILINEAR,
	BICUBIC_TRIANGULAR,
	BICUBIC_BELL_SHAPED,
	BICUBIC_B_SPLINE,
	CATMULL_ROM_SPLINE
};

class ZoomPan : public State, public MouseEventListener, public KeyboardEventListener {

public:

	ZoomPan(StateMachine& machine);
	~ZoomPan();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

private:

	void applyTransformation(TrackBall& arc);
	void renderUi();
	void DrawZoomedImage();
	void DrawActualImage();
	void HandleZoom(const short delta);
	bool WithinImageArea(int posX, int posY);

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;

	Background m_background;
	Mode mode = Mode::NONE;
	Filter filter = Filter::NEAREST;

	float m_fXOffset;
	float m_fYOffset;

	float m_fZoomWidth;
	float m_fZoomHeight;

	Shader* m_shader;
	ZoomableQuad m_zoomableQuad;

	Texture* m_pTexture;
	Shader* m_pShader;
};
