#define NOMINMAX
#pragma once
#include <algorithm>
#include <array>
#include <random>

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Camera.h"
#include "engine/TrackBall.h"

#include "Constants.h"
#include "StateMachine.h"

enum Program {
	KOCH,
	LEAVE,
	TERRAIN
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

	bool m_initUi = true;	

	Shader* prog[3];
	Shader* draw[2];
	float frand();
	GLuint create_rand_texture(GLenum target, GLenum format, int w, int h);
	void init();
	void init_koch();
	void init_terrain();
	void subdivideBuffer(GLuint buffer, GLenum prim, int verts);
	void drawBuffer(GLuint buffer, GLenum prim, int verts);
	void do_feedback();

	GLuint vbo[2];
	int current_buffer = 0;
	const int max_buffer_verts = 1 << 20;
	GLuint primitives_written = 1;
	GLuint query;
	int verts_per_prim = 2;
	GLenum render_prim = GL_LINES, input_prim = GL_LINES, feedback_prim = GL_LINES;
	int prim_output_div = 1;

	GLuint rand_tex;
	float init_scale = 0.5;
	float rand_scale;
	float rand_xform_scale = 0.5;
	float rand_xform_offset = 0.5;

	int step = 0;
	int max_steps = 8;
	int m_steps = 8;
	int seed = 1973;

	bool m_enableLighting = true;
	bool m_continuousSubdivision = true;
	Program program = Program::KOCH;
	Shader* activeProgram;
};

