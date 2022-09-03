#pragma once

#include <functional>
#include <numeric>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Vector.h"
#include "Extension.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"
#include "AssetManger.h"
#include "BuiltInShader.h"
#include "ObjModel.h"

#define ASSIMP_LOAD_FLAGS (aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices)

class AssimpMesh;
class AssimpModel {

public:

	struct Material {
		float ambient[4];
		float diffuse[4];
		float specular[4];
		float shininess;
		std::string diffuseTexPath;
		std::string bumpMapPath;
		std::string displacementMapPath;
	};

	AssimpModel();
	~AssimpModel();

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void rotate(const Vector3f& axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	void drawRaw();
	void drawRawInstanced();
	void drawRawStacked();
	void drawRawInstancedStacked();

	void draw(Camera& camera);
	void drawInstanced(Camera& camera);
	void drawStacked(Camera& camera);
	void drawInstancedStacked(Camera& camera);

	void createAABB();

	void drawAABB();

	bool loadModel(const char* filename, bool isStacked = false);

	std::string getModelDirectory();
	BoundingBox& getAABB();
	Transform& getTransform();
	std::vector<AssimpMesh*> getMeshes();

	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	
	void initAssets(bool instanced = false);
	void initAssets(AssetManager<Shader>& shaderManager, AssetManager<Texture>& textureManager, bool instanced = false);
	
private:
	
	unsigned int m_numberOfVertices, m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;
	bool m_isStacked;

	std::vector<AssimpMesh*> m_meshes;
	std::string m_modelDirectory;

	Vector3f m_center;
	BoundingBox aabb;

	std::unordered_map<int, Shader*> m_shader;
	std::unordered_map<int, Texture> m_textures;

	AssetManager<Shader> m_shaderManager;
	AssetManager<Texture> m_textureManager;

	Transform m_transform;

	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_ibo = 0;
	unsigned int m_vboInstances = 0;

	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo, unsigned int stride);
	void static ReadAiMaterial(const aiMaterial* aiMaterial, Material& material);
};

class AssimpMesh {

	friend AssimpModel;

public:

	AssimpMesh(AssimpModel* model);
	~AssimpMesh();

	void drawRaw();
	void drawRawInstanced();

private:

	AssimpModel::Material m_material;
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
	unsigned int m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;

	void updateMaterialUbo(unsigned int& ubo);
};