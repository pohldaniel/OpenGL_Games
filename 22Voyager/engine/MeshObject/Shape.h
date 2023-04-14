#ifndef _SHAPE_H
#define _SHAPE_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class Shape {
	Shape();

	void drawRaw();
	void buildCapsule();

	void setPrecision(int uResolution, int vResolution);
	int getNumberOfTriangles();

private:

	int m_uResolution;
	int m_vResolution;
	//Vector3f m_position;

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