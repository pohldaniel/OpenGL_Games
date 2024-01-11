#pragma once

#include <engine/Vector.h>
#include <engine/Transform.h>
#include <vector>
#include <map>
#include <iostream>
#include <filesystem>
#include <numeric>
#include <array>

class ObjSequence {

	struct Frame{		
		unsigned int numberOfTriangles;
		unsigned int baseVertex;
		unsigned int baseIndex;
		unsigned int drawCount;
	};

public:

	ObjSequence();
	~ObjSequence();

	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix();

	void setPosition(float x, float y, float z);

	void rotate(const Vector3f& axis, float degrees);
	void rotate(float pitch, float yaw, float roll);
	void translate(float dx, float dy, float dz);
	void scale(float sx, float sy, float sz);

	void drawRawSequence(unsigned short frame);

	void loadSequence(const char* path, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void loadSequence(const char* path, Vector3f& axis, float degree, Vector3f& translate = Vector3f(0.0f, 0.0f, 0.0f), float scale = 1.0f, bool isStacked = false, bool withoutNormals = false, bool generateSmoothNormals = false, bool generateFlatNormals = false, bool generateSmoothTangents = false, bool rescale = false);
	void cleanup();

private:

	unsigned int m_numberOfMeshes, m_stride;
	std::vector<Frame> m_meshes;
	Transform m_transform;

	std::vector<float> m_vertexBuffer;
	std::vector<unsigned int> m_indexBuffer;

	unsigned int m_vao;
	unsigned int m_vbo;
	unsigned int m_ibo;
};