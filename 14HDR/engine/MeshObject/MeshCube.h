#ifndef _MESHCUBE_H
#define _MESHCUBE_H

#include <GL/glew.h>
#include <vector>
#include <memory>

#include "../Texture.h"
#include "../Camera.h"
#include "../Shader.h"
#include "../Vector.h"
#include "../Transform.h"
#include "../../Constants.h"

class MeshCube {

public:

	MeshCube(float width, float height, float depth);
	MeshCube(const Vector3f &position, float width, float height, float depth);
	MeshCube(const Vector3f &position, float width, float height, float depth, bool generateTexels, bool generateNormals);
	~MeshCube();

	void setPrecision(int uResolution, int vResolution);
	void buildMesh();
	void buildMesh4Q();
	void draw(const Camera camera);
	void drawShadow(const Camera camera);
	void update(float dt);
	void dissolve();

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
	float m_depth;
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
	unsigned int m_vbo[4];
	unsigned int m_drawCount;
	unsigned int m_numberOfTriangle;
	Transform m_transform;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;

	Texture* m_gradient;

	const float m_transitionSpeed = 1.5f;
	bool m_transitionEnd = false;
	bool m_fadeIn = false;	
	bool m_fadeOut = true;
	float m_dissolveAmount = 0.0f;
};
#endif