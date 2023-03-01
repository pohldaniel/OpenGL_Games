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

#include "Constants.h"
#include "StateMachine.h"
#include "XTree.h"
#include "TerrainNV.h"

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

	void shadowPass();

	void setUpSplitDist(frustum f[MAX_SPLITS], float nd, float fd);

	void updateFrustumPoints(frustum& f, const Vector3f& center, const Vector3f& view_dir);
	float applyCropMatrix(frustum &f, const Matrix4f& lightView, Matrix4f& lightProj);
	Camera m_camera;

	TrackBall m_trackball;
	Transform m_transform;
	Quad* m_quad;
	bool m_initUi = true;	
	Shader* quad;
	Shader* progTerrain[5];
	Shader* progTree[5];
	Texture m_canyi;

	TerrainNV *terrain;
	obj_BoundingSphere obj_BSphere[NUM_OBJECTS];
	frustum f[MAX_SPLITS];
	std::vector<Matrix4f> shad_cpm;
	int cur_num_splits = 2;

	float* m_cascadeEndClipSpace;
	float far_bound[MAX_SPLITS];

	float split_weight = 0.75f;
	void renderScene();
	GLuint depth_fb, depth_rb;
	GLuint depth_tex_ar;
	GLuint	tex;

	bool m_showDepthTex = true;
};

