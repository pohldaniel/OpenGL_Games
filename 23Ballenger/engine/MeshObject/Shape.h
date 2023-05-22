#ifndef _SHAPE_H
#define _SHAPE_H

#include <GL/glew.h>
#include <vector>
#include <array>
#include <cctype>
#include <iterator>
#include <map>
#include "../Vector.h"

class Shape {

public:

	Shape();
	~Shape();

	void buildCapsule(float radius = 1.0f, float length = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 20, int vResolution = 20, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildTorus(float radius = 0.5f, float tubeRadius = 0.25f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSphere(float radius = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSpiral(float radius = 0.5f, float tubeRadius = 0.25f, float length = 1.5f, int numRotations = 2, bool repeatTexture = true, const Vector3f& position = Vector3f(0.0f, -0.75f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildCylinder(float baseRadius = 1.0f, float topRadius = 1.0f, float length = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 10, int vResolution = 10, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXY(const Vector3f& position = Vector3f(-1.0f, -1.0f, 0.0f), const Vector2f& size = Vector2f(2.0f, 2.0f),  int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXZ(const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), const Vector2f& size = Vector2f(2.0f, 2.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildCube(const Vector3f& position = Vector3f(-1.0f, -1.0f, -1.0f), const Vector3f&  size = Vector3f(2.0f, 2.0f, 2.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildDiskXY(float radius = 1.0f, const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildDiskXZ(float radius = 1.0f, const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	
	//void fromBuffer(std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int> indexBuffer);
	void fromBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int stride);
	void fromObj(const char* filename);

	void drawRaw() const;
	void drawRawInstanced() const;
	void addInstances(const std::vector<Matrix4f>& modelMTX);
	void addInstance(const Matrix4f& modelMTX);
	void addVec4Attribute(const std::vector<Vector4f>& modelMTX, unsigned int divisor = 0);
	int getNumberOfTriangles();
	void cleanup();
	std::vector<Vector3f>& getPositions();
	std::vector<unsigned int>& getIndexBuffer();

private:

	unsigned int m_vao;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_drawCount;
	unsigned int m_vboInstances = 0;
	unsigned int m_vboColor = 0;
	unsigned int m_instanceCount = 0;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	std::vector<Vector3f> m_tangents;
	std::vector<Vector3f> m_bitangents;
	std::vector<Matrix4f> m_instances;

	Vector3f m_min;
	Vector3f m_max;

	void createBuffer();
	int whitespaces(const char c[]);
	int addVertex(int hash, const float *pVertex, int stride, std::map<int, std::vector<int>>& vertexCache, std::vector <float>& vertexBuffer);
};
#endif