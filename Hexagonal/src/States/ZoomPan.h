#pragma once

#include <fstream>
#include <sstream>

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/TrackBall.h>
#include <engine/Clock.h>
#include <engine/Pixelbuffer.h>
#include <engine/Framebuffer.h>
#include <engine/Spritesheet.h>

#include "StateMachine.h"
#include "Background.h"

#include "GLVertexBuffer.h"
#include "GLImageView.h"

enum Mode {
	NEAREST,
	LINEAR,
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
	void PrepareVertexBuffer();
	void PrepareSingleView();
	void HandleZoom(const short delta);

	Camera m_camera;
	TrackBall m_trackball;
	Transform m_transform;

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_transparentTile = false;
	float m_scale = 1.0f;

	Background m_background;
	Mode mode = Mode::NEAREST;

	float m_fXOffset;
	float m_fYOffset;

	float m_fZoomWidth;
	float m_fZoomHeight;

	//GLTexture m_glTexture;
	GLVertexBuffer m_glVertexBuffer;
	GLVertexBuffer m_glVertexBufferZoom;
	std::vector<GLImageView*> m_ImageView;
	Shader* m_shader;
	GLImageView* pImage;

	bool WithinImageArea(int posX, int posY);
};
