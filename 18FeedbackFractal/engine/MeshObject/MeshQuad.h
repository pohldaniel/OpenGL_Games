#ifndef _MESHQUAD_H
#define _MESHQUAD_H

#include <GL/glew.h>
#include <vector>
#include <memory>

#include "../Texture.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../Vector.h"
#include "../Transform.h"

class MeshQuad {

public:

	MeshQuad(bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);

	MeshQuad(const Vector3f &position, const Vector2f& size);
	MeshQuad(const Vector3f &position, const Vector2f& size, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives);
	~MeshQuad();

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	void setTransformation(const Matrix4f& model);

	const Vector3f &getPosition() const;
	const Vector2f &getSize() const;
	const Vector3f &getCenter() const;

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void draw(const Camera camera);
	void drawRaw();
	void setShader(Shader* shader);
	void setTexture(Texture* texture);
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


	Transform m_transform;
	Matrix4f m_model;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;

};
#endif