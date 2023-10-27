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

#include "Vector.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"
#include "AssetManger.h"
#include "BuiltInShader.h"
#include "Material.h"
#include "../Miniball/Miniball.h"


class ObjModel;
struct BoundingBoxS {

	void createBuffer();
	void drawRaw();

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;

	Vector3f position;
	Vector3f size;
};

struct BoundingSphere {

	void createBuffer(ObjModel& model);	
	void drawRaw();

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
	void createBuffer(const char* filename, Vector3f &rotate, float degree, Vector3f& translate, float scale, bool useConvhull, ObjModel& model);
	void drawRaw();

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
	friend BoundingBoxS;
	friend BoundingSphere;
	friend ConvexHull;

public:

	ObjModel();
	~ObjModel();

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void rotate(const Vector3f& axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	void drawRaw() const;
	void drawRawInstanced();
	void drawRawStacked();
	void drawRawInstancedStacked();

	void draw(const Camera& camera);
	void drawInstanced(const Camera& camera);
	void drawStacked(const Camera& camera);
	void drawInstancedStacked(const Camera& camera);

	void createAABB();
	void createSphere();
	void createConvexHull(const char* a_filename, bool useConvhull = true);
	void createConvexHull(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate, float scale, bool useConvhull = true);

	void drawAABB();
	void drawSphere();
	void drawHull();

	bool loadModel(const char* filename, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	bool loadModel(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool asStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);

	std::string getMltPath();
	std::string getModelDirectory();
	BoundingBoxS& getAABB();
	BoundingSphere& getBoundingSphere();
	ConvexHull& getConvexHull();
	Transform& getTransform();
	std::vector<ObjMesh*>& getMeshes();
	unsigned int getNumberOfTriangles();

	void generateTangents();
	void generateNormals();
	
	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void addInstance(const Matrix4f& modelMTX);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	void initAssets(bool instanced = false);
	void initAssets(AssetManager<Shader>& shaderManager, bool instanced = false);

	const unsigned int& getVbo() const;
	const unsigned int& getIbo() const;

private:

	int whitespaces(char c[]);

	unsigned int m_numberOfVertices, m_numberOfTriangles, m_numberOfMeshes, m_stride;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents, m_hasMaterial;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;
	bool m_isStacked;

	std::vector<ObjMesh*> m_meshes;
	std::string m_mltPath;
	std::string m_modelDirectory;

	Vector3f m_center;
	
	BoundingBoxS aabb;
	BoundingSphere boundingSphere;
	ConvexHull convexHull;

	std::unordered_map<int, Shader*> m_shader;
	AssetManager<Shader> m_shaderManager;

	Transform m_transform;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<Matrix4f> m_instances;

	unsigned int m_instanceCount = 0;
	unsigned int m_drawCount = 0;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
	unsigned int m_vboInstances = 0;

	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& vao, unsigned int& vbo, unsigned int& ibo, unsigned int stride);
	void static GenerateNormals(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static GenerateTangents(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, bool& hasTangents, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);

	void static GenerateNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateFlatNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateTangents(std::vector<float>& vertexCoords, std::vector<float>& textureCoords, std::vector<float>& normalCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& tangentCoords, std::vector<float>& bitangentCoords);

	void static ReadMaterialFromFile(std::string path, std::string mltName, short& index);
	std::string static GetTexturePath(std::string texPath, std::string modelDirectory);
};

class ObjMesh {

	friend ObjModel;

public:

	ObjMesh(std::string mltName, int numberTriangles, ObjModel* model);
	ObjMesh(int numberTriangles, ObjModel* model);
	~ObjMesh();

	void drawRaw() const;
	void drawRawInstanced();
	const Material& getMaterial() const;
	
	std::vector<float>& getVertexBuffer();
	std::vector<unsigned int>& getIndexBuffer();
	int getStride();
	void cleanup();
	unsigned int getNumberOfTriangles();
	const unsigned int& getVbo() const;
	const unsigned int& getIbo() const;
	void packBuffer();

private:

	ObjModel* m_model;
	std::string m_mltName;

	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void addInstance(ObjModel& model);
	void updateInstances(std::vector<Matrix4f>& modelMTX);

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboInstances = 0;
	unsigned int m_ibo = 0;
	
	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;
	
	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;	
	short m_materialIndex = -1;
};