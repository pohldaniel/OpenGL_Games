#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include <random>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/ObjModel.h"
#include "engine/TrackBall.h"

#include "Constants.h"
#include "StateMachine.h"

// Grid sizes (in vertices)
#define X_SIZE_LOG2		6
#define Y_SIZE_LOG2		6
#define Z_SIZE_LOG2		6

#define TOTAL_POINTS	(1<<(X_SIZE_LOG2 + Y_SIZE_LOG2 + Z_SIZE_LOG2))
#define CELLS_COUNT		(((1<<X_SIZE_LOG2) - 1) * ((1<<Y_SIZE_LOG2) - 1) * ((1<<Z_SIZE_LOG2) - 1))

#define SWIZZLE	1

#define MAKE_INDEX(x, y, z, sizeLog2)	(GLint)((x) | ((y) << sizeLog2[0]) | ((z) << (sizeLog2[0] + sizeLog2[1])))


enum RenderMode {
	PROG0,
	PROG1,
	PROG2
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
	void applyTransformation(TrackBall& mtx);

	Camera m_camera;

	TrackBall m_trackball;
	Transform m_transform;

	Shader* progGeom[3];
	Shader* prog[3];

	bool m_initUi = true;	
	GLuint position_vbo, index_vbo;
	GLuint edge_tex;
	GLuint volume_tex, noise_tex;
	void UnSwizzle(GLuint index, GLuint sizeLog2[3], GLuint * pPos);
	void GenerateIndices(GLuint *indices, GLuint sizeLog2[3]);
	void GeneratePositions(float *positions, GLuint sizeLog2[3]);
	GLuint loadRawVolume(const char *filename, int w, int h, int d);
	float frand();
	GLuint create_noise_texture(GLenum format, int w, int h, int d);
	void drawTetrahedra();
	void drawLayers();

	GLuint prim = GL_LINES_ADJACENCY;
	int nprims = CELLS_COUNT * 24;

	float isovalue = 1.0f;
	float anim = 0.0f;
	RenderMode renderMode = RenderMode::PROG0;
	bool m_drawLayers = false;
	bool m_withFrag = true;
	bool m_withGeom = true;
	bool m_usePoints = false;
};

