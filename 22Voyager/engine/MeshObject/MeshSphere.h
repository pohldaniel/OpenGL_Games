#ifndef _MESHSPHERE_H
#define _MESHSPHERE_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class MeshSphere {

public:

	MeshSphere(int uResolution = 49, int vResolution = 49);
	MeshSphere(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution = 49, int vResolution = 49);
	MeshSphere(const Vector3f &position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution = 49, int vResolution = 49);
	~MeshSphere();

	void drawRaw();
	void drawRawInstanced();
	void createInstancesStatic(const std::vector<Matrix4f>& modelMTX);
	void addInstance(const Matrix4f& modelMTX);
	void setPrecision(int uResolution, int vResolution);
	int getNumberOfTriangles();

	static void BuildMesh(float radius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents);

private:

	int m_uResolution;
	int m_vResolution;
	float m_radius;

	Vector3f m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[5];
	unsigned int m_drawCount;
	unsigned int m_vboInstances = 0;
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
};
#endif