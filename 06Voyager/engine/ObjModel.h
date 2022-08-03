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
#include "ModelMatrix.h"

#include "..\Miniball\Miniball.h"
#include "..\Constants.h"


struct BoundingBox {
	void createBuffer(float minX, float maxX, float minY, float maxY, float minZ, float maxZ, unsigned int id);
	void drawRaw();

	std::vector <float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_ibo = 0;
};

struct BoundingSphere {
	void createBuffer();
	void drawRaw();

	std::vector <float> m_vertexBuffer;
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
	void createBuffer();
	void drawRaw();

	std::vector <float> m_vertexBuffer;
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
public:

	Model();
	~Model();

	const Vector3f &getCenter() const;
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	void setRotPos(const Vector3f &axis, float degrees, float dx, float dy, float dz);
	void setRotXYZPos(const Vector3f &axisX, float degreesX,
		const Vector3f &axisY, float degreesY,
		const Vector3f &axisZ, float degreesZ,
		float dx, float dy, float dz);

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);

	void draw(const Camera& camera);
	void drawRaw();
	void drawAABB();
	void drawSphere();
	void drawHull();
	void drawInstanced(const Camera& camera);

	//size values
	unsigned int m_size, m_numVertices, m_numIndices, m_stride, m_offset, m_numberOfBytes;


	bool loadObject(const char* filename);
	bool loadObject(const char* a_filename, Vector3f &rotate, float degree, Vector3f& translate, float scale);

	int getNumberOfIndices() const;

	std::string getMltPath();
	std::string getModelDirectory();


	void setModelMatrix(Matrix4f &modelMatrix) { m_modelMatrix = modelMatrix;}
	void createInstances(std::vector<Matrix4f> modelMTX);

	bool m_hasTextureCoords;
	bool m_hasNormals;
	bool m_hasTangents;

	std::vector<Mesh*> m_mesh;


	int addVertex(int hash, float *pVertex, int stride);
	int m_numberOfMeshes;
	int m_numberOfTriangles;
	std::string m_mltPath;
	std::string m_modelDirectory;
	bool m_hasMaterial;
	Vector3f m_center;
	Matrix4f m_modelMatrix;
	ModelMatrix modelMatrix;

	std::vector <float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::map<int, std::vector<int>> m_vertexCache;


	BoundingBox aabb;
	BoundingSphere boundingSphere;
	ConvexHull convexHull;

	unsigned int m_id;

	static unsigned int s_id;
};

class Mesh {

	friend Model;

public:
	///////////////////////////////geometry content
	struct Material {
		enum MaterialID {
			NONE = 0,
			GLOSSY = 1,
			DIFFUSE = 2,
			NONE_TEXTURE = 4,
			GLOSSY_TEXTURE = 8,
			DIFFUSE_TEXTURE = 16,
		};



		float ambient[4];
		float diffuse[4];
		float specular[4];
		float shininess;
		std::string diffuseTexPath;
		std::string bumpMapPath;
		std::string displacementMapPath;
		Shader * shader;
		MaterialID materialID = MaterialID::NONE;
	};

	Mesh(std::string mltName, int numberTriangles, Model* model);
	Mesh(int numberTriangles, Model* model);
	~Mesh();

	void draw(const Camera& camera);
	void drawRaw();
	void drawInstanced(const Camera& camera);

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
	
	void setShader(Shader* shader);
	void setTexture(Texture* texture);

	short m_numBuffers;
	unsigned int m_vao = 0;
	unsigned int m_vbo[5] = { 0 };
	unsigned int m_ibo = 0;
	unsigned int m_vbo2;

	unsigned int m_drawCount;
	unsigned int m_instanceCount = 0;
	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;

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