#ifndef _MESHQUAD_H
#define _MESHQUAD_H

#include <vector>
#include <memory>

#include "..\Texture.h"
#include "..\Extension.h"
#include "..\Camera.h"
#include "..\Shader.h"
#include "..\Vector.h"
#include "..\Transform.h"

#include "..\..\Constants.h"

class MeshQuad {

public:

	MeshQuad(float width, float height, float groundLevel);
	MeshQuad(float width, float height, float groundLevel, bool generateTexels, bool generateNormals);
	~MeshQuad();

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	void setTransformation(const Matrix4f& model);

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera camera);
	void setShader(Shader* shader);
	void setTexture(Texture* texture);
	int getNumberOfTriangles();

	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	Vector3f m_offset;

private:

	int m_uResolution;
	int m_vResolution;
	float m_width;
	float m_height;
	float m_groundLevel;

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
	unsigned int m_vbo[4];
	unsigned int m_drawCount;
	unsigned int m_numberOfTriangle;
	Transform m_transform;
	Matrix4f _modelMatrix;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;

};
#endif