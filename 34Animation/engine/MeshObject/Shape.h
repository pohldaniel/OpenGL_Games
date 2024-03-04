#ifndef _SHAPE_H
#define _SHAPE_H

#include <GL/glew.h>
#include <vector>
#include <array>
#include <cctype>
#include <iterator>
#include <map>
#include "../Vector.h"
#include "../BoundingBox.h"

class Shape {

public:

	Shape();
	Shape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride, bool createBuffer = true);
	Shape(Shape const& rhs);
	Shape(Shape&& rhs);
	Shape& operator=(const Shape& rhs);
	Shape& operator=(Shape&& rhs);
	~Shape();

	void buildCapsule(float radius = 1.0f, float length = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 20, int vResolution = 20, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildTorus(float radius = 0.5f, float tubeRadius = 0.25f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSphere(float radius = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildSpiral(float radius = 0.5f, float tubeRadius = 0.25f, float length = 1.5f, int numRotations = 2, bool repeatTexture = true, const Vector3f& position = Vector3f(0.0f, -0.75f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildCylinder(float baseRadius = 1.0f, float topRadius = 1.0f, float length = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), bool top = true, bool bottom = true, int uResolution = 10, int vResolution = 10, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXY(const Vector3f& position = Vector3f(-1.0f, -1.0f, 0.0f), const Vector2f& size = Vector2f(2.0f, 2.0f),  int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildQuadXZ(const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), const Vector2f& size = Vector2f(2.0f, 2.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildDiamondXY(const Vector2f& size = Vector2f(2.0f, 2.0f), float border = 0.75f, int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);

	
	void buildCube(const Vector3f& position = Vector3f(-1.0f, -1.0f, -1.0f), const Vector3f&  size = Vector3f(2.0f, 2.0f, 2.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildDiskXY(float radius = 1.0f, const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	void buildDiskXZ(float radius = 1.0f, const Vector3f& position = Vector3f(-1.0f, 0.0f, -1.0f), int uResolution = 1, int vResolution = 1, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false);
	
	//void fromBuffer(std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer);
	void fromBuffer(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, unsigned int stride, bool createBuffer = true);
	void fromBuffer(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, unsigned int stride, const std::vector<std::array<float, 4>>& weights, const std::vector<std::array<unsigned int, 4>>& boneIds);
	void fromObj(const char* filename);

	void drawRaw() const;
	void drawRawInstanced() const;
	void drawAABB() const;

	void addInstance(const Matrix4f& value, unsigned int divisor = 1u);
	void removeInstance(unsigned int index);
	void insertInstance(const Matrix4f& value, unsigned int index);

	void addInstances(const std::vector<Matrix4f>& values, unsigned int divisor = 1u);
	void setInstances(const std::vector<Matrix4f>& values, unsigned int divisor = 1u);
	void setInstances(unsigned int length, unsigned int divisor = 1u, unsigned int usage = 35048u /*GL_DYNAMIC_DRAW*/);
	void updateInstances(const std::vector<Matrix4f>& values);
	void updateInstance(const Matrix4f& mtx, unsigned int index);

	void setVec4Attribute(const std::vector<Vector4f>& values, unsigned int divisor = 1u);
	void setMat4Attribute(const std::vector<Matrix4f>& values, unsigned int divisor = 1u);
	void setFloatAttribute(const std::vector<float>& values, unsigned int divisor = 1u);

	void setMat4Attribute(unsigned int length, unsigned int divisor = 1u, unsigned int usage = 35048u);
	void updateMat4Attribute(const std::vector<Matrix4f>& values);

	void setFloatAttribute(unsigned int length, unsigned int divisor = 1u, unsigned int usage = 35048u);
	void updateFloatAttribute(const std::vector<float>& values);

	unsigned int getNumberOfTriangles();
	std::vector<Vector3f>& getPositions();
	std::vector<unsigned int>& getIndexBuffer();
	const std::vector<Matrix4f>& getInstances() const;
	BoundingBox& getAABB() const;

	void cleanup();
	void markForDelete();
	void createBoundingBox();

private:

	unsigned int m_vao;
	unsigned int m_vbo[7] = { 0 };
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
	std::vector<std::array<unsigned int, 4>> m_boneIds;
	std::vector<std::array<float, 4>> m_weights;
	std::vector<Matrix4f> m_instances;
	mutable BoundingBox m_aabb;
	bool m_markForDelete;

	void createBuffer();
	int whitespaces(const char c[]);
	int addVertex(int hash, const float *pVertex, int stride, std::map<int, std::vector<int>>& vertexCache, std::vector <float>& vertexBuffer);
};
#endif