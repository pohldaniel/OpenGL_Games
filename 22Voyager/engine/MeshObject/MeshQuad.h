#ifndef _MESHQUAD_H
#define _MESHQUAD_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class MeshQuad {

public:

	MeshQuad(int uResolution = 1, int vResolution = 1);
	MeshQuad(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 1, int vResolution = 1);
	MeshQuad(const Vector3f &position, const Vector2f& size, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 1, int vResolution = 1);
	~MeshQuad();

	const Vector3f &getPosition() const;
	const Vector2f &getSize() const;
	const Vector3f &getCenter() const;

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void drawRaw();
	int getNumberOfTriangles();

private:

	int m_uResolution;
	int m_vResolution;
	
	Vector3f m_position;
	Vector2f m_size;
	Vector3f m_center;

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
	unsigned int m_numberOfTriangle;

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
};
#endif