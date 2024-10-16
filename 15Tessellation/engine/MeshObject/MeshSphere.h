#ifndef _MESHSPHERE_H
#define _MESHSPHERE_H
#include <GL/glew.h>
#include <vector>
#include <memory>

#include "../Texture.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../Vector.h"

class MeshSphere {

public:

	MeshSphere(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);
	MeshSphere(const Vector3f &position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);

	MeshSphere(float radius, const std::string &texture);
	MeshSphere(const Vector3f &position, float radius, const std::string &texture);
	MeshSphere(const Vector3f &position, float radius, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives, const std::string &texture);
	~MeshSphere();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera camera);
	void drawRaw();
	void setTransformation(const Matrix4f &transformation) { m_model = transformation; }

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


	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;
};
#endif