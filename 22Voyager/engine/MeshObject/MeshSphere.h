#ifndef _MESHSPHERE_H
#define _MESHSPHERE_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class MeshSphere {

public:

	MeshSphere(int uResolution = 49, int vResolution = 49);
	MeshSphere(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 49, int vResolution = 49);
	MeshSphere(const Vector3f &position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 49, int vResolution = 49);
	~MeshSphere();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void drawRaw();

private:

	int m_uResolution;
	int m_vResolution;
	float m_radius;

	Vector3f m_position;
	Matrix4f m_model;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;
	bool m_generateNormalDerivatives;

	bool m_isInitialized;
	bool m_hasTexels;
	bool m_hasNormals;
	bool m_hasTangents;
	bool m_hasNormalDerivatives;

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[7];
	unsigned int m_drawCount;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;

	Vector3f m_min;
	Vector3f m_max;
};
#endif