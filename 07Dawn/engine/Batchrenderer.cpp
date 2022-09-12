#include "Batchrenderer.h"
#include <algorithm>

//Batchrenderer Batchrenderer::s_instance = Batchrenderer(ViewPort::get().getCamera());
//
//Batchrenderer::Batchrenderer(const Camera& camera) : m_cameraRef(camera){
//
//}

Batchrenderer Batchrenderer::s_instance;

void Batchrenderer::setCamera(const Camera& camera) {
	m_camera = &camera;
}

void Batchrenderer::setShader(Shader* shader) {
	m_shader = shader;
}

Batchrenderer& Batchrenderer::get() {
	return s_instance;
}

Batchrenderer::~Batchrenderer() {
	//shutdown();
}

void Batchrenderer::init(size_t size)  {

	size_t m_maxQuad = size;
	size_t m_maxVert = m_maxQuad * 4;
	size_t m_maxIndex = m_maxQuad * 6;

	buffer = new Vertex[m_maxVert];

	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxVert * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW); // dynamic

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)(5 * sizeof(float)));
	
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
}


void Batchrenderer::shutdown(){
	glDeleteBuffers(1, &m_vbo);
	//glDeleteBuffers(1, &m_ibo);
	glDeleteVertexArrays(1, &m_vao);

	delete[] buffer;
}

void Batchrenderer::endBatch(){
	GLsizeiptr size = (uint8_t*)bufferPtr - (uint8_t*)buffer;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
}


void Batchrenderer::flush(){
	
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_camera->getOrthographicMatrix() * m_camera->getViewMatrix());
	
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	indexCount = 0;
}

void Batchrenderer::beginBatch(){
	bufferPtr = buffer;
}

void Batchrenderer::addQuad(Vector4f position, Vector4f texCoord, unsigned int frame){

  if (indexCount >= m_maxIndex) {
    endBatch();
    flush();
    beginBatch();
  }
 
  bufferPtr->position = { position[0], position[1], 0.0f};
  bufferPtr->texCoord = { texCoord[0],  texCoord[1]};
  bufferPtr->frame = frame;
  bufferPtr++;

  bufferPtr->position = { (position[0] + position[2]), position[1], 0.0f};
  bufferPtr->texCoord = { texCoord[0] + texCoord[2],  texCoord[1] };
  bufferPtr->frame = frame;
  bufferPtr++;

  bufferPtr->position = { (position[0] + position[2]), (position[1] + position[3]), 0.0f};
  bufferPtr->texCoord = { texCoord[0] + texCoord[2], texCoord[1] + texCoord[3] };
  bufferPtr->frame = frame;
  bufferPtr++;

  bufferPtr->position = { position[0], (position[1] + position[3]), 0.0f};
  bufferPtr->texCoord = { texCoord[0],  texCoord[1] + texCoord[3] };
  bufferPtr->frame = frame;
  bufferPtr++;

  indexCount += 6;
}