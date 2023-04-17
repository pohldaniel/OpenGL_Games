#ifndef _MESHTORUS_H
#define _MESHTORUS_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class MeshTorus {

public:

	MeshTorus(int uResolution = 49, int vResolution = 49);
	MeshTorus(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 49, int vResolution = 49);
	MeshTorus(const Vector3f &position, float radius, float tubeRadius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 49, int vResolution = 49);
	~MeshTorus();

	void setPrecision(int uResolution, int vResolution);
	void drawRaw();
	void drawRawInstanced();

	void createInstancesStatic(const std::vector<Matrix4f>& modelMTX);
	void addInstance(const Matrix4f& modelMTX);

	static void BuildMesh(float radius, float tubeRadius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents, std::vector<Vector3f>& normalsDu,	std::vector<Vector3f>& normalsDv);

private:

	int m_uResolution;
	int m_vResolution;
	float m_radius;
	float m_tubeRadius;
	Vector3f m_position;

	bool m_generateNormals;
	bool m_generateTexels;
	bool m_generateTangents;
	bool m_generateNormalDerivatives;

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[7];
	unsigned int m_drawCount;
	unsigned int m_vboInstances = 0;
	unsigned int m_instanceCount = 0;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	std::vector<Vector3f> m_tangents;
	std::vector<Vector3f> m_bitangents;
	std::vector<Vector3f> m_normalsDu;
	std::vector<Vector3f> m_normalsDv;

	std::vector<Matrix4f> m_instances;

	void createBuffer();

};
#endif