#include <GL/glew.h>
#include "Map.h"

void Map::createBuffer(std::vector<Tile>& tiles) {
	std::vector<float> vertices;
	std::vector<unsigned int> indices;
	std::vector<unsigned int> mapIndices;
	
	for (int i = 0; i < tiles.size(); i++) {


		Vector2f pos = tiles[i].position;
		float w = tiles[i].size[0];
		float h = tiles[i].size[1];

		vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
		vertices.push_back(pos[0]); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(1.0f);
		vertices.push_back(pos[0] + w); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
		vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

		mapIndices.push_back(tiles[i].gid); mapIndices.push_back(tiles[i].gid); mapIndices.push_back(tiles[i].gid); mapIndices.push_back(tiles[i].gid);

		unsigned int currentOffset = (vertices.size() / 5) - 4;

		indices.push_back(currentOffset + 0);
		indices.push_back(currentOffset + 1);
		indices.push_back(currentOffset + 2);

		indices.push_back(currentOffset + 0);
		indices.push_back(currentOffset + 2);
		indices.push_back(currentOffset + 3);
	}

	drawCount = indices.size();

	short stride = 5, offset = 3;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboMap);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//map indices
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMap);
	glBufferData(GL_ARRAY_BUFFER, mapIndices.size() * sizeof(mapIndices[0]), &mapIndices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Map::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, drawCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
