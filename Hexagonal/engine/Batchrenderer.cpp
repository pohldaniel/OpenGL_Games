#include <algorithm>
#include "Batchrenderer.h"

Batchrenderer Batchrenderer::s_instance;
unsigned int Batchrenderer::s_drawCallCount = 0;
unsigned int Batchrenderer::s_quadCount = 0;

void Batchrenderer::setShader(const Shader* shader) {
	m_shader = shader;
}

float(&Batchrenderer::getQuadPos())[8]{
	return quadPos;
}

float(&Batchrenderer::getTexPos())[8]{
	return texPos;
}

float(&Batchrenderer::getColor())[4]{
	return color;
}

unsigned int& Batchrenderer::getFrame() {
	return frame;
}

Batchrenderer& Batchrenderer::Get() {
	return s_instance;
}

Batchrenderer::~Batchrenderer() {
	delete[] buffer;
	buffer = nullptr;	
}

void Batchrenderer::init(size_t size, bool drawSingle, bool drawRaw) {

	m_drawRaw = drawRaw;
	m_maxQuad = size;
	m_maxVert = m_maxQuad * 4;
	m_maxIndex = m_maxQuad * 6;

	buffer = new Vertex[m_maxVert];

	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxVert * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW); // dynamic

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(2 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(4 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)(8 * sizeof(float)));

	uint32_t* indices = new uint32_t[m_maxIndex];
	uint32_t index_offset = 0;
	for (int i = 0; i < m_maxIndex; i += 6) {
		indices[i + 0] = 0 + index_offset;
		indices[i + 1] = 1 + index_offset;
		indices[i + 2] = 2 + index_offset;

		indices[i + 3] = 2 + index_offset;
		indices[i + 4] = 3 + index_offset;
		indices[i + 5] = 0 + index_offset;

		index_offset += 4;
	}

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_maxIndex, indices, GL_STATIC_DRAW);

	// unbind vbo and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &m_ibo);

	delete[] indices;

	bufferPtr = buffer;

	if (drawSingle) {
		glGenBuffers(1, &m_vboSingle);

		glGenVertexArrays(1, &m_vaoSingle);
		glBindVertexArray(m_vaoSingle);

		glBindBuffer(GL_ARRAY_BUFFER, m_vboSingle);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW); // dynamic

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(2 * sizeof(float)));

		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(4 * sizeof(float)));

		glEnableVertexAttribArray(3);
		glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)(8 * sizeof(float)));

		const GLushort _indices[] = { 0, 1, 2, 2, 3, 0 };

		glGenBuffers(1, &m_iboSingle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_iboSingle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

		// unbind vbo and vao
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		glDeleteBuffers(1, &m_iboSingle);
	}
}

void Batchrenderer::shutdown() {

	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);
	
	if (m_vboSingle) {
		glDeleteBuffers(1, &m_vboSingle);
		glDeleteVertexArrays(1, &m_vaoSingle);
	}
}

void Batchrenderer::addQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color, unsigned int frame) {

	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { posSize[0], posSize[1],  texPosSize[0],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2], posSize[1],  texPosSize[0] + texPosSize[2],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2], posSize[1] + posSize[3],  texPosSize[0] + texPosSize[2],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0], posSize[1] + posSize[3],  texPosSize[0],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 6;
	//s_quadCount++;
}

void Batchrenderer::addDiamondAA(Vector4f posSize, Vector4f texPosSize, Vector4f color, unsigned int frame) {
	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { posSize[0] - posSize[2] * 0.5f, posSize[1], texPosSize[0],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0], posSize[1] -posSize[3] * 0.5f,   texPosSize[0] + texPosSize[2],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2] * 0.5f, posSize[1],  texPosSize[0] + texPosSize[2],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0], posSize[1] + posSize[3] * 0.5f,   texPosSize[0],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 6;
}

