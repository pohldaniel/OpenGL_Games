#include "Text.h"

Text::Text(CharacterSetBmp &charset) : m_charset(charset) {

	m_fontShader = Globals::shaderManager.getAssetPointer("font");
	m_texture = &Globals::textureManager.get("verdana");
}

Text::Text(std::string label, CharacterSetBmp &charset) : Text(charset) {
	setLabel(label);
}

void Text::setLabel(std::string label) {

	m_label = label;
	
	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float x2 = 0.0f;
	float y2 = 900.0f;
	std::string::const_iterator c;
	for (c = m_label.begin(); c != m_label.end(); c++) {
		CharacterBmp ch = m_charset.characters[*c];
		Vector2f pos = Vector2f(x2 + ch.bearing[0] * m_scale, y2 - ch.bearing[1] * m_scale);
		addChar(pos, *c, vertices, indices);
		x2 += (ch.xAdvance * m_scale);
	}

	m_indexCount = indices.size();

	short stride = 5, offset = 3;

	unsigned int ibo;
	glGenBuffers(1, &ibo);
	glGenBuffers(1, &m_vbo);

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

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &ibo);

	vertices.clear();
	vertices.shrink_to_fit();

	indices.clear();
	indices.shrink_to_fit();
	
}

void Text::addChar(const Vector2f& pos, unsigned int _c, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
	CharacterBmp ch = m_charset.characters[_c];
	float w = ch.size[0] * m_scale;
	float h = ch.size[1] * m_scale;

	vertices.push_back(pos[0]); vertices.push_back(pos[1] - h); vertices.push_back(0.0f); vertices.push_back(ch.textureOffset[0]); vertices.push_back(ch.textureOffset[1] - ch.textureSize[1]);
	vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(ch.textureOffset[0]); vertices.push_back(ch.textureOffset[1]);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(ch.textureOffset[0] + ch.textureSize[0]); vertices.push_back(ch.textureOffset[1]);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] - h); vertices.push_back(0.0f); vertices.push_back(ch.textureOffset[0] + ch.textureSize[0]); vertices.push_back(ch.textureOffset[1] - ch.textureSize[1]);
	
	/*vertices.push_back(pos[0]); vertices.push_back(pos[1] - h); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0]); vertices.push_back(ch.textureOffset[1] + ch.textureSize[1]);
	vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0]); vertices.push_back(ch.textureOffset[1]);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0] + ch.textureSize[0]); vertices.push_back(ch.textureOffset[1]);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] - h); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0] + ch.textureSize[0]); vertices.push_back(ch.textureOffset[1] + ch.textureSize[1]);*/

	

	unsigned int currentOffset = (vertices.size() / 5) - 4;

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

void Text::render() {
	
	glUseProgram(m_fontShader->m_program);
	m_fontShader->loadMatrix("u_projection", Globals::ortoghraphic);

	m_charset.texture.bind(0);
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);
}