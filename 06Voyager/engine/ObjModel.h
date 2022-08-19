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

#include "Vector.h"
#include "Extension.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"
#include "AssetManger.h"
#include "..\BuiltInShader.h"
#include "..\Miniball\Miniball.h"


class ObjModel;
struct BoundingBox {

	void createBuffer(ObjModel& model);
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


class Mesh;
class ObjModel {
	friend Mesh;
	friend BoundingBox;
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

	void drawRaw();
	void drawRawInstanced();
	void drawRawStacked();
	void drawRawInstancedStacked();

	void draw(Camera& camera);
	void drawInstanced(Camera& camera);
	void drawStacked(Camera& camera);
	void drawInstancedStacked(Camera& camera);

	void createAABB();
	void createSphere();
	void createConvexHull(const char* a_filename, bool useConvhull = true);
	void createConvexHull(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate, float scale, bool useConvhull = true);

	void drawAABB();
	void drawSphere();
	void drawHull();

	bool loadObject(const char* filename, bool asStackedModel = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false);
	bool loadObject(const char* a_filename, Vector3f& rotate, float degree, Vector3f& translate, float scale, bool asStackedModel = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false);

	std::string getMltPath();
	std::string getModelDirectory();
	BoundingBox& getAABB();
	BoundingSphere& getBoundingSphere();
	ConvexHull& getConvexHull();
	Transform& getTransform();
	std::vector<Mesh*> getMeshes();

	void generateTangents();
	void generateNormals();
	
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

	std::vector<Mesh*> m_meshes;
	std::string m_mltPath;
	std::string m_modelDirectory;

	Vector3f m_center;
	
	BoundingBox aabb;
	BoundingSphere boundingSphere;
	ConvexHull convexHull;

	std::unordered_map<int, Shader*> m_shader;
	std::unordered_map<int, Texture> m_textures;

	AssetManager<Shader> m_shaderManager;
	AssetManager<Texture> m_textureManager;

	Transform m_transform;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;

	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_ibo = 0;
	unsigned int m_vboInstances = 0;

	void static CreateBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int& drawCount, unsigned int& vao, unsigned int(&vbo)[5], unsigned int& ibo, unsigned int stride);
	void static GenerateNormals(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);
	void static GenerateTangents(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, ObjModel& model, bool& hasNormals, bool& hasTangents, unsigned int& stride, unsigned int startIndex, unsigned int endIndex);

	void static GenerateNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateFlatNormals(std::vector<float>& vertexCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& normalCoords);
	void static GenerateTangents(std::vector<float>& vertexCoords, std::vector<float>& textureCoords, std::vector<float>& normalCoords, std::vector<std::array<int, 10>>& face, std::vector<float>& tangentCoords, std::vector<float>& bitangentCoords);
};

class Mesh {

	friend ObjModel;

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

	Mesh(std::string mltName, int numberTriangles, ObjModel* model);
	Mesh(int numberTriangles, ObjModel* model);
	~Mesh();

	void drawRaw();
	void drawRawInstanced();
	void setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies);
	Material& getMaterial();
	
	std::vector<float>& getVertexBuffer();
	std::vector<unsigned int>& getIndexBuffer();
	int getStride();

private:
	bool readMaterial();

	ObjModel* m_model;
	std::string m_mltName;
	Material m_material;	
	///////////////////////////////////////OpenGL content//////////////////

	void createInstancesStatic(std::vector<Matrix4f>& modelMTX);
	void createInstancesDynamic(unsigned int numberOfInstances);
	void updateInstances(std::vector<Matrix4f>& modelMTX);
	
	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_vboInstances = 0;
	unsigned int m_ibo = 0;
	
	unsigned int m_drawCount = 0;
	unsigned int m_instanceCount = 0;
	
	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride, m_baseVertex, m_baseIndex;

	void updateMaterialUbo(unsigned int& ubo);
};