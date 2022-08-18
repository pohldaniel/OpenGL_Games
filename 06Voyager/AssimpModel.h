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
#include "engine\AssetManger.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices)

class AssimpMesh;
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

	std::string getModelDirectory();

	std::vector<AssimpMesh*> m_mesh;

private:
	unsigned int m_numberOfMeshes = 0;
	std::string m_modelDirectory;

	Transform m_transform;

	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;
	unsigned int m_materialCount = 0;

	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_ibo = 0;
	unsigned int m_vboInstances = 0;

	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& drawCount, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo, unsigned int stride);

	AssetManager<Texture> m_textureManager;
	std::unordered_map<unsigned int, Texture> m_textures;
};

class AssimpMesh {

	friend AssimpModel;

public:

	AssimpMesh(AssimpModel* model);
	~AssimpMesh();

	void drawRaw();

	AssimpModel* m_model;

	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_vboInstances = 0;
	unsigned int m_ibo = 0;

	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_numberOfTriangles, m_baseVertex, m_baseIndex;
};