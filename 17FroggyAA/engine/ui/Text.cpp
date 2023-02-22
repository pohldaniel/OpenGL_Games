#include <cstring>
#include "Text.h"

std::unique_ptr<Shader> Text::s_shaderText = nullptr;
unsigned short Text::s_total = 0;

Text::Text(CharacterSet &charset) : m_charset(charset) {
	s_total++;

	if (!s_shaderText) {
		s_shaderText = std::unique_ptr<Shader>(new Shader(TEXT_VERTEX, TEXT_FRGAMENT, false));
	}
	m_charset = charset;
	m_characters = charset.characters;
}

Text::Text(std::string label, CharacterSet &charset, float scale) : Text(charset) {
	setLabel(label, scale);
}

Text::Text(size_t maxChar, CharacterSet &charset, float scale) : Text(charset) {
	setMaxChar(m_maxChar, scale);
}

Text::Text(Text const& rhs) : Text(rhs.m_charset) {

	m_label = rhs.m_label;
	m_scale = rhs.m_scale;
	m_maxChar = rhs.m_maxChar;

	m_transform = rhs.m_transform;
	m_position = rhs.m_position;
	m_size = rhs.m_size;
	m_origin = rhs.m_origin;
	m_color = rhs.m_color;

	//the call of the destructor will invalid this members
	//m_vao = rhs.m_vao;
	//m_vbo = rhs.m_vbo;
	//m_ibo = rhs.m_ibo;
	//m_indexCount = rhs.m_indexCount;
	if (!m_label.empty())
		setLabel(m_label, m_scale);
	else
		setMaxChar(m_maxChar, m_scale);
}

Text& Text::operator=(const Text& rhs) {

	m_label = rhs.m_label;
	m_scale = rhs.m_scale;
	m_maxChar = rhs.m_maxChar;

	m_transform = rhs.m_transform;
	m_position = rhs.m_position;
	m_size = rhs.m_size;
	m_origin = rhs.m_origin;
	m_color = rhs.m_color;
	m_charset = rhs.m_charset;
	
	if (!m_label.empty())
		setLabel(m_label, m_scale);
	else
		setMaxChar(m_maxChar, m_scale);

	return *this;
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
	
	s_total--;
	/*if (s_total == 0) {
		s_shaderText.reset();
	}*/
}

void Text::setLabel(std::string label, float scale) {

	m_label = label;
	m_scale = scale;

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
	calcSize();
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
	
	glUseProgram(s_shaderText->m_program);
	s_shaderText->loadMatrix("u_transform", m_transform * Globals::projection);
	s_shaderText->loadVector("textColor", m_color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_charset.spriteSheet);
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
	

	glUseProgram(s_shaderText->m_program);
	s_shaderText->loadMatrix("u_transform", Globals::projection);
	s_shaderText->loadVector("textColor", m_color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_charset.spriteSheet);

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


	glUseProgram(s_shaderText->m_program);
	s_shaderText->loadMatrix("u_transform", m_transform * Globals::projection);
	s_shaderText->loadVector("textColor", m_color);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_charset.spriteSheet);

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

void Text::setMaxChar(const size_t maxChar, float scale){
	m_scale = scale;
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

	calcSize();
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
		m_size = Vector2f(0.0f, m_charset.lineHeight * m_scale);
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

std::string Text::FloatToString(float val, int precision) {
	static const int bufSize = 100;
	static char buffer[bufSize];

	std::snprintf(buffer, bufSize, "%.*f", precision, val);

	return std::string(buffer);
}
