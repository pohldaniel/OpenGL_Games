#ifndef _SHAPE_H
#define _SHAPE_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class Shape {

public:

	Shape();

	void drawRaw();
	void buildCapsule(float radius = 1.0f, float length = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 20, int vResolution = 20, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false, bool generateNormalDerivatives = false);
	void buildTorus(float radius = 0.5f, float tubeRadius = 0.25f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false, bool generateNormalDerivatives = false);
	void buildSphere(float radius = 1.0f, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f), int uResolution = 49, int vResolution = 49, bool generateTexels = true, bool generateNormals = true, bool generateTangents = false, bool generateNormalDerivatives = false);


	void setPrecision(int uResolution, int vResolution);
	int getNumberOfTriangles();

private:

	int m_uResolution;
	int m_vResolution;
	Vector3f m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;
	bool m_generateNormalDerivatives;

	bool m_isInitialized;
	bool m_hasTexels;
	bool m_hasNormals;
	bool m_hasTangents;
	bool m_hasNormalDerivatives;

	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	std::vector<Vector3f> m_tangents;
	std::vector<Vector3f> m_bitangents;
	std::vector<Vector3f> m_normalsDu;
	std::vector<Vector3f> m_normalsDv;
	std::vector<unsigned int> m_indexBuffer;

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[7];
	unsigned int m_drawCount;

	Vector3f m_min;
	Vector3f m_max;

	void createBuffer();
};
#endif