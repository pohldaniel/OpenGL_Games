#ifndef _CAPSULE_H
#define _CAPSULE_H

#include <GL/glew.h>
#include <vector>

#include "../Vector.h"

class Capsule {

public:

	Capsule(int uResolution = 20, int vResolution = 20);
	Capsule(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 20, int vResolution = 20);
	Capsule(const Vector3f &position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, int uResolution = 20, int vResolution = 20);
	
	~Capsule();

	void drawRaw();
	void setPrecision(int uResolution, int vResolution);
	

	int getNumberOfTriangles();


	static void BuildMesh(const Vector3f& position, float radius, float length, int uResolution, int vResolution, bool generateTexels, bool generateNormals, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer);

private:

	int m_uResolution;
	int m_vResolution;
	float m_radius;
	float m_length;
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
	std::vector<unsigned int> m_indexBuffer;

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[7];
	unsigned int m_drawCount;

	Vector3f m_min;
	Vector3f m_max;

	void createBuffer();

	static void BuildHemisphere(const Vector3f& position, float radius, float length, const Vector3f &offset, bool north, int uResolution, int vResolution, bool generateTexels, bool generateNormals, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer);
	static void BuildCylinder(const Vector3f& position, float radius, float length, int uResolution, int vResolution, bool generateTexels, bool generateNormals, std::vector<Vector3f>& positions, std::vector<Vector2f>& texels, std::vector<Vector3f>& normals, std::vector<unsigned int>& indexBuffer);
	static std::vector<float> GetSideNormals(int uResolution);
};

#endif
