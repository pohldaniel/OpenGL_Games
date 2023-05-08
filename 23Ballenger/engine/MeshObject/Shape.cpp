#include "Shape.h"
#include "Capsule.h"
#include "MeshTorus.h"
#include "MeshSphere.h"
#include "MeshSpiral.h"
#include "MeshCylinder.h"
#include "MeshQuad.h"
#include "MeshCube.h"
#include <iostream>

Shape::Shape() { }

void Shape::buildCapsule(float radius, float length, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Capsule::BuildMesh(radius, length, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildTorus(float radius, float tubeRadius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshTorus::BuildMesh(radius, tubeRadius, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildSphere(float radius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshSphere::BuildMesh(radius, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildSpiral(float radius, float tubeRadius, float length, int numRotations, bool repeatTexture, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshSpiral::BuildMesh(radius, tubeRadius, length, numRotations, repeatTexture, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildCylinder(float baseRadius, float topRadius, float length, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshCylinder::BuildMesh(baseRadius, topRadius, length, position, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildQuadXY(const Vector3f& position, const Vector2f&  size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshQuad::BuildMeshXY(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::buildCube(const Vector3f& position, const Vector3f&  size, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	MeshCube::BuildMesh4Q(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents);
	createBuffer();
}

void Shape::fromBuffer(std::vector<float>& vertexBuffer, std::vector<unsigned int> indexBuffer, unsigned int stride) {

	if (stride == 3) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
		}
	}

	if (stride == 5) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_texels.push_back(Vector2f(vertexBuffer[i + 3], vertexBuffer[i + 4]));
		}
	}

	if (stride == 6) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_normals.push_back(Vector3f(vertexBuffer[i + 3], vertexBuffer[i + 4], vertexBuffer[i + 5]));
		}
	}

	if (stride == 8) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_texels.push_back(Vector2f(vertexBuffer[i + 3], vertexBuffer[i + 4]));
			m_normals.push_back(Vector3f(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]));
			
		}
	}

	if (stride == 14) {
		for (unsigned int i = 0; i < vertexBuffer.size(); i = i + stride) {
			m_positions.push_back(Vector3f(vertexBuffer[i], vertexBuffer[i + 1], vertexBuffer[i + 2]));
			m_texels.push_back(Vector2f(vertexBuffer[i + 3], vertexBuffer[i + 4]));
			m_normals.push_back(Vector3f(vertexBuffer[i + 5], vertexBuffer[i + 6], vertexBuffer[i + 7]));
			m_tangents.push_back(Vector3f(vertexBuffer[i + 8], vertexBuffer[i + 9], vertexBuffer[i + 10]));
			m_bitangents.push_back(Vector3f(vertexBuffer[i + 11], vertexBuffer[i + 12], vertexBuffer[i + 13]));

		}
	}

	m_indexBuffer.reserve(indexBuffer.size());
	std::copy(indexBuffer.begin(), indexBuffer.end(), std::back_inserter(m_indexBuffer));

	createBuffer();
}

std::vector<Vector3f>& Shape::getPositions() {
	return m_positions;
}

std::vector<unsigned int>& Shape::getIndexBuffer() {
	return m_indexBuffer;
}

int Shape::getNumberOfTriangles() {
	return m_drawCount / 3;
}

Shape::~Shape() {
	cleanup();
}

void Shape::cleanup() {

	if (m_vao)
		glDeleteVertexArrays(1, &m_vao);

	if (m_vbo[0])
		glDeleteBuffers(1, &m_vbo[0]);

	if (m_vbo[1])
		glDeleteBuffers(1, &m_vbo[1]);

	if (m_vbo[2])
		glDeleteBuffers(1, &m_vbo[2]);

	if (m_vbo[3])
		glDeleteBuffers(1, &m_vbo[3]);

	if (m_vbo[4])
		glDeleteBuffers(1, &m_vbo[4]);

	if (m_vboInstances)
		glDeleteBuffers(1, &m_vboInstances);

	m_positions.clear();
	m_positions.shrink_to_fit();
	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	m_instances.clear();
	m_instances.shrink_to_fit();
}

void Shape::createBuffer() {
	m_drawCount = m_indexBuffer.size();

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(!m_positions.empty() + !m_texels.empty() + !m_normals.empty() + !m_tangents.empty() + !m_bitangents.empty(), m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	if (!m_texels.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (!m_normals.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, !m_texels.empty() ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//tangents
	if (!m_tangents.empty()) {
		glBindBuffer(GL_ARRAY_BUFFER, (!m_texels.empty() && !m_normals.empty()) ? m_vbo[3] : (!m_texels.empty() || !m_normals.empty()) ? m_vbo[2] : m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * sizeof(m_tangents[0]), &m_tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, (!m_texels.empty() && !m_normals.empty()) ? m_vbo[4] : (!m_texels.empty() || !m_normals.empty()) ? m_vbo[3] : m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, m_bitangents.size() * sizeof(m_bitangents[0]), &m_bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(m_indexBuffer[0]), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	//m_positions.clear();
	//m_positions.shrink_to_fit();
	//m_indexBuffer.clear();
	//m_indexBuffer.shrink_to_fit();
	m_texels.clear();
	m_texels.shrink_to_fit();
	m_normals.clear();
	m_normals.shrink_to_fit();
	m_tangents.clear();
	m_tangents.shrink_to_fit();
	m_bitangents.clear();
	m_bitangents.shrink_to_fit();
}

void Shape::drawRaw() const {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void Shape::addInstances(const std::vector<Matrix4f>& modelMTX) {
	m_instances.insert(m_instances.end(), modelMTX.begin(), modelMTX.end());
	m_instanceCount = m_instances.size();

	glGenBuffers(1, &m_vboInstances);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
	glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindVertexArray(0);
}

void Shape::addInstance(const Matrix4f& modelMTX) {
	m_instances.push_back(modelMTX);
	m_instanceCount = m_instances.size();

	if (m_vboInstances) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

	}else {
		glGenBuffers(1, &m_vboInstances);
		glBindVertexArray(m_vao);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboInstances);
		glBufferData(GL_ARRAY_BUFFER, m_instances.size() * sizeof(float) * 4 * 4, m_instances[0][0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glEnableVertexAttribArray(6);
		glEnableVertexAttribArray(7);
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(0));
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 4));
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 8));
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4 * 4, (void*)(sizeof(float) * 12));

		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);

		glBindVertexArray(0);
	}
}

void Shape::drawRawInstanced() const {
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, m_instanceCount);
	glBindVertexArray(0);
}