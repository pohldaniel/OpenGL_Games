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
#include "engine/MeshObject/MeshCylinder.h"
#include "engine/MeshObject/Capsule.h"

#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"
#include "engine/Cube.h"
#include "engine/Tetraedron.h"
#include "engine/TrackBall.h"
#include "engine/Texture.h"
#include "Cubemap.h"

#include "Constants.h"
#include "StateMachine.h"

#define DOWNSAMPLE_BUFFERS 2
#define BLUR_BUFFERS 2

enum Model {	
	QUAD,
	SPHERE,
	TETRA, 
	CUBE,
	TORUS,
	SPIRAL,
	CYLINDER,
	CONE,
	CAPSULE,
	VENUS
};

enum Culling {
	NONE,
	FRONT,
	BACK
};

enum RenderMode {
	HDR1,
	HDR2,
	NORMAL,
	TEXTURE
};

struct aaInfo {
	int samples;
	int coverage_samples;
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
	void applyTransformation(TrackBall& arc);
	void drawModel();

	void renderScene();
	void blurPass(Framebuffer& src);
	void downsample2(Framebuffer& src, Framebuffer& dest);
	void downsample4(Framebuffer& src, Framebuffer& dest);
	void recompileShader();
	void generate1DConvolutionFP_filter(Shader*& shader, float *weights, int width, bool vertical, bool tex2D, int img_width, int img_height);
	float *generateGaussianWeights(float s, int &width);
	void createBuffers(AttachmentTex::AttachmentTex texFormat, AttachmentRB::AttachmentRB rbFormat, aaInfo aaMode);
	void panoramaToCubemap();

	Camera m_camera;
	Quad* m_quad;
	Cube* m_cube;
	Tetraedron* m_tetraedron;

	MeshSphere* m_sphere;
	MeshSphere* m_midpoint;
	MeshTorus* m_torus;
	MeshSpiral* m_spiral;
	MeshCube* m_mcube;
	MeshQuad* m_mquad;
	MeshCylinder* m_cylinder;
	MeshCylinder* m_cone;
	Capsule* m_capsule;

	TrackBall m_trackball;
	Transform m_transform;

	Cubemap m_cubemap;
	Texture m_hdriCross;
	Texture m_hdriEnv;
	unsigned int m_cubemapEnv;
	unsigned int cubemap;

	Shader* tone;
	Shader* blurH = nullptr;
	Shader* blurV = nullptr;
	Shader* object;
	Shader* down2;
	Shader* down4;
	Shader* normal;
	Shader* texture;
	Shader* panToCube;

	GLuint vertex;
	ObjModel venus;

	Framebuffer sceneBuffer;
	Framebuffer blurBuffer[2];
	Framebuffer downsampleBuffer[2];
	Framebuffer msaaBuffer;
	Framebuffer cubeBuffer;

	float m_exposure = 16.0f;
	float m_aniso = 2.0f;
	float m_blurWidth = 3.0f;
	float m_blurAmount = 0.5f;
	float m_effectAmount = 0.2f;
	float m_blendAmout = 0.5f;
	float m_scale = 1.0f;

	bool m_glow = true;
	bool m_rays = true;
	bool m_drawSkaybox = true;
	bool m_wireframe = false;
	bool m_blend = false;
	bool m_initUi = true;
	bool m_drawUi = true;

	Model model = Model::VENUS;
	int currentBuffer = 1;

	AttachmentTex::AttachmentTex bufferTokens[4] = { AttachmentTex::AttachmentTex::RGBA, AttachmentTex::AttachmentTex::RGBA16F, AttachmentTex::AttachmentTex::RGBA32F, AttachmentTex::AttachmentTex::R11FG11FB10F };
	AttachmentRB::AttachmentRB rbTokens[4] = { AttachmentRB::AttachmentRB::RGBA, AttachmentRB::AttachmentRB::RGBA16F, AttachmentRB::AttachmentRB::RGBA32F, AttachmentRB::AttachmentRB::R11FG11FB10F };

	int currentMode = 2;
	aaInfo aaModes[6] = { { 0,0 },{ 2,2 },{ 4,4 },{ 4,8 },{ 8,8 },{ 8,16 } };
	const char* aaModesLabel[6] = { "None", "2x", "4x", "8xCSAA", "8x", "16xCSAA" };

	int currentAnsio = 1;
	float ansio[5] = {1.0f, 2.0f, 4.0f, 8.0f, 16.0f};
	const char* ansioLabel[5] = { "1.0", "2.0", "4.0", "8.0", "16.0"};

	Culling cullMode = Culling::NONE;

	RenderMode renderMode = RenderMode::HDR1;

	float m_color[3] = {0.05f, 0.2f, 0.05f};

	Vector3f m_translate;
	Vector3f m_centerOfRotation;
	float m_exposureMax = 512.0f;
};

