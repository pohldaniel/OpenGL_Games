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
#include <engine/ObjModel.h>
#include <engine/SlicedCube.h>
#include <engine/Transform.h>

#include <States/StateMachine.h>
#include "Background.h"
#include "VideoReader.h"

#define BLUR_BUFFERS 2

class Game : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Game(StateMachine& machine);
	~Game();

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
	
	void recompileShader();
	void generate1DConvolutionFP_filter(Shader*& shader, float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height);
	float *generateGaussianWeights(float s, int &width);
	void blurPass(Framebuffer& src);
	void createBuffers(AttachmentTex::AttachmentTex texFormat);

	bool m_initUi = true;
	bool m_drawUi = true;
	bool m_useBlue = true;

	Background m_background;
	TrackBall m_trackball;
	Transform m_transform;
	Camera m_camera;

	VideoReaderState vr_state;
	uint8_t* frame_data;
	GLuint tex_handle;
	Clock m_lock;

	int frame_width;
	int frame_height;

	GLuint vertex;
	Shader* blurH = nullptr;
	Shader* blurV = nullptr;
	float m_blurWidth = 3.0f;
	Framebuffer blurBuffer[2];
	Framebuffer m_mainRT;
	float m_color[3] = { 1.0f, 1.0f, 1.0f };
	ZoomableQuad m_zoomableQuad;
	float m_zoomFactor = 1.0f;
};