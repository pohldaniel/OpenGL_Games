#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include <random>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"

#include "engine/MeshObject/MeshQuad.h"
#include "engine/MeshObject/MeshCube.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"
#include "engine/Cube.h"
#include "engine/TrackBall.h"
#include "Cubemap.h"

#include "Constants.h"
#include "StateMachine.h"
#include "FluidSystem.h"
#include "nvidia/HDRImages.h"

#define DOWNSAMPLE_BUFFERS 2
#define BLUR_BUFFERS 2

class Game : public State, public MouseEventListener {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;	
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void applyTransformation(Matrix4f& mtx);
	void drawModel();
	GLuint createCubemapTexture(HDRImage &img, GLint internalformat);
	void renderScene();
	void blurPass(Framebuffer& src);
	void downsample2(Framebuffer& src, Framebuffer& dest);
	void downsample4(Framebuffer& src, Framebuffer& dest);
	void generate1DConvolutionFP_filter(Shader*& shader, float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height);
	float *generateGaussianWeights(float s, int &width);

	Camera m_camera;
	Quad* m_quad;
	Cube* m_cube;

	TrackBall m_trackball;
	Transform m_transform;

	HDRImage image;
	Cubemap m_cubemap;

	Shader* tone;
	Shader* blurH = nullptr;
	Shader* blurV = nullptr;
	Shader* object;
	Shader* down2;
	Shader* down4;
	GLuint vertex;

	ObjModel venus;
	GLuint m_texture;

	Framebuffer sceneBuffer;
	Framebuffer blurBuffer[2];
	Framebuffer downsampleBuffer[2];

	float exposure = 16.0;
	float aniso = 2.0;
	float blur_width = 3.0;
	float blur_amount = 0.5;
	float effect_amount = 0.2;

	bool m_drawSkaybox = false;
};

