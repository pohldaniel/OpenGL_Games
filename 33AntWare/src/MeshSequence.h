#pragma once

#include <engine/ObjModel.h>
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>
#include <numeric>
#include <array>

class MeshSequence {

	struct Mesh{		
		unsigned int numberOfTriangles;
		unsigned int baseVertex;
		unsigned int baseIndex;
		unsigned int drawCount;
	};

public:

	MeshSequence();
	MeshSequence(MeshSequence const& rhs);
	MeshSequence(MeshSequence&& rhs);
	MeshSequence &operator=(const MeshSequence& rhs);
	MeshSequence& operator=(MeshSequence&& rhs);
	~MeshSequence();

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void setPosition(float x, float y, float z);

	void rotate(const Vector3f& axis, float degrees);
	void rotate(float pitch, float yaw, float roll);
	void translate(float dx, float dy, float dz);
	void scale(float sx, float sy, float sz);

	const void draw(unsigned short meshIndex, short textureIndex = -1, short materialIndex = -1) const;

	void loadSequence(const char* path, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void loadSequence(const char* path, Vector3f& axis, float degree, Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void addMeshFromFile(const char* path, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false, bool flipYZ = false, bool flipWinding = false);
	void addMeshFromFile(const char* path, Vector3f& axis, float degree, Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false, bool flipYZ = false, bool flipWinding = false);
	void addMesh(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);
	void addMeshAfter(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer);
	void loadSequenceGpu();

	void cleanup();
	void markForDelete();
	const unsigned int getNumberOfMeshes() const;
	const std::vector<Mesh>& getMeshes() const;
	const std::vector<BoundingBox>& getLocalBoundingBoxes() const;
	const BoundingBox& getLocalBoundingBox(unsigned int meshIndex) const;
	
private:

	std::vector<BoundingBox> m_localBoundingBoxes;
	std::map<unsigned int, size_t> m_boundingBoxCache;

	unsigned int m_numberOfMeshes, m_stride, m_numberOfVertices, m_numberOfIndices;
	std::vector<Mesh> m_meshes;
	
	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	Transform m_transform;
	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;	
	bool m_markForDelete = false;
};