void Batchrenderer::addHexagon(Vector4f posSize, Vector4f color, unsigned int frame) {

	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { posSize[0] - 0.25f * posSize[2], posSize[1] + posSize[3] * 0.5f, 0.25f, 1.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] - 0.5f  * posSize[2], posSize[1],   0.0f, 0.5f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] - 0.25f * posSize[2], posSize[1] - posSize[3] * 0.5f,  0.25f, 0.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + 0.25f * posSize[2], posSize[1] + posSize[3] * 0.5f,   0.75f, 1.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + 0.25f * posSize[2], posSize[1] + posSize[3] * 0.5f, 0.75f, 1.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] - 0.25f * posSize[2], posSize[1] - posSize[3] * 0.5f,   0.25f, 0.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + 0.25f * posSize[2], posSize[1] - posSize[3] * 0.5f,  0.75f, 0.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + 0.5f  * posSize[2], posSize[1],   1.0f, 0.5f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 12;
}

void Batchrenderer::addHexagonFlip(Vector4f posSize, Vector4f color, unsigned int frame) {
	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { posSize[0] - posSize[2] * 0.5f, posSize[1] - 0.25f * posSize[3], 0.0f, 0.25f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0],                     posSize[1] - 0.5f  * posSize[3],   0.5f, 0.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2] * 0.5f, posSize[1] - 0.25f * posSize[3],  1.0f, 0.25f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2] * 0.5f, posSize[1] + 0.25f  * posSize[3],   1.0f, 0.75f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] - posSize[2] * 0.5f, posSize[1] - 0.25f * posSize[3], 0.0f, 0.25f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] + posSize[2] * 0.5f, posSize[1] + 0.25f * posSize[3],   1.0f, 0.75f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0],                     posSize[1] + 0.5f  * posSize[3],  0.5f, 1.0f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0] - posSize[2] * 0.5f, posSize[1] + 0.25f * posSize[3],   0.0f, 0.75f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;
	indexCount += 12;
}

