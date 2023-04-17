#include "Shape.h"
#include "Capsule.h"
#include "MeshTorus.h"
#include "MeshSphere.h"
#include <iostream>

Shape::Shape() { }

void Shape::buildCapsule(float radius, float length, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) {
	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;
	m_generateNormalDerivatives = generateNormalDerivatives;
	m_uResolution = uResolution;
	m_vResolution = vResolution;
	m_numBuffers = 1 + m_generateTexels + m_generateNormals + 2 * m_generateTangents + 2 * m_generateNormalDerivatives;
	Capsule::BuildMesh(radius, length, m_position, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_positions, m_texels, m_normals, m_indexBuffer);
	createBuffer();
}

void Shape::buildTorus(float radius, float tubeRadius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) {
	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;
	m_generateNormalDerivatives = generateNormalDerivatives;
	m_uResolution = uResolution;
	m_vResolution = vResolution;
	m_numBuffers = 1 + m_generateTexels + m_generateNormals + 2 * m_generateTangents + 2 * m_generateNormalDerivatives;
	MeshTorus::BuildMesh(radius, tubeRadius, m_position, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_generateNormalDerivatives, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents, m_normalsDu, m_normalsDv);
	createBuffer();
}

void Shape::buildSphere(float radius, const Vector3f& position, int uResolution, int vResolution, bool generateTexels, bool generateNormals, bool generateTangents, bool generateNormalDerivatives) {
	m_position = position;
	m_generateNormals = generateNormals;
	m_generateTexels = generateTexels;
	m_generateTangents = generateTangents;
	m_generateNormalDerivatives = generateNormalDerivatives;
	m_uResolution = uResolution;
	m_vResolution = vResolution;
	m_numBuffers = 1 + m_generateTexels + m_generateNormals + 2 * m_generateTangents + 2 * m_generateNormalDerivatives;
	MeshSphere::BuildMesh(radius, m_position, m_uResolution, m_vResolution, m_generateTexels, m_generateNormals, m_generateTangents, m_generateNormalDerivatives, m_positions, m_texels, m_normals, m_indexBuffer, m_tangents, m_bitangents, m_normalsDu, m_normalsDv);
	createBuffer();
}


void Shape::createBuffer() {
	m_drawCount = m_indexBuffer.size();

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(m_numBuffers, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	//Position
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_positions.size() * sizeof(m_positions[0]), &m_positions[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Texture Coordinates
	if (m_generateTexels) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
		glBufferData(GL_ARRAY_BUFFER, m_texels.size() * sizeof(m_texels[0]), &m_texels[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (m_generateNormals) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
		glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(m_normals[0]), &m_normals[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	//Normals
	if (m_generateTangents) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[3]);
		glBufferData(GL_ARRAY_BUFFER, m_tangents.size() * sizeof(m_tangents[0]), &m_tangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[4]);
		glBufferData(GL_ARRAY_BUFFER, m_bitangents.size() * sizeof(m_bitangents[0]), &m_bitangents[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	if (m_generateNormalDerivatives) {
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[5]);
		glBufferData(GL_ARRAY_BUFFER, m_normalsDu.size() * sizeof(m_normalsDu[0]), &m_normalsDu[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ARRAY_BUFFER, m_vbo[6]);
		glBufferData(GL_ARRAY_BUFFER, m_normalsDv.size() * sizeof(m_normalsDv[0]), &m_normalsDv[0], GL_STATIC_DRAW);

		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
	m_normalsDu.clear();
	m_normalsDu.shrink_to_fit();
	m_normalsDv.clear();
	m_normalsDv.shrink_to_fit();
}

void Shape::setPrecision(int uResolution, int vResolution) {
	m_uResolution = uResolution;
	m_vResolution = vResolution;
}

int Shape::getNumberOfTriangles() {
	return m_drawCount / 3;
}

void Shape::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}