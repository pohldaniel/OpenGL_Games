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
#include "engine/MeshObject/MeshSphere.h"
#include "engine/MeshObject/MeshTorus.h"
#include "engine/MeshObject/MeshSpiral.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"
#include "engine/Cube.h"
#include "engine/Tetraedron.h"
#include "engine/TrackBall.h"
#include "Cubemap.h"

#include "Constants.h"
#include "StateMachine.h"
#include "FluidSystem.h"
#include "nvidia/HDRImages.h"

#define DOWNSAMPLE_BUFFERS 2
#define BLUR_BUFFERS 2

enum Model {	
	SPHERE,
	TETRA, 
	CUBE,
	TORUS,
	SPIRAL,
	VENUS
};

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
	void renderUi();
	void applyTransformation(Matrix4f& mtx);
	void drawModel();
	GLuint createCubemapTexture(HDRImage &img, GLint internalformat);
	void renderScene();
	void blurPass(Framebuffer& src);
	void downsample2(Framebuffer& src, Framebuffer& dest);
	void downsample4(Framebuffer& src, Framebuffer& dest);
	void recompileShader();
	void generate1DConvolutionFP_filter(Shader*& shader, float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height);
	float *generateGaussianWeights(float s, int &width);

	Camera m_camera;
	Quad* m_quad;
	Cube* m_cube;
	Tetraedron* m_tetraedron;

	MeshSphere* m_sphere;
	MeshTorus* m_torus;
	MeshSpiral* m_spiral;

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

	float exposure = 16.0f;
	float aniso = 2.0f;
	float m_blurWidth = 3.0f;
	float m_blurAmount = 0.5f;
	float effect_amount = 0.2f;

	bool m_glow = true;
	bool m_rays = true;
	bool m_drawSkaybox = false;
	bool m_wireframe = false;

	bool m_initUi = true;

	Model model = Model::VENUS;
};

