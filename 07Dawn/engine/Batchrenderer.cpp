#include "Batchrenderer.h"
#include <algorithm>
#include "../Constants.h"

//Batchrenderer Batchrenderer::s_instance = Batchrenderer(ViewPort::get().getCamera());
//
//Batchrenderer::Batchrenderer(const Camera& camera) : m_cameraRef(camera){
//
//}

Batchrenderer Batchrenderer::s_instance;

void Batchrenderer::setCamera(const Camera& camera) {
	m_camera = &camera;
}

Batchrenderer& Batchrenderer::get() {
	return s_instance;
}

Batchrenderer::~Batchrenderer() {
	shutdown();
}

void Batchrenderer::init()  {

	m_shader = Globals::shaderManager.getAssetPointer("batch");
	buffer = new Vertex[max_quad_vert_count];

	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, max_quad_vert_count * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW); // dynamic

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(Vertex), (const void*)(5 * sizeof(float)));
	
	uint32_t indices[max_quad_index_count];
	uint32_t index_offset = 0;
	for (int i = 0; i < max_quad_index_count; i += 6) {
		indices[i + 0] = 0 + index_offset;
		indices[i + 1] = 1 + index_offset;
		indices[i + 2] = 2 + index_offset;

		indices[i + 3] = 2 + index_offset;
		indices[i + 4] = 3 + index_offset;
		indices[i + 5] = 0 + index_offset;

		index_offset += 4;
	}

	glGenBuffers(1, &m_indexbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// unbind vbo and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &m_indexbuffer);
}


void Batchrenderer::shutdown(){
	glDeleteBuffers(1, &m_vbo);
	glDeleteVertexArrays(1, &m_vao);

	delete[] buffer;

	_buffer.clear();
	_buffer.shrink_to_fit();
}

void Batchrenderer::endBatch(){
	GLsizeiptr size = (uint8_t*)buffer_ptr - (uint8_t*)buffer;

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
}


void Batchrenderer::flush(){
	
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", m_camera->getOrthographicMatrix() * m_camera->getViewMatrix());

	
	
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

	

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	index_count = 0;
}

void Batchrenderer::beginBatch(){
	buffer_ptr = buffer;
}

void Batchrenderer::addQuad(Vector4f position, Vector4f texCoord, unsigned int frame){
  if (index_count >= max_quad_index_count) {
    endBatch();
    flush();
    beginBatch();
  }
 
  buffer_ptr->position = { position[0], position[1], 0.0f};
  buffer_ptr->texCoord = { texCoord[0],  texCoord[1]};
  buffer_ptr->frame = frame;
  buffer_ptr++;

  buffer_ptr->position = { (position[0] + position[2]), position[1], 0.0f};
  buffer_ptr->texCoord = { texCoord[2],  texCoord[1] };
  buffer_ptr->frame = frame;
  buffer_ptr++;

  buffer_ptr->position = { (position[0] + position[2]), (position[1] + position[3]), 0.0f};
  buffer_ptr->texCoord = { texCoord[2],  texCoord[3] };
  buffer_ptr->frame = frame;
  buffer_ptr++;

  buffer_ptr->position = { position[0], (position[1] + position[3]), 0.0f};
  buffer_ptr->texCoord = { texCoord[0],  texCoord[3] };
  buffer_ptr->frame = frame;
  buffer_ptr++;

  index_count += 6;
}