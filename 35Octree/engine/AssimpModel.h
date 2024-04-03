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
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FindDegenerates | aiProcess_GenUVCoords)

class AssimpMesh;
class AssimpModel {

	friend AssimpMesh;

public:

	AssimpModel();
	AssimpModel(AssimpModel const& rhs);
	AssimpModel(AssimpModel&& rhs);
	AssimpModel& operator=(const AssimpModel& rhs);
	AssimpModel& operator=(AssimpModel&& rhs);
	~AssimpModel();

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void setPosition(float x, float y, float z);

	void rotate(const Vector3f& axis, float degrees);
	void rotate(float pitch, float yaw, float roll);
	void translate(float dx, float dy, float dz);
	void scale(float sx, float sy, float sz);

	void drawRaw() const;
	void drawRawInstanced() const;
	void drawRawStacked() const;
	void drawRawInstancedStacked() const;

	void draw(const Camera& camera) const;
	void draw(const Camera& camera, unsigned short meshIndex) const;
	void drawInstanced(const Camera& camera) const;
	void drawStacked(const Camera& camera) const;
	void drawInstancedStacked(const Camera& camera) const;

	void createAABB();
	void drawAABB();
	void loadModel(const char* filename, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModelCpu(const char* filename, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModelGpu();

	std::string getModelDirectory();
	const BoundingBox& getAABB() const;
	Transform& getTransform();
	const AssimpMesh* getMesh(unsigned short index = 0u) const;
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
	void markForDelete();

	static void CleanupShader();

private:

	unsigned int m_numberOfVertices, m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;
	bool m_isStacked;

	std::vector<AssimpMesh*> m_meshes;
	std::vector<Shader*> m_shader;

	std::string m_modelDirectory;
	Vector3f m_center;
	BoundingBox m_aabb;
	Transform m_transform;

	unsigned int m_drawCount;
	unsigned int m_instanceCount;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<Matrix4f> m_instances;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_vboInstances;
	bool m_markForDelete;

	void unuseAllShader() const;
	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int vbo, unsigned int& ibo, unsigned int stride);
	void static ReadAiMaterial(const aiMaterial* aiMaterial, short& index, std::string modelDirectory, std::string mltName);
	std::string static GetTexturePath(std::string texPath, std::string modelDirectory);

	static AssetManager<Shader> ShaderManager;
};

class AssimpMesh {

	friend AssimpModel;

public:

	AssimpMesh(AssimpModel* model);
	AssimpMesh(AssimpMesh const& rhs);
	AssimpMesh(AssimpMesh&& rhs);
	AssimpMesh& operator=(const AssimpMesh& rhs);
	AssimpMesh& operator=(AssimpMesh&& rhs);
	~AssimpMesh();

	void drawRaw() const;
	void drawRawInstanced() const;

	std::vector<float>& getVertexBuffer();
	std::vector<unsigned int>& getIndexBuffer();
	unsigned int getStride();
	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;

	const Material& getMaterial() const;
	void cleanup();
	void markForDelete();

private:

	void addInstance(const AssimpModel& model);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	AssimpModel* m_model;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_vboInstances;
	unsigned int m_ibo;

	unsigned int m_drawCount;
	unsigned int m_instanceCount;
	bool m_markForDelete;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;
	
	mutable short m_materialIndex;
	mutable short m_textureIndex;
};