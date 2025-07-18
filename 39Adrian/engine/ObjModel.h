#pragma once
#include <GL/glew.h>
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

#include "Vector.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"
#include "AssetManger.h"
#include "BuiltInShader.h"
#include "Material.h"
#include "BoundingBox.h"
#include "../Miniball/Miniball.h"

class ObjModel;

struct BoundingSphere {

	void createBuffer(ObjModel& model);	
	void drawRaw() const;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;


	int m_uResolution = 49;
	int m_vResolution = 49;

	float m_radius;
	Vector3f m_position;
};

struct ConvexHull {
	void createBuffer(const char* filename, const Vector3f &rotate, float degree, const Vector3f& translate, float scale, bool useConvhull, ObjModel& model);
	void drawRaw() const;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
};

struct IndexBufferCreator {

	std::vector<std::array<int, 10>> face;
	std::vector<float> positionCoordsIn;
	std::vector<float> normalCoordsIn;
	std::vector<float> textureCoordsIn;
	std::vector<float> tangentCoordsIn;
	std::vector<float> bitangentCoordsIn;

	std::vector <float> vertexBufferOut;
	std::vector<unsigned int> indexBufferOut;

	void createIndexBuffer();

private:
	std::map<int, std::vector<int>> m_vertexCache;
	int addVertex(int hash, const float *pVertex, int stride);
};

class ObjMesh;
class ObjModel {

	friend ObjMesh;
	friend BoundingBox;
	friend BoundingSphere;
	friend ConvexHull;
	friend class MeshSequence;

public:

	ObjModel();
	ObjModel(ObjModel const& rhs);
	ObjModel(ObjModel&& rhs);
	ObjModel& operator=(const ObjModel& rhs);
	ObjModel& operator=(ObjModel&& rhs);
	~ObjModel();

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
	void drawRawSequence(unsigned short frame) const;

	void draw(const Camera& camera) const;
	void draw(const Camera& camera, unsigned short meshIndex) const;
	void drawInstanced(const Camera& camera) const;
	void drawStacked(const Camera& camera) const;
	void drawInstancedStacked(const Camera& camera) const;

	void createAABB();
	void createSphere();
	void createConvexHull(const char* a_filename, bool useConvhull = true);
	void createConvexHull(const char* a_filename, const Vector3f& rotate, float degree, const Vector3f& translate, float scale, bool useConvhull = true);

	void drawAABB() const;
	void drawSphere() const;
	void drawHull() const;

	void loadModel(const char* filename, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void loadModel(const char* filename, const Vector3f& axis, float degree, const Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void loadModelCpu(const char* filename, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void loadModelCpu(const char* filename, const Vector3f& axis, float degree, const Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool asStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void loadModelGpu(bool forceClearCpuBuffer = true);

	const std::string& getMltPath();
	const std::string& getModelDirectory();
	const BoundingBox& getAABB() const;
	const BoundingSphere& getBoundingSphere() const;
	const ConvexHull& getConvexHull() const;
	const Transform& getTransform() const;
	const ObjMesh* getMesh(unsigned short index = 0u) const;
	const std::vector<ObjMesh*>& getMeshes() const;
	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	unsigned int getNumberOfTriangles();

	void generateTangents();
	void generateNormals();
	void packBuffer();
	
	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void addInstance(const Matrix4f& modelMTX);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	void initShader(bool instanced = false);
	void initShader(AssetManager<Shader>& shaderManager, bool instanced = false);

	const unsigned int& getVbo() const;
	const unsigned int& getIbo() const;
	void cleanup();
	void markForDelete();

	static void CleanupShader();

private:

	unsigned int m_numberOfVertices, m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;
	bool m_isStacked;

	std::vector<ObjMesh*> m_meshes;
	std::string m_mltPath;
	std::string m_modelDirectory;

	Vector3f m_center;
	
	BoundingBox m_aabb;
	BoundingSphere m_boundingSphere;
	ConvexHull m_convexHull;

	std::vector<Shader*> m_shader;

	Transform m_transform;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<Matrix4f> m_instances;

	unsigned int m_instanceCount;
	unsigned int m_drawCount;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
	unsigned int m_vboInstances;
	bool m_markForDelete;

	void unuseAllShader() const;

	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int& vbo, unsigned int& ibo, unsigned int stride);
	void static GenerateNormals(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static GenerateTangents(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, bool& hasTangents, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static PackBuffer(std::vector<float>& vertexBuffer, unsigned int& vao, unsigned int& vbo, unsigned int stride);

	void static GenerateNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateFlatNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateTangents(std::vector<float>& vertexCoords, std::vector<float>& textureCoords, std::vector<float>& normalCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& tangentCoords, std::vector<float>& bitangentCoords);


	void static ReadMaterialFromFile(std::string path, std::string mltLib, std::string mltName, short& index);
	std::string static GetTexturePath(std::string texPath, std::string modelDirectory);

	static AssetManager<Shader> ShaderManager;
};

class ObjMesh {

	friend ObjModel;

public:

	ObjMesh(std::string mltName, unsigned int numberTriangles, ObjModel* model);
	ObjMesh(unsigned int numberTriangles, ObjModel* model);
	ObjMesh(ObjMesh const& rhs);
	ObjMesh(ObjMesh&& rhs);
	ObjMesh& operator=(const ObjMesh& rhs);
	ObjMesh& operator=(ObjMesh&& rhs);
	~ObjMesh();

	void drawRaw() const;
	void drawRawInstanced() const;
	
	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	int getStride();
	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;

	const Material& getMaterial() const;
	void cleanup();
	void markForDelete();

	unsigned int getNumberOfTriangles();
	const unsigned int& getVbo() const;
	const unsigned int& getIbo() const;

private:

	ObjModel* m_model;
	std::string m_mltName;

	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void addInstance(ObjModel& model);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_vboInstances;
	unsigned int m_ibo;
	bool m_markForDelete;

	unsigned int m_drawCount;
	unsigned int m_instanceCount;
	
	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;	
	mutable short m_materialIndex;
	mutable short m_textureIndex;
};