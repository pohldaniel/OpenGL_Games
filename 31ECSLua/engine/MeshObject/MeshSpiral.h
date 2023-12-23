#ifndef _MESHSPIRAL_H
#define _MESHSPIRA_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class MeshSpiral {

public:

	MeshSpiral(int uResolution = 49, int vResolution = 49);
	MeshSpiral(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution = 49, int vResolution = 49);
	MeshSpiral(const Vector3f &position, float radius, float tubeRadius, int numRotations, float length, bool repeatTexture, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution = 49, int vResolution = 49);
	~MeshSpiral();

	void drawRaw();
	void drawRawInstanced();
	void createInstancesStatic(const std::vector<Matrix4f>& modelMTX);
	void addInstance(const Matrix4f& modelMTX);
	void setPrecision(int uResolution, int vResolution);
	int getNumberOfTriangles();
	std::vector<Vector3f>& getPositions();
	std::vector<unsigned int>& getIndexBuffer();

	static void BuildMesh(float radius, float tubeRadius, float length, int numRotations, bool repeatTexture, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents);

private:

	int m_uResolution;
	int m_vResolution;

	float m_radius;
	float m_tubeRadius;
	float m_length;
	int m_numRotations;
	bool m_repeatTexture;
	
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

	void createBuffer();
};
#endif