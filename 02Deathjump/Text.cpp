#include <cstring>
#include "Text.h"

Text::Text(std::string label, float scale) {
	m_characters = Globals::fontManager.get("font_90").characters;
	m_shaderText = Globals::shaderManager.getAssetPointer("text");
	//m_shaderText = new Shader("shader/text.vs", "shader/text.fs");

	m_label = label;
	m_scale = scale;
	calcSize();

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float x2 = 0.0f;
	float y2 = 0.0f;
	std::string::const_iterator c;
	for (c = m_label.begin(); c != m_label.end(); c++) {
		Character ch = m_characters[*c];
		Vector2f pos = Vector2f(x2 + ch.bearing[0] * m_scale, y2 + ch.bearing[1] * m_scale);
		addChar(pos, *c, vertices, indices);
		x2 += ch.advance[0] * m_scale;
		y2 += ch.advance[1] * m_scale;
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

Text::Text(size_t maxChar, float scale) {

	m_characters = Globals::fontManager.get("font_90").characters;
	m_shaderText = Globals::shaderManager.getAssetPointer("text");
	m_scale = scale;
	m_maxChar = maxChar;
	setMaxChar(m_maxChar);
	calcSize();
}

Text::~Text(){
	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}

	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	/*if (m_shaderText) {
		delete m_shaderText;
		m_shaderText = NULL;
	}*/
}

void Text::addChar(const Vector2f& pos, unsigned int _c, std::vector<float>& vertices, std::vector<unsigned int>& indices){
	Character ch = m_characters[_c];
	float w = ch.size[0] * m_scale;
	float h = ch.size[1] * m_scale;

	vertices.push_back(pos[0]);vertices.push_back(pos[1]);vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0]); vertices.push_back(ch.textureOffset[1]);
	vertices.push_back(pos[0]); vertices.push_back(pos[1] - h); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0]); vertices.push_back(ch.textureOffset[1] + ch.textureSize[1]);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] - h); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0] + ch.textureSize[0]); vertices.push_back(ch.textureOffset[1] + ch.textureSize[1]);
	//vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0]); vertices.push_back(ch.textureOffset[1]);
	//vertices.push_back(pos[0] + w); vertices.push_back(pos[1] - h); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0] + ch.textureSize[0]); vertices.push_back(ch.textureOffset[1] + ch.textureSize[1]);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0); vertices.push_back(ch.textureOffset[0] + ch.textureSize[0]); vertices.push_back(ch.textureOffset[1]);

	unsigned int currentOffset = (vertices.size() / 5) - 4;
	
	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

void Text::render() {
	glUseProgram(m_shaderText->m_program);
	m_shaderText->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shaderText->loadVector("textColor", m_color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Globals::fontManager.get("font_90").spriteSheet);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Text::render(Vector4f color) {
	m_color = color;
	render();
}

void Text::render(std::string label, float x, float y, float sx, float sy) {
	const size_t len = label.size();
	if (len > m_maxChar) return;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float x2 = x;
	float y2 = y;
	std::string::const_iterator c;
	for (c = label.begin(); c != label.end(); c++) {
		Character ch = m_characters[*c];
		Vector2f pos = Vector2f(x2 + ch.bearing[0] * sx, y2 + ch.bearing[1] * sy);
		addChar(pos, *c, vertices, indices);
		x2 += ch.advance[0] * sx;
		y2 += ch.advance[1] * sy;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(m_shaderText->m_program);
	m_shaderText->loadMatrix("u_transform", Globals::projection);
	m_shaderText->loadVector("textColor", m_color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Globals::fontManager.get("font_90").spriteSheet);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	/*vertices.clear();
	vertices.shrink_to_fit();

	indices.clear();
	indices.shrink_to_fit();*/
}

void Text::render(std::string label) {
	const size_t len = label.size();
	if (len > m_maxChar) return;

	std::vector<float> vertices;
	std::vector<unsigned int> indices;

	float x2 = 0.0f;
	float y2 = 0.0f;
	std::string::const_iterator c;
	for (c = label.begin(); c != label.end(); c++) {
		Character ch = m_characters[*c];
		Vector2f pos = Vector2f(x2 + ch.bearing[0] * m_scale, y2 + ch.bearing[1] * m_scale);
		addChar(pos, *c, vertices, indices);
		x2 += ch.advance[0] * m_scale;
		y2 += ch.advance[1] * m_scale;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices[0]);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(unsigned int), &indices[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glUseProgram(m_shaderText->m_program);
	m_shaderText->loadMatrix("u_transform", m_transform * Globals::projection);
	m_shaderText->loadVector("textColor", m_color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Globals::fontManager.get("font_90").spriteSheet);

	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);

	/*vertices.clear();
	vertices.shrink_to_fit();

	indices.clear();
	indices.shrink_to_fit();*/
}

void Text::render(std::string label, Vector4f color) {
	m_color = color;
	render(label);
}

void Text::setMaxChar(const size_t maxChar){
	m_maxChar = maxChar;

	short stride = 5, offset = 3;

	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ibo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_maxChar * 4 * stride * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_maxChar * 6 * sizeof(unsigned int), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Text::setPosition(const Vector2f &position) {
	m_position = position;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Text::setPosition(const float x, const float y) {
	m_position[0] = x;
	m_position[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Text::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

void Text::setOrigin(const float x, const float y) {
	m_origin[0] = x;
	m_origin[1] = y;
	m_transform.translate((m_position[0] - m_origin[0]), (m_position[1] - m_origin[1]), 0.0f);
}

const Vector2f &Text::getPosition() const {
	return m_position;
}

const Vector2f &Text::getSize() const {
	return m_size;
}

void Text::calcSize() {

	if (m_label.empty()) {
		m_size = Vector2f(0.0f, Globals::fontManager.get("font_90").lineHeight * m_scale);
	}else {

		int sizeX = 0, sizeY = 0;
		Character ch;

		std::string::const_iterator c;
		for (c = m_label.begin(); c != m_label.end(); c++) {
			ch = m_characters[*c];

			sizeX = sizeX + (ch.advance[0]);
			sizeY = std::max(sizeY, ch.size[1]);
		}		
		m_size = Vector2f(sizeX * m_scale, sizeY * m_scale);
	}
}

void Text::calcSize(std::string label) {
	int sizeX = 0, sizeY = 0;
	Character ch;

	std::string::const_iterator c;
	for (c = label.begin(); c != label.end(); c++) {
		ch = m_characters[*c];

		sizeX = sizeX + ((ch.advance[0]) * m_scale);
		sizeY = std::max(sizeY, ch.size[1]);
	}
	m_size = Vector2f(sizeX, sizeY * m_scale);
}

void Text::setColor(const Vector4f &color) {
	m_color = color;
}

std::string Text::floatToString(float val, int precision) {
	static const int bufSize = 100;
	static char buffer[bufSize];

	std::snprintf(buffer, bufSize, "%.*f", precision, val);

	return std::string(buffer);
}
