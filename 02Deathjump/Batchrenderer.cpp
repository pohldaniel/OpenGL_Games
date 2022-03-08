#include "Batchrenderer.h"
#include <algorithm>
Batchrenderer::~Batchrenderer() {

}

void Batchrenderer::init() {

	m_shader = new Shader("shader/quad_color.vs", "shader/quad_color.fs");
	buffer = new Vertex[max_quad_vert_count];
	//_buffer.resize(max_quad_vert_count);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, max_quad_vert_count * sizeof(Vertex), nullptr, GL_DYNAMIC_DRAW); // dynamic

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)(4 * sizeof(float)));

	

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

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// unbind vbo and vao
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Batchrenderer::resetStats() {
	draw_calls = 0;
	quad_vertex = 0;
}

void Batchrenderer::shutdown(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	delete[] buffer;
}

void Batchrenderer::endBatch(){
	GLsizeiptr size = (uint8_t*)buffer_ptr - (uint8_t*)buffer;

	// Set dynamic vertex buffer & upload data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	// glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
	glBufferSubData(GL_ARRAY_BUFFER, 0, size, buffer);
}


void Batchrenderer::flush(){

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Globals::projection);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

	draw_calls += 1;
	index_count = 0;

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	//std::cout << "Draw Calls: " << draw_calls << std::endl;
}

void Batchrenderer::beginBatch(){
	buffer_ptr = buffer;
}

void Batchrenderer::addQuad(Vector2f position, Vector2f size, Vector4f color){
  if (index_count >= max_quad_index_count) {
    endBatch();
    flush();
    beginBatch();
  }
  float xpos = position[0];
  float ypos = position[1];
  float w = size[0];
  float h = size[1];



  buffer_ptr->posTex = { xpos, ypos, 0.0f, 0.0f};
  buffer_ptr->color = color;
  buffer_ptr++;

  buffer_ptr->posTex = { (xpos + w), ypos, 1.0f, 0.0f };
  buffer_ptr->color = color;
  buffer_ptr++;

  buffer_ptr->posTex = { (xpos + w), (ypos + h), 1.0f, 1.0f };
  buffer_ptr->color = color;
  buffer_ptr++;

  buffer_ptr->posTex = { xpos, (ypos + h), 0.0f, 1.0f };
  buffer_ptr->color = color;
  buffer_ptr++;

  index_count += 6;
  quad_vertex += 4;

  std::cout << index_count << std::endl;
}

void Batchrenderer::addVertex(Vector2f position, Vector4f color) {
	if (index_count >= max_quad_index_count) {
		endBatch();
		flush();
		beginBatch();
	}
	float xpos = position[0];
	float ypos = position[1];
	

	buffer_ptr->posTex = { xpos, ypos, 0.0f, 0.0f };
	buffer_ptr->color = color;
	buffer_ptr++;

	quad_vertex += 1;

	if (quad_vertex % 4 == 0) {
		index_count += 6;
	}
}

void Batchrenderer::addQuad(std::vector<Vertex> particles) {
	
	if (_buffer.size() >= max_quad_vert_count) {
		endBatch2();
		flush2();
		beginBatch2();
	}

	_buffer.insert(std::end(_buffer), std::begin(particles), std::end(particles));
	index_count += 6 * _buffer.size() * 0.25;
	quad_vertex += particles.size();
}

void Batchrenderer::endBatch2() {
	// Set dynamic vertex buffer & upload data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * _buffer.size(), &_buffer[0]);
}
void Batchrenderer::beginBatch2() {
	_buffer.resize(0);
}

void Batchrenderer::flush2() {
	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Globals::projection);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, nullptr);

	draw_calls += 1;
	index_count = 0;

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glUseProgram(0);

	std::cout << "Draw Calls: " << draw_calls << std::endl;
}

void Batchrenderer::resetStats2() {
	draw_calls = 0;
	quad_vertex = 0;
}
