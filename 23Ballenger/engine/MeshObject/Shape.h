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
	void buildCylinder(float baseRadius = 1.0f, float topRadius = 1.0f, float length = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), bool top = true, bool bottom = true, int uResolution = 10, int vResolution = 10, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXY(const Vector3f& position = Vector3f(-1.0f, -1.0f, 0.0f), const Vector2f& size = Vector2f(2.0f, 2.0f),  int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXZ(const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), const Vector2f& size = Vector2f(2.0f, 2.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildCube(const Vector3f& position = Vector3f(-1.0f, -1.0f, -1.0f), const Vector3f&  size = Vector3f(2.0f, 2.0f, 2.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildDiskXY(float radius = 1.0f, const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildDiskXZ(float radius = 1.0f, const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	
	//void fromBuffer(std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer);
	void fromBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride);
	void fromObj(const char* filename);

	void drawRaw() const;
	void drawRawInstanced() const;
	void addInstance(const Matrix4f& value, unsigned int divisor = 1u);
	void removeInstance(unsigned int index);
	void insertInstance(const Matrix4f& value, unsigned int index);

	void addInstances(const std::vector<Matrix4f>& values, unsigned int divisor = 1u);
	void addInstances(unsigned int length, unsigned int divisor = 1u, unsigned int usage = 35048u /*GL_DYNAMIC_DRAW*/);
	void updateInstances(const std::vector<Matrix4f>& values);
	void updateInstance(const Matrix4f& mtx, unsigned int index);

	void addVec4Attribute(const std::vector<Vector4f>& values, unsigned int divisor = 1u);
	void addMat4Attribute(const std::vector<Matrix4f>& values, unsigned int divisor = 1u);
	void addFloatAttribute(const std::vector<float>& values, unsigned int divisor = 1u);

	void addMat4Attribute(unsigned int length, unsigned int divisor = 1u, unsigned int usage = 35048u);
	void updateMat4Attribute(const std::vector<Matrix4f>& values);

	void addFloatAttribute(unsigned int length, unsigned int divisor = 1u, unsigned int usage = 35048u);
	void updateFloatAttribute(const std::vector<float>& values);

	unsigned int getNumberOfTriangles();
	std::vector<Vector3f>& getPositions();
	std::vector<unsigned int>& getIndexBuffer();
	const std::vector<Matrix4f>& getInstances() const;
	void cleanup();

private:

	unsigned int m_vao;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_drawCount;
	unsigned int m_vboInstances = 0;
	unsigned int m_vboAdd1 = 0;
	unsigned int m_vboAdd2 = 0;
	unsigned int m_vboAdd3 = 0;
	unsigned int m_instanceCount = 0;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	std::vector<Vector3f> m_tangents;
	std::vector<Vector3f> m_bitangents;
	std::vector<Matrix4f> m_instances;

	void createBuffer();
	int whitespaces(const char c[]);
	int addVertex(int hash, const float *pVertex, int stride, std::map<int, std::vector<int>>& vertexCache, std::vector <float>& vertexBuffer);
};
#endif