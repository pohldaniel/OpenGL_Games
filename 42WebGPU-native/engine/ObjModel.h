#pragma once
#include <iostream>
#include <ctime>
#include <vector>
#include <array>
#include <fstream>
#include <map>
#include <string>
#include <algorithm>
#include <memory>
#include <cctype>
#include <unordered_map>
#include <functional>
#include <filesystem>
#include <numeric>

#include "Mesh.h"
#include "Vector.h"
#include "Camera.h"
#include "Transform.h"
#include "Material.h"

class ObjModel;

struct IndexBufferCreator {

	std::vector<std::array<int, 10>> face;
	std::vector<float> positionCoordsIn;
	std::vector<float> normalCoordsIn;
	std::vector<float> textureCoordsIn;
	std::vector<float> tangentCoordsIn;
	std::vector<float> bitangentCoordsIn;

	std::vector <float> vertexBufferOut;
	std::vector<unsigned int> indexBufferOut;

	void createIndexBuffer(bool flipWinding = false);

private:
	std::map<int, std::vector<int>> m_vertexCache;
	int addVertex(int hash, const float *pVertex, int stride);
};

class ObjMesh;
class ObjModel {

	friend ObjMesh;

public:

	ObjModel();
	ObjModel(ObjModel const& rhs);
	ObjModel(ObjModel&& rhs) noexcept;
	ObjModel& operator=(const ObjModel& rhs);
	ObjModel& operator=(ObjModel&& rhs) noexcept;
	~ObjModel();

	void loadModel(const char* filename, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool flipYZ = false, bool flipWinding = false, bool rescale = false);
	void loadModel(const char* filename, const Vector3f& axis, float degree, const Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool flipYZ = false, bool flipWinding = false, bool rescale = false);
	void loadModelCpu(const char* filename, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool flipYZ = false, bool flipWinding = false, bool rescale = false);
	void loadModelCpu(const char* filename, const Vector3f& axis, float degree, const Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool asStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool flipYZ = false, bool flipWinding = false, bool rescale = false);

	void rotate(const Vector3f& axis, float degrees);
	void rotate(float pitch, float yaw, float roll);
	void translate(float dx, float dy, float dz);
	void scale(float sx, float sy, float sz);
	void setPosition(float x, float y, float z);

	const Vector3f& getCenter() const;
	const Matrix4f& getTransformationMatrix() const;
	const Matrix4f& getInvTransformationMatrix();
	const std::string& getMltPath();
	const std::string& getModelDirectory();
	const Transform& getTransform() const;
	const Mesh* getMesh(unsigned short index = 0u) const;
	const std::vector<ObjMesh*>& getMeshes() const;
	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	unsigned int getNumberOfTriangles();
	
	void generateTangents();
	void generateNormals();
	void packBuffer();
	void cleanup();
	void rewind();

private:

	unsigned int m_numberOfVertices, m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_isStacked;

	std::vector<ObjMesh*> m_meshes;
	std::string m_mltPath;
	std::string m_modelDirectory;

	Vector3f m_center;
	
	Transform m_transform;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_drawCount;

	void static GenerateNormals(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static GenerateTangents(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, bool& hasTangents, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static PackBuffer(std::vector<float>& vertexBuffer, unsigned int stride);
	void static Rewind(const std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride);

	void static GenerateNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateFlatNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateTangents(std::vector<float>& vertexCoords, std::vector<float>& textureCoords, std::vector<float>& normalCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& tangentCoords, std::vector<float>& bitangentCoords);


	void static ReadMaterialFromFile(std::string path, std::string mltLib, std::string mltName, short& index);
	std::string static GetTexturePath(std::string texPath, std::string modelDirectory);
};

class ObjMesh : public Mesh {

	friend ObjModel;

public:

	ObjMesh(std::string mltName, unsigned int numberTriangles, ObjModel* model);
	ObjMesh(unsigned int numberTriangles, ObjModel* model);
	ObjMesh(ObjMesh const& rhs);
	ObjMesh(ObjMesh&& rhs) noexcept;
	ObjMesh& operator=(const ObjMesh& rhs);
	ObjMesh& operator=(ObjMesh&& rhs) noexcept;
	~ObjMesh();

	const std::vector<float>& getVertexBuffer() const override;
	const std::vector<unsigned int>& getIndexBuffer() const override;
	unsigned int getStride() override;
	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;
	const Material& getMaterial() const;
	unsigned int getNumberOfTriangles() const;
	void cleanup();

private:

	ObjModel* m_model;
	std::string m_mltName;

	unsigned int m_drawCount;
	unsigned int m_instanceCount;
	
	std::vector<float> m_vertexBuffer;
	std::vector<float> m_positionBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;	
	mutable short m_materialIndex;
	mutable short m_textureIndex;
};