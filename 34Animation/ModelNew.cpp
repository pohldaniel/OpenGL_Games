#include <GL/glew.h>
#include <engine/BuiltInShader.h>
#include "ModelNew.h"

ModelNew::ModelNew() {

	solidConverter.solidToBuffer("res/Body.solid", true, { 180.0f, 0.0f, 0.0f }, { 0.04f, 0.04f, 0.04f }, m_vertexBufferMap, m_indexBuffer);
	solidConverter.fromBufferMap(m_vertexBufferMap, m_vertexBufferDraw);
	solidConverter.loadSkeleton("res/BasicFigure", m_vertexBufferMap, m_vertexBuffer, m_skeleton, m_weights, m_boneIds);

	m_vertexNum = m_vertexBufferDraw.size() / 5;
	m_muscleNum = m_skeleton.m_muscles.size();

	m_drawCount = m_indexBuffer.size();

	glGenBuffers(1, &BuiltInShader::matrixUbo);
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f) * 96, NULL, GL_DYNAMIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 3, BuiltInShader::matrixUbo, 0, sizeof(Matrix4f) * 96);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	m_skinMatrices = new Matrix4f[m_muscleNum];

	for (size_t i = 0; i < m_muscleNum; ++i) {
		m_skinMatrices[i] = m_skeleton.m_muscles[i].offsetMatrix;
		//m_skinMatrices[i] = Matrix4f::IDENTITY;
	}

	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_muscleNum, m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	buffer = new Vertex[m_vertexNum];

	glGenBuffers(3, m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, m_vertexNum * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(3 * sizeof(float)));

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, m_weights.size() * sizeof(float) * 4, &m_weights.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 0, 0);


	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, m_boneIds.size() * sizeof(std::array<unsigned int, 4>), &m_boneIds.front(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(6);
	glVertexAttribIPointer(6, 4, GL_UNSIGNED_INT, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * m_indexBuffer.size(), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	bufferPtr = buffer;
}

void ModelNew::draw() {

	for (int i = 0; i < m_vertexNum; i++) {
		bufferPtr->pos = { m_vertexBuffer[i * 3], m_vertexBuffer[i * 3 + 1], m_vertexBuffer[i * 3 + 2] };
		bufferPtr->tex = { m_vertexBufferDraw[i * 5 + 3], m_vertexBufferDraw[i * 5 + 4] };
		bufferPtr++;
	}


	glBindBuffer(GL_ARRAY_BUFFER, m_vbo[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexNum * sizeof(Vertex), buffer);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	bufferPtr = buffer;
}