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

#include "Vector.h"
#include "Extension.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "Transform.h"

#include "..\Miniball\Miniball.h"
#include "..\Constants.h"

struct BoundingBox {

	void createBuffer(Model& model);
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

	void createBuffer(Model& model);
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
	void createBuffer(const char* filename, Vector3f &rotate, float degree, Vector3f& translate, float scale, bool useConvhull, Model& model);
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

	std::vector <float> vertexBufferOut;
	std::vector<unsigned int> indexBufferOut;

	void createIndexBuffer();

private:
	std::map<int, std::vector<int>> m_vertexCache;
	int addVertex(int hash, const float *pVertex, int stride);
};


class Mesh;
class Model {
	friend Mesh;
	friend BoundingBox;
	friend BoundingSphere;
	friend ConvexHull;

public:

	Model();
	~Model();

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	void drawRaw();

	void createAABB();
	void createSphere();
	void createConvexHull(const char* a_filename, bool useConvhull = true);
	void createConvexHull(const char* a_filename, Vector3f &rotate, float degree, Vector3f& translate, float scale, bool useConvhull = true);

	void drawAABB();
	void drawSphere();
	void drawHull();

	bool loadObject(const char* filename);
	bool loadObject(const char* a_filename, Vector3f &rotate, float degree, Vector3f& translate, float scale);


	std::string getMltPath();
	std::string getModelDirectory();
	BoundingBox& getAABB();
	BoundingSphere& getBoundingSphere();
	ConvexHull& getConvexHull();

	void setModelMatrix(Matrix4f &modelMatrix) { m_modelMatrix = modelMatrix; }
	void createInstances(std::vector<Matrix4f> modelMTX);

	std::vector<Mesh*> getMeshes() {
		return m_mesh;
	}
	
	Transform m_transform;

private:
	//size values
	unsigned int m_size, m_numVertices, m_numIndices, m_stride, m_offset, m_numberOfBytes;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	bool m_hasAABB, m_hasBoundingSphere, m_hasConvexHull;

	std::vector<Mesh*> m_mesh;
	
	int m_numberOfMeshes;
	int m_numberOfTriangles;
	unsigned int m_numberOfVertices;
	std::string m_mltPath;
	std::string m_modelDirectory;
	bool m_hasMaterial;
	Vector3f m_center;
	Matrix4f m_modelMatrix;
	

	BoundingBox aabb;
	BoundingSphere boundingSphere;
	ConvexHull convexHull;
};

class Mesh {

	friend Model;

public:
	///////////////////////////////geometry content
	struct Material {		
		float ambient[4];
		float diffuse[4];
		float specular[4];
		float shininess;
		std::string diffuseTexPath;
		std::string bumpMapPath;
		std::string displacementMapPath;
	};

	Mesh(std::string mltName, int numberTriangles, Model* model);
	Mesh(int numberTriangles, Model* model);
	~Mesh();

	void drawRaw();

	void setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies);
	Material getMaterial();
	void generateTangents();
	void generateNormals();
	bool readMaterial();

	Model* m_model;
	std::string m_mltName;
	Material m_material;	
	///////////////////////////////////////OpenGL content////////////////// to do seperate it 

	void createBuffer();
	void createInstances(std::vector<Matrix4f> modelMTX);
	
	short m_numBuffers;
	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_ibo = 0;
	unsigned int m_vbo2;

	unsigned int m_drawCount;
	unsigned int m_instanceCount = 0;
	
	std::vector <float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
	std::vector<Vector3f> m_tangents;
	std::vector<Vector3f> m_bitangents;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride;
};