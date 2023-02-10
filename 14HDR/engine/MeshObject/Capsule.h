#ifndef _CAPSULE_H
#define _CAPSULE_H

#include <GL/glew.h>
#include <vector>
#include <memory>

#include "../Texture.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../Vector.h"

class Capsule {
public:

	Capsule(const Vector3f &position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);	
	Capsule(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);
	Capsule(const Vector3f &position, float radius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture);
	Capsule(const Vector3f &position, float radius, float length, const std::string &texture);
	Capsule(float radius, float length, const std::string &texture);
	~Capsule();

	void drawRaw();
	void setPrecision(int uResolution, int vResolution);

private:

	void buildMesh();
	void buildHemisphere(const Vector3f &offset, bool north);
	void buildCylinder();
	std::vector<float> getSideNormals();

	int m_uResolution;
	int m_vResolution;
	float m_radius;
	float m_invRadius;
	float m_length;
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

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;
};

#endif
