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


#include "engine\Vector.h"
#include "engine\Extension.h"
#include "engine\Shader.h"
#include "Texture.h"
#include "Camera.h"

#include "Constants.h"

struct IndexBufferCreator {

	std::vector<std::array<int, 10>> face;
	std::vector<float> positionCoordsIn;
	std::vector<float> normalCoordsIn;
	std::vector<float> textureCoordsIn;

	std::vector <float> vertexBufferOut;
	std::vector<int> indexBufferOut;

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

	void draw(const Camera camera);
	void draw(const Camera camera, const Matrix4f &model);

	//size values
	unsigned int m_size, m_numVertices, m_numIndices, m_stride, m_offset, m_numberOfBytes;


	bool loadObject(const char* filename);
	bool loadObject(const char* a_filename, Vector3f &rotate, float degree, Vector3f& translate, float scale);

	int getNumberOfIndices() const;

	std::string getMltPath();
	std::string getModelDirectory();

	void setShader(const char* vertex, const char* fragment);
	void Model::setModelMatrix(Matrix4f &modelMatrix) { m_modelMatrix = modelMatrix;}

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

	std::vector <float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;
	std::map<int, std::vector<int>> m_vertexCache;
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

	void draw(const Camera camera);
	void draw(const Camera camera, const Matrix4f &model);

	void setMaterial(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular, float shinies);
	Material getMaterial();


	bool readMaterial();

	Model* m_model;
	std::string m_mltName;
	Material m_material;

	unsigned int m_triangleOffset, m_numberOfTriangles, m_stride;
	///////////////////////////////////////OpenGL content////////////////// to do seperate it 

	void createBuffer();
	void createShader();

	short m_numBuffers;
	unsigned int m_vao;
	unsigned int m_vbo[4];
	unsigned int m_drawCount;

	std::shared_ptr<Shader> m_shader;
	std::shared_ptr<Texture> m_texture;

	std::vector <float> m_vertexBuffer;

	std::vector<int> m_indexBuffer;
	std::vector<Vector3f> m_positions;
	std::vector<Vector2f> m_texels;
	std::vector<Vector3f> m_normals;
};