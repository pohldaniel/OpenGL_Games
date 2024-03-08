#ifndef _MODELS_HPP_
#define _MODELS_HPP_


#include <GL/glew.h>
#include "XYZ.h"
#include "Utils/binio.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

//
// Textures List
//
typedef struct{
	long xsz, ysz;
	GLubyte* txt;
} ModelTexture;

//
// Model Structures
//

class TexturedTriangle{
public:
	short vertex[3];
	float gx[3], gy[3];
	XYZ facenormal;
};

#define max_model_decals 300

enum ModelType{
	nothing = 0,
	notextype = 1,
	rawtype = 2,
	decalstype = 3,
	normaltype = 4
};

class Model{
public:
	short vertexNum;

	ModelType type;

	int* owner;
	XYZ* vertex;
	XYZ* normals;
	std::vector<TexturedTriangle> Triangles;
	GLfloat* vArray;

	//TextureLu textureptr;
	ModelTexture modelTexture;
	bool color;

	XYZ boundingspherecenter;
	float boundingsphereradius;

	bool flat;

	Model();
	~Model();

	
	const XYZ& getTriangleVertex(unsigned triangleId, unsigned vertexId) const;

	int SphereCheck(XYZ* p1, float radius, XYZ* p, XYZ* move, float* rotate);
	int SphereCheckPossible(XYZ* p1, float radius, XYZ* move, float* rotate);
	int LineCheck(XYZ* p1, XYZ* p2, XYZ* p, XYZ* move, float* rotate);
	int LineCheckPossible(XYZ* p1, XYZ* p2, XYZ* p, XYZ* move, float* rotate);
	int LineCheckSlidePossible(XYZ* p1, XYZ* p2, XYZ* move, float* rotate);
	void UpdateVertexArray();
	void UpdateVertexArrayNoTex();
	void UpdateVertexArrayNoTexNoNorm();
	bool loadnotex(const std::string& filename);
	bool loadraw(const std::string& filename);
	bool load(const std::string& filename);
	bool loaddecal(const std::string& filename);
	void Scale(float xscale, float yscale, float zscale);
	void FlipTexCoords();
	void UniformTexCoords();
	void ScaleTexCoords(float howmuch);
	void ScaleNormals(float xscale, float yscale, float zscale);
	void Translate(float xtrans, float ytrans, float ztrans);
	void CalculateNormals(bool facenormalise);
	void draw(bool skip = true);
	void drawimmediate();
	void Rotate(float xang, float yang, float zang);

private:
	void deallocate();
	/* indices of triangles that might collide */
	std::vector<unsigned int> possible;
};

#endif
