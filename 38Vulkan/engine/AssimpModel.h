#pragma once

#include <functional>
#include <numeric>

#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Camera.h"
#include "Material.h"

//#define ASSIMP_LOAD_FLAGS (aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices | aiProcess_Triangulate)
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FindDegenerates | aiProcess_GenUVCoords)

class AssimpMesh;
class AssimpModel {

	friend AssimpMesh;

public:

	AssimpModel();
	AssimpModel(AssimpModel const& rhs);
	AssimpModel(AssimpModel&& rhs) noexcept;
	AssimpModel& operator=(const AssimpModel& rhs);
	AssimpModel& operator=(AssimpModel&& rhs) noexcept;
	~AssimpModel();

	const glm::vec3&getCenter() const;

	void loadModel(const char* filename, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModel(const char* filename, const glm::vec3& axis, float degree, const glm::vec3& translate = glm::vec3(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModelCpu(const char* filename, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModelCpu(const char* filename, const glm::vec3& axis, float degree, const glm::vec3& translate = glm::vec3(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	
	const std::string& getModelDirectory();
	const AssimpMesh* getMesh(unsigned short index = 0u) const;
	const std::vector<AssimpMesh*>& getMeshes() const;
	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;

	void cleanup();

private:

	unsigned int m_numberOfVertices, m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;
	bool m_isStacked;

	std::vector<AssimpMesh*> m_meshes;

	std::string m_modelDirectory;
	glm::vec3 m_center;

	unsigned int m_drawCount;


	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	void static ReadAiMaterial(const aiMaterial* aiMaterial, short& index, std::string modelDirectory, std::string mltName);
	std::string static GetTexturePath(std::string texPath, std::string modelDirectory);


};

class AssimpMesh {

	friend AssimpModel;

public:

	AssimpMesh(AssimpModel* model);
	AssimpMesh(AssimpMesh const& rhs);
	AssimpMesh(AssimpMesh&& rhs) noexcept;
	AssimpMesh& operator=(const AssimpMesh& rhs);
	AssimpMesh& operator=(AssimpMesh&& rhs) noexcept;
	~AssimpMesh();

	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	unsigned int getStride();
	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;

	const Material& getMaterial() const;
	void cleanup();

private:

	AssimpModel* m_model;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_vboInstances;
	unsigned int m_ibo;

	unsigned int m_drawCount;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;
	
	mutable short m_materialIndex;
	mutable short m_textureIndex;
};