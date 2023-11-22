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
#include "engine/Cube.h"
#include "engine/Frustum.h"
#include "engine/MeshObject/MeshCube.h"

#include "Constants.h"
#include "StateMachine.h"
#include "TerrainNV.h"

enum Program {
	BASE,
	SPLITS,
	SMOOTHSHADOW,
	SMOOTHSHADOWNL,
	PCF,
	PCFTRILINEAR,
	PCFTAP4,
	PCFTAPRANDOM,
	PCFGAUSSIAN
};

struct obj_BoundingSphere {
	Vector3f center;
	float radius;
};

struct frustum {
	float neard;
	float fard;
	float fov;
	float ratio;
	Vector3f point[8];
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
	void regenerateDepthTex(GLuint depth_size);
	void overviewCam();
	void shadowPass();

	void setUpSplitDist(frustum f[MAX_SPLITS], float nd, float fd);
	void rotateLight(Vector3f& light, float angle, const Vector3f& direction);

	void updateFrustumPoints(frustum& f, const Vector3f& center, const Vector3f& view_dir);
	float applyCropMatrix(frustum &f, const Matrix4f& lightView, Matrix4f& lightProj);
	Camera m_camera;

	TrackBall m_trackball;
	Transform m_transform;
	Quad* m_quad;
	MeshCube* m_cube;
	Frustum* m_frustum;

	bool m_initUi = true;	
	Shader* quad;
	Shader* progTerrain[9];
	Shader* progTree[9];
	Shader* prog0;
	Texture m_canyi;

	TerrainNV *terrain;
	obj_BoundingSphere obj_BSphere[NUM_OBJECTS];
	frustum f[MAX_SPLITS];
	std::vector<Matrix4f> shad_cpm;
	std::vector<Matrix4f> shadow;
	int cur_num_splits = 2;

	float* m_cascadeEndClipSpace;
	float far_bound[MAX_SPLITS];

	float split_weight = 0.75f;
	void renderScene();
	GLuint depth_fb, depth_rb;
	GLuint depth_tex_ar;
	GLuint	tex;

	bool m_showDepthTex = false;
	bool m_rotateLight = false;

	Program program = Program::BASE;
	Shader* activeProgramTerrain;
	Shader* activeProgramTree;
	int v = 2;

	char * items[4] = { "512", "1024", "2048" , "4096" };
	Vector3f centerNear;
	float degree;
	Vector3f light_dir = Vector3f( 0.2f, 0.99f, 0.0f);
};

