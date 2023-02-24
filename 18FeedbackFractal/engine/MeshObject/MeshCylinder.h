#ifndef _MESHCYLINDER_H
#define _MESHCYLINDER_H

#include <GL/glew.h>
#include <vector>
#include <memory>

#include "../Texture.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../Vector.h"

class MeshCylinder {
public:

	MeshCylinder(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);
	MeshCylinder(const Vector3f &position, float baseRadius, float topRadius, float length, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);
	~MeshCylinder();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera& camera);
	void drawRaw();
	void drawRawInstanced();

	void setTransformation(const Matrix4f &transformation) { m_model = transformation; }
	void createInstancesStatic(const std::vector<Matrix4f>& modelMTX);
	void addInstance(const Matrix4f& modelMTX);

private:

	std::vector<float> getSideNormals();

	int m_tubeSegments;
	int m_mainSegments;

	float m_baseRadius;
	float m_topRadius;
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
	std::vector<Matrix4f> m_instances;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;

	Matrix4f m_model;
};

#endif