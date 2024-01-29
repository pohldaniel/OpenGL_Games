#pragma once

#include <functional>
#include <numeric>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Vector.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"
#include "AssetManger.h"
#include "BuiltInShader.h"
#include "Material.h"
#include "ObjModel.h"

//#define ASSIMP_LOAD_FLAGS (aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices | aiProcess_Triangulate)
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FindDegenerates)

class AssimpMesh;
class AssimpModel {

	friend AssimpMesh;

public:

	AssimpModel();
	~AssimpModel();

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void setPosition(float x, float y, float z);

	void rotate(const Vector3f& axis, float degrees);
	void rotate(float pitch, float yaw, float roll);
	void translate(float dx, float dy, float dz);
	void scale(float sx, float sy, float sz);

	void drawRaw();
	void drawRawInstanced();
	void drawRawStacked();
	void drawRawInstancedStacked();

	void draw(const Camera& camera);
	void drawInstanced(const Camera& camera);
	void drawStacked(const Camera& camera);
	void drawInstancedStacked(const Camera& camera);

	void createAABB();
	void drawAABB();

	bool loadModel(const char* filename, bool isStacked = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);

	std::string getModelDirectory();
	BoundingBox& getAABB();
	Transform& getTransform();
	const AssimpMesh* getMesh(unsigned short index = 0) const;
	std::vector<AssimpMesh*>& getMeshes();
	std::vector<float>& getVertexBuffer();
	std::vector<unsigned int>& getIndexBuffer();

	void addInstances(const std::vector<Matrix4f>& modelMTX);
	void addInstance(const Matrix4f& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	void initShader(bool instanced = false);
	void initShader(AssetManager<Shader>& shaderManager, bool instanced = false);
	void cleanup();

	static void Cleanup();

private:

	unsigned int m_numberOfVertices, m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;
	bool m_isStacked;

	std::vector<AssimpMesh*> m_meshes;
	std::string m_modelDirectory;

	Vector3f m_center;
	BoundingBox m_aabb;

	std::vector<Shader*> m_shader;


	Transform m_transform;

	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<Matrix4f> m_instances;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
	unsigned int m_vboInstances = 0;

	void unuseAllShader();
	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int vbo, unsigned int& ibo, unsigned int stride);
	void static ReadAiMaterial(const aiMaterial* aiMaterial, short& index, std::string modelDirectory);
	std::string static GetTexturePath(std::string texPath, std::string modelDirectory);

	static AssetManager<Shader> ShaderManager;
};

class AssimpMesh {

	friend AssimpModel;

public:

	AssimpMesh(AssimpModel* model);
	~AssimpMesh();

	void drawRaw();
	void drawRawInstanced();

	std::vector<float>& getVertexBuffer();
	std::vector<unsigned int>& getIndexBuffer();
	unsigned int getStride();
	short getMaterialIndex();
	void setMaterialIndex(short index) const;
	short getTextureIndex();
	void setTextureIndex(short index) const;

	const Material& getMaterial() const;
	void cleanup();

private:

	void addInstance(const AssimpModel& model);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	AssimpModel* m_model;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboInstances = 0;
	unsigned int m_ibo = 0;

	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;
	
	mutable short m_materialIndex = -1;
	mutable short m_textureIndex = -1;
};