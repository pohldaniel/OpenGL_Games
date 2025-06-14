#include <algorithm>
#include "Batchrenderer.h"
#include "Framebuffer.h"

Batchrenderer Batchrenderer::s_instance;
unsigned int Batchrenderer::s_drawCallCount = 0;
unsigned int Batchrenderer::s_primitiveCount = 0;

void Batchrenderer::setShader(const Shader* shader) {
	m_shader = shader;
}

const Shader* Batchrenderer::getShader() {
	return m_shader;
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
	bufferPtr = nullptr;

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}

	if (m_vboSingle) {
		glDeleteBuffers(1, &m_vboSingle);
		m_vboSingle = 0u;
		glDeleteVertexArrays(1, &m_vaoSingle);
		m_vaoSingle = 0u;
	}
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
	glBindBuffer(GL_ARRAY_BUFFER, 0);

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

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_maxIndex, indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

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
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		const GLushort _indices[] = { 0, 1, 2, 2, 3, 0 };

		unsigned int iboSingle;
		glGenBuffers(1, &iboSingle);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboSingle);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(_indices), _indices, GL_STATIC_DRAW);

		glBindVertexArray(0);

		glDeleteBuffers(1, &iboSingle);
	}

	left = FLT_MAX;
	right = -FLT_MAX;
	bottom = FLT_MAX;
	top = -FLT_MAX;
}

void Batchrenderer::shutdown() {

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0u;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0u;
	}
	
	if (m_vboSingle) {
		glDeleteBuffers(1, &m_vboSingle);
		m_vboSingle = 0u;
		glDeleteVertexArrays(1, &m_vaoSingle);
		m_vaoSingle = 0u;
	}
}

void Batchrenderer::blitBufferToTexture(int widthDst, int heightDst, int paddingX, int paddingY, Texture& texture) {
	GLfloat color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	Rect blitRect;
	getBlitRect(blitRect);

	unsigned int renderTarget, renderBuffer, blitTarget;
	glGenRenderbuffers(1, &renderBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, Framebuffer::GetDefaultWidth(), Framebuffer::GetDefaultHeight());
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glGenFramebuffers(1, &renderTarget);
	glBindFramebuffer(GL_FRAMEBUFFER, renderTarget);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, renderBuffer);

	glClear(GL_COLOR_BUFFER_BIT);
	drawBuffer();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(color[0], color[1], color[2], color[3]);

	glGenFramebuffers(1, &blitTarget);
	glBindFramebuffer(GL_FRAMEBUFFER, blitTarget);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.getTexture(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, renderTarget);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, blitTarget);
	glBlitFramebuffer(blitRect.posX - paddingX / 2, blitRect.posY - paddingY / 2, blitRect.width + paddingX / 2, blitRect.height + paddingY / 2, 0, 0, widthDst, heightDst, GL_COLOR_BUFFER_BIT, GL_LINEAR);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glDeleteFramebuffers(1, &renderTarget);
	glDeleteFramebuffers(1, &blitTarget);
	glDeleteRenderbuffers(1, &renderTarget);
}

void Batchrenderer::setBlitSize(unsigned int width, unsigned int height) {
	Framebuffer::SetDefaultSize(width, height);
	glViewport(0, 0, width, height);
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
	//s_primitiveCount++;


	left = std::min(left, posSize[0]);
	bottom = std::min(bottom, posSize[1]);
	right = std::max(right, posSize[0] + posSize[2]);
	top = std::max(top, posSize[1] + posSize[3]);

}

void Batchrenderer::addDiamondAA(Vector4f posSize, Vector4f texPosSize, Vector4f color, unsigned int frame) {
	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { posSize[0] - posSize[2] * 0.5f, posSize[1], texPosSize[0],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0], posSize[1] - posSize[3] * 0.5f,   texPosSize[0] + texPosSize[2],  texPosSize[1] };
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
	//s_primitiveCount++;
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
	//s_primitiveCount++;
}

void Batchrenderer::addHexagonFlip(Vector4f posSize, Vector4f color, unsigned int frame) {
	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { posSize[0] - posSize[2] * 0.5f, posSize[1] - 0.25f * posSize[3], 0.0f, 0.25f };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { posSize[0], posSize[1] - 0.5f  * posSize[3], 0.5f, 0.0f };
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
	//s_primitiveCount++;
}

void Batchrenderer::addQuad(Vector2f vertices[4], Vector4f texPosSize, Vector4f color, unsigned int frame) {
	if (indexCount >= m_maxIndex) {
		m_drawRaw ? drawBufferRaw() : drawBuffer();
	}

	bufferPtr->posTex = { vertices[0][0], vertices[0][1],  texPosSize[0],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { vertices[1][0], vertices[1][1],  texPosSize[0] + texPosSize[2],  texPosSize[1] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { vertices[2][0], vertices[2][1],  texPosSize[0] + texPosSize[2],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	bufferPtr->posTex = { vertices[3][0], vertices[3][1],  texPosSize[0],  texPosSize[1] + texPosSize[3] };
	bufferPtr->color = { color[0], color[1], color[2], color[3] };
	bufferPtr->frame = frame;
	bufferPtr++;

	indexCount += 6;
	//s_primitiveCount++;
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
	//s_primitiveCount++;
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
	//s_primitiveCount++;
}

void Batchrenderer::getBlitRect(Rect& rect) {
	size_t size = ((uint8_t*)bufferPtr - (uint8_t*)buffer) / sizeof(Vertex);

	if (size == 0)
		rect = { 0.0f, 0.0f, 0.0f, 0.0f };

	rect.posX = left; rect.posY = bottom;
	rect.width = right; rect.height = top;
}

void Batchrenderer::drawBuffer() {
	//s_drawCallCount++;
	GLsizeiptr size = (uint8_t*)bufferPtr - (uint8_t*)buffer;
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
	m_shader->use();
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shader->unuse();

	indexCount = 0;
	bufferPtr = buffer;
	left = FLT_MAX;
	right = -FLT_MAX;
	bottom = FLT_MAX;
	top = -FLT_MAX;
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
	left = FLT_MAX;
	right = -FLT_MAX;
	bottom = FLT_MAX;
	top = -FLT_MAX;
}

void Batchrenderer::drawSingleQuadAA(Vector4f posSize, Vector4f texPosSize, Vector4f color, unsigned int frame) {

	glBindBuffer(GL_ARRAY_BUFFER, m_vboSingle);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, NULL, GL_DYNAMIC_DRAW);
	Vertex* ptr = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[0] = { posSize[0], posSize[1], texPosSize[0], texPosSize[1], color[0], color[1], color[2], color[3], frame };
		ptr[1] = { posSize[0] + posSize[2], posSize[1], texPosSize[0] + texPosSize[2],  texPosSize[1], color[0], color[1], color[2], color[3],  frame };
		ptr[2] = { posSize[0] + posSize[2], posSize[1] + posSize[3], texPosSize[0] + texPosSize[2], texPosSize[1] + texPosSize[3], color[0], color[1], color[2], color[3], frame };
		ptr[3] = { posSize[0], posSize[1] + posSize[3], texPosSize[0], texPosSize[1] + texPosSize[3], color[0], color[1], color[2], color[3], frame };
		
		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	m_shader->use();
	glBindVertexArray(m_vaoSingle);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shader->unuse();
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
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 4, NULL, GL_DYNAMIC_DRAW);
	Vertex* ptr = (Vertex*)glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	if (ptr) {
		ptr[0] = { quadPos[0], quadPos[1], texPos[0], texPos[1], color[0], color[1], color[2], color[3], frame };
		ptr[1] = { quadPos[2], quadPos[3], texPos[2], texPos[3], color[0], color[1], color[2], color[3], frame };
		ptr[2] = { quadPos[4], quadPos[5], texPos[4], texPos[5], color[0], color[1], color[2], color[3], frame };
		ptr[3] = { quadPos[6], quadPos[7], texPos[6], texPos[7], color[0], color[1], color[2], color[3], frame };

		glUnmapBuffer(GL_ARRAY_BUFFER);
	}

	m_shader->use();
	glBindVertexArray(m_vaoSingle);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	m_shader->unuse();
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
	s_primitiveCount = 0;
}

void Batchrenderer::PrintStatistic() {
	std::cout << "Draw Calls: " << s_drawCallCount << std::endl;
	std::cout << "Quad Count: " << s_primitiveCount << std::endl;
	std::cout << "------------------" << std::endl;
	ResetStatistic();
}