void Batchrenderer::addRotatedQuadRH(Vector4f posSize, float angle, float rotX, float rotY, Vector4f texPosSize, Vector4f color, unsigned int frame) {
	
	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	angle *= PI_ON_180;
	float s = sinf(angle);
	float c = cosf(angle);
	float _x = posSize[0];
	float _y = posSize[1];

	bufferPtr->posTex = { (1.0f - c) * rotX + rotY * s + _x, (1.0f - c) * rotY - rotX * s + _y, texPosSize[0], texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { (posSize[2] - rotX) * c + rotY * s + rotX + _x, (posSize[2] - rotX) * s + (1.0f - c) * rotY + _y, texPosSize[0] + texPosSize[2], texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { (posSize[2] - rotX) * c - (posSize[3] - rotY) * s + rotX + _x, (posSize[2] - rotX) * s + (posSize[3] - rotY) * c + rotY + _y, texPosSize[0] + texPosSize[2], texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { -rotX * c - (posSize[3] - rotY) * s + rotX + _x, -rotX * s + (posSize[3] - rotY) * c + rotY + _y, texPosSize[0], texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 6;
}

void Batchrenderer::addRotatedQuadLH(Vector4f posSize, float angle, float rotX, float rotY, Vector4f texPosSize, Vector4f color, unsigned int frame) {
	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	angle *= PI_ON_180;
	float s = sinf(angle);
	float c = cosf(angle);
	float _x = posSize[0];
	float _y = posSize[1];

	bufferPtr->posTex = { (1.0f - c) * rotX - rotY * s + _x, (1.0f - c) * rotY + rotX * s + _y, texPosSize[0], texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { (posSize[2] - rotX) * c - rotY * s + rotX + _x, (rotX - posSize[2]) * s + (1.0f - c) * rotY + _y, texPosSize[0] + texPosSize[2], texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { (posSize[2] - rotX) * c + (posSize[3] - rotY) * s + rotX + _x, (rotX - posSize[2]) * s + (posSize[3] - rotY) * c + rotY + _y, texPosSize[0] + texPosSize[2], texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { -rotX * c + (posSize[3] - rotY) * s + rotX + _x, rotX * s + (posSize[3] - rotY) * c + rotY + _y, texPosSize[0], texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 6;
}

void Batchrenderer::drawBuffer() {
	//s_drawCallCount++;
	GLsizeiptr size = (uint8_t*)bufferPtr - (uint8_t*)buffer;
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
	glUseProgram(m_shader->m_program);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	indexCount = 0;
	bufferPtr = buffer;
}

void Batchrenderer::drawBufferRaw() {
	//s_drawCallCount++;
	GLsizeiptr size = (uint8_t*)bufferPtr - (uint8_t*)buffer;
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	indexCount = 0;
	bufferPtr = buffer;
}

void Batchrenderer::drawSingleQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color, unsigned int frame) {

	glBindBuffer(GL_ARRAY_BUFFER, m_vboSingle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, NULL, GL_STATIC_DRAW);
	Vertex* ptr = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[0] = { posSize[0], posSize[1], texPosSize[0], texPosSize[1], color[0], color[1], color[2], color[3], frame };
		ptr[1] = { posSize[0] + posSize[2], posSize[1], texPosSize[0] + texPosSize[2],  texPosSize[1], color[0], color[1], color[2], color[3],  frame };
		ptr[2] = { posSize[0] + posSize[2], posSize[1] + posSize[3], texPosSize[0] + texPosSize[2], texPosSize[1] + texPosSize[3], color[0], color[1], color[2], color[3], frame };
		ptr[3] = { posSize[0], posSize[1] + posSize[3], texPosSize[0], texPosSize[1] + texPosSize[3], color[0], color[1], color[2], color[3], frame };
		
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	glUseProgram(m_shader->m_program);
	glBindVertexArray(m_vaoSingle);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Batchrenderer::processQuad() {

	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { quadPos[0], quadPos[1], texPos[0], texPos[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { quadPos[2], quadPos[3], texPos[2], texPos[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { quadPos[4], quadPos[5], texPos[4], texPos[5] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { quadPos[6], quadPos[7], texPos[6], texPos[7] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 6;
}

void Batchrenderer::processSingleQuad() {
	glBindBuffer(GL_ARRAY_BUFFER, m_vboSingle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, NULL, GL_STATIC_DRAW);
	Vertex* ptr = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[0] = { quadPos[0], quadPos[1], texPos[0], texPos[1], color[0], color[1], color[2], color[3], frame };
		ptr[1] = { quadPos[2], quadPos[3], texPos[2], texPos[3], color[0], color[1], color[2], color[3], frame };
		ptr[2] = { quadPos[4], quadPos[5], texPos[4], texPos[5], color[0], color[1], color[2], color[3], frame };
		ptr[3] = { quadPos[6], quadPos[7], texPos[6], texPos[7], color[0], color[1], color[2], color[3], frame };

		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	glUseProgram(m_shader->m_program);
	glBindVertexArray(m_vaoSingle);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);
}

void Batchrenderer::bindTexture(unsigned int texture, bool isTextureArray, unsigned int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(isTextureArray ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture);
}

void Batchrenderer::unbindTexture(bool isTextureArray, unsigned int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
	glBindTexture(isTextureArray ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, 0);
}

void Batchrenderer::bindTexture(unsigned int texture, bool isTextureArray) {
	glBindTexture(isTextureArray ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, texture);
}

void Batchrenderer::unbindTexture(bool isTextureArray) {
	glBindTexture(isTextureArray ? GL_TEXTURE_2D_ARRAY : GL_TEXTURE_2D, 0);
}

void Batchrenderer::activeTexture(unsigned int unit) {
	glActiveTexture(GL_TEXTURE0 + unit);
}

void Batchrenderer::ResetStatistic() {
	s_drawCallCount = 0;
	s_quadCount = 0;
}

void Batchrenderer::PrintStatistic() {
	std::cout << "Draw Calls: " << s_drawCallCount << std::endl;
	std::cout << "Quad Count: " << s_quadCount << std::endl;
	std::cout << "------------------" << std::endl;
	ResetStatistic();
}