#ifndef _MESHDISK_H
#define _MESHDISK_H
#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class MeshDisk {

public:

	MeshDisk(int uResolution = 1, int vResolution = 1);
	MeshDisk(bool generateTexels, bool generateNormals, bool generateTangents, int uResolution = 1, int vResolution = 1);
	MeshDisk(const Vector3f &position, const Vector2f& size, bool generateTexels, bool generateNormals, bool generateTangents, int uResolution = 1, int vResolution = 1);
	~MeshDisk();

	void drawRaw();
	void drawRawInstanced();
	void createInstancesStatic(const std::vector<Matrix4f>& modelMTX);
	void addInstance(const Matrix4f& modelMTX);
	void setPrecision(int uResolution, int vResolution);
	int getNumberOfTriangles();

	const Vector3f &getPosition() const;
	const Vector2f &getSize() const;
	const Vector3f &getCenter() const;
	std::vector<Vector3f>& getPositions();
	std::vector<unsigned int>& getIndexBuffer();

	static void BuildMeshXY(const Vector3f& position, const Vector2f& size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents);
	static void BuildMeshXZ(const Vector3f& position, const Vector2f& size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer, std::vector<Vector3f>& tangents, std::vector<Vector3f>& bitangents);

private:

	int m_uResolution;
	int m_vResolution;

	Vector3f m_position;
	Vector2f m_size;
	Vector3f m_center;

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