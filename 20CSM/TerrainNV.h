#pragma once

#include <vector>
#include <GL/glew.h>

#include "engine/Vector.h"
#include "engine/Texture.h"
#include "engine/ObjModel.h"
#include "engine/Camera.h"
#include "Constants.h"

#define FAR_DIST 200.0f
#define MAX_SPLITS 4
#define NUM_OBJECTS 4
#define LIGHT_FOV 45.0

#define SCALE 0.2f;
#define MODEL_Y_TRANSLATE -0.1f
#define MODEL_HEIGHT 3.0f

extern float cam_pos[3];
extern float cam_view[3];
extern GLfloat light_dir[4];
extern int depth_size;

class TerrainNV {

public:
	TerrainNV();
	~TerrainNV();
	bool	Load();
	void	Draw(const Camera& camera);
	void	Draw(float minCamZ, const Matrix4f& proj, const Matrix4f& view);
	void    DrawRawTerrain();
	void    DrawRawTrunk();
	void    DrawRawLeave();
	void	DrawCoarse();
	int		getDim() { return (width>height) ? width : height; }

	void	MakeTerrain();
	void	LoadTree();
	void	DrawTree();

	GLuint	tex;
	float	*heights;
	float	*normals;


	int		height;
	int		width;

	ObjModel modelT;
	ObjModel modelL;

	GLuint	vboIdT;
	GLuint	eboIdT;
	GLuint	vboIdL;
	GLuint	eboIdL;

	GLuint m_ibo;
	GLuint m_vbo;
	GLuint m_vao;
	unsigned int m_totalIndices;

	GLuint	terrain_list;

	Texture m_canyi;
	Texture m_canyd;
	Texture m_ent;

	std::vector<Matrix4f> m_instances;
};
