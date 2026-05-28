#pragma once

#include <functional>
#include <numeric>
#include <unordered_map>

#include <assimp/Importer.hpp> 
#include <assimp/Exporter.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Model.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"

//#define ASSIMP_LOAD_FLAGS (aiProcess_JoinIdenticalVertices | aiProcess_RemoveRedundantMaterials | aiProcess_PreTransformVertices | aiProcess_Triangulate)
#define ASSIMP_LOAD_FLAGS (aiProcess_Triangulate | aiProcess_FindDegenerates | aiProcess_GenUVCoords)

class AssimpMesh;
class AssimpModel : public Model {

	friend class AssimpMesh;

public:

	AssimpModel();
	AssimpModel(AssimpModel const& rhs);
	AssimpModel(AssimpModel&& rhs) noexcept;
	AssimpModel& operator=(const AssimpModel& rhs);
	AssimpModel& operator=(AssimpModel&& rhs) noexcept;
	~AssimpModel();

	void loadModel(const char* filename, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModel(const char* filename, const Vector3f& axis, float degrees, const Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModelCpu(const char* filename, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
	void loadModelCpu(const char* filename, const Vector3f& axis, float degrees, const Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool isStacked = false, bool generateNormals = false, bool generateTangents = false, bool flipYZ = false, bool flipWinding = false);
		
	const Vector3f& getCenter() const;
	const unsigned int getStride() const override;
	const std::string& getModelDirectory();
	const Mesh* getMesh(unsigned short index = 0u) const;
	const std::vector<Mesh*>& getMeshes() const;
	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	const unsigned int getNumberOfTriangles() const;

	void generateNormals();
	void rewind();

	void generateColors(ModelColor modelColor = MC_WHITE);
	void generateUVs(ProjectedPlane projectedPlane = XY);
	void packBuffer();
	void cleanup();

private:

	unsigned int m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_isStacked;

	std::string m_modelDirectory;
	Vector3f m_center;
	unsigned int m_drawCount;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	void static ReadAiMaterial(const aiMaterial* aiMaterial, short& index, std::string modelDirectory, std::string mltName);
	std::string static GetTexturePath(std::string texPath, std::string modelDirectory);
};

class AssimpMesh : public Mesh {

	friend class AssimpModel;

public:

	AssimpMesh(AssimpModel* model);
	AssimpMesh(AssimpMesh const& rhs);
	AssimpMesh(AssimpMesh&& rhs) noexcept;
	AssimpMesh& operator=(const AssimpMesh& rhs);
	AssimpMesh& operator=(AssimpMesh&& rhs) noexcept;
	~AssimpMesh();

	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;

	const Material& getMaterial() const;
	void cleanup();

	const std::unordered_map<TextureSlot, std::pair<unsigned char*, unsigned int>>& getEmbeddedTextures() const;
	const void removeEmbeddedTexture(TextureSlot textureSlot) const;
	const bool hasMaterial() const;

private:

	AssimpModel* m_model;
	mutable short m_textureIndex;
	mutable short m_materialIndex;

	mutable std::unordered_map<TextureSlot, std::pair<unsigned char*, unsigned int>> m_embeddedTextures;
};