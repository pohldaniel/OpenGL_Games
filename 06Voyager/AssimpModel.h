#pragma once

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "engine\Vector.h"
#include "engine\Extension.h"
#include "engine\Shader.h"
#include "engine\Texture.h"
#include "engine\Camera.h"
#include "engine\Transform.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices)

class AssimpModel {

public:

	AssimpModel();
	~AssimpModel();

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void rotate(const Vector3f& axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	void drawRaw();

	bool loadModel(const char* filename);

	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);
private:
	Transform m_transform;

	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;

	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_ibo = 0;
	unsigned int m_vboInstances = 0;

	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& drawCount, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo, unsigned int stride);
};