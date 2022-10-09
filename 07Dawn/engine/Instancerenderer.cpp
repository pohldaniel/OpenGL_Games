#include "Instancerenderer.h"

Instancedrenderer Instancedrenderer::s_instance = Instancedrenderer(ViewPort::get().getCamera());

Instancedrenderer::Instancedrenderer(const Camera& camera) : m_camera(camera){}

void Instancedrenderer::setShader(Shader* shader) {
	m_shader = shader;
}

Instancedrenderer& Instancedrenderer::Get() {
	return s_instance;
}

Instancedrenderer::~Instancedrenderer() {
	delete[] buffer;
	buffer = nullptr;
}

void Instancedrenderer::init(size_t size) {

	static const GLfloat baseVertex[] = {
		0.0f, 0.0f,  0.0f, 0.0f,
		0.0f,  1.0f,  0.0f, 1.0f,
		1.0f,  1.0f, 1.0f, 1.0f,
		1.0f, 0.0f,  1.0f, 0.0f,		
	};

	m_maxQuad = size;
	m_maxVert = m_maxQuad * 4;

	buffer = new Vertex[m_maxVert];

	glGenBuffers(1, &m_vboBase);
	glGenBuffers(1, &m_vboInstance);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vboBase);
	glBufferData(GL_ARRAY_BUFFER, sizeof(baseVertex), baseVertex, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)(0));

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstance);
	glBufferData(GL_ARRAY_BUFFER, m_maxVert * 1 * sizeof(Vertex), NULL, GL_STREAM_DRAW);


	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(4 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)(8 * sizeof(float)));

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);
	glVertexAttribDivisor(3, 1);
	
	const GLushort indices[] = {
		0, 1, 2,
		0, 2, 3
	};

	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &m_ibo);


	bufferPtr = buffer;
}

void Instancedrenderer::shutdown() {
	glDeleteBuffers(1, &m_vboBase);
	glDeleteBuffers(1, &m_vboInstance);
	glDeleteVertexArrays(1, &m_vao);
}

void Instancedrenderer::addQuad(Vector4f posSize, Vector4f texPosSize, unsigned int frame) {

	if (m_instanceCount >= m_maxQuad) {
		drawBuffer();
	}

	//instanceBuffer.push_back({posSize[0], posSize[1], posSize[2], posSize[3],
	//						  texPosSize[0], texPosSize[1], texPosSize[2], texPosSize[3],
	//						  frame });

	bufferPtr->posSize = { posSize[0] , posSize[1] , posSize[2] , posSize[3] };
	bufferPtr->texPosSize = { texPosSize[0] , texPosSize[1] , texPosSize[2] , texPosSize[3] };
	bufferPtr->frame = frame;

	bufferPtr++;

	m_instanceCount++;
}

void Instancedrenderer::drawBuffer() {

	glBindBuffer(GL_ARRAY_BUFFER, m_vboInstance);
	glBufferSubData(GL_ARRAY_BUFFER, 0, m_instanceCount * sizeof(Vertex), buffer);

	//glBindBuffer(GL_ARRAY_BUFFER, m_vboInstance);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, instanceBuffer.size() * sizeof(Vertex), &instanceBuffer[0]);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_camera.getOrthographicMatrix() * m_camera.getViewMatrix());

	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0, m_instanceCount);
	glBindVertexArray(0);

	glUseProgram(0);

	m_instanceCount = 0;
	bufferPtr = buffer;
	
	//instanceBuffer.clear();
	//instanceBuffer.shrink_to_fit();
}