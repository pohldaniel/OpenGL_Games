#include <GL/glew.h>

#include "Mesh.h"

Mesh::Mesh(){

	m_vertexSize = 0;
	m_verticesCount = 0;
	m_indicesCount = 0;

	m_vertices = nullptr;
	m_indices = nullptr;

	m_vertexType = VertexType2::_Simple;
	m_vertexPrimitive = VertexPrimitive2::_Triangle;
	m_vertexBufferDrawType = VertexBufferDrawType2::_StaticDraw;

	m_drawMode = GL_TRIANGLES;
	m_vao = 0;
	m_vbo = 0;
	m_ibo = 0;
}

Mesh::~Mesh(){

}

void Mesh::setVertexPrimitive(VertexPrimitive2 vertexPrimitive) {


	switch (vertexPrimitive){
	case _Triangle:
		m_drawMode = GL_TRIANGLES;
		break;
	case _TriangleFan:
		m_drawMode = GL_TRIANGLE_FAN;
		break;
	case _TriangleStrip:
		m_drawMode = GL_TRIANGLE_STRIP;
		break;
	case _Lines:
		m_drawMode = GL_LINES;
		break;
	}
}

void Mesh::setVertexType(VertexType2 vertexType) {
	m_vertexType = vertexType;
	m_vertexSize = getVertexSize(vertexType);
}

void Mesh::setVertexBufferDrawType(VertexBufferDrawType2 drawType) {
	m_vertexBufferDrawType = drawType;
}

void Mesh::createVertices(int count) {
	m_verticesCount = count;
	m_vertices = new float[m_verticesCount * (m_vertexSize / 4)];
}

void Mesh::setVertices(void* data, int count) {
	m_vertices = data;
	m_verticesCount = count;
}

void Mesh::updateVertices(void* data, int count, bool deleteData) {
	if (m_vbo != 0){
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexSize * count, data);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		if (deleteData)
			delete[] data;
	}
}

void* Mesh::getVertices() {
	return m_vertices;
}

void Mesh::createIndices(int count) {
	m_indicesCount = count;
	m_indices = new unsigned short[count];
}

void Mesh::setIndicesNumber(int count) {
	m_indicesCount = count;
}

void Mesh::setIndices(void* data, int count) {
	m_indices = data;
	m_indicesCount = count;
}

void Mesh::updateIndices(void* data, int count) {
	if (m_ibo != 0){
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(unsigned short) * count, data);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		delete[] data;
	}
}

void* Mesh::getIndices() {
	return m_indices;
}

void Mesh::setVertexArtibutes() {
	switch (m_vertexType) {
		case _Simple: {
			//position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)0);
			glEnableVertexAttribArray(0);
		}
					 break;
		case _Color:
		{
			//position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
		}
		break;
		case _Textured:
		{
			//position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// TexCoord attribute
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);
		}
		break;
		case _TextureNormal:
		{
			//position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)0);
			glEnableVertexAttribArray(0);

			//normal attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			// TexCoord attribute
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);

		}
		break;
		case _TextureColor:
		{
			//position attribute
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)0);
			glEnableVertexAttribArray(0);

			// Color attribute
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(3 * sizeof(GLfloat)));
			glEnableVertexAttribArray(1);

			// TexCoord attribute
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, m_vertexSize, (GLvoid*)(6 * sizeof(GLfloat)));
			glEnableVertexAttribArray(2);
		}
		break;
		case _ColorNormal: break;
		case _TextureColorNormal: break;
		default: break;
	}
}

void Mesh::createBuffer(bool removeData) {

	//generate vertex buffer object
	glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	if (m_vertexBufferDrawType == _DynamicDraw){
		glBufferData(GL_ARRAY_BUFFER, m_vertexSize * m_verticesCount, m_vertices, GL_DYNAMIC_DRAW);

	}else if (m_vertexBufferDrawType == _StaticDraw){
		glBufferData(GL_ARRAY_BUFFER, m_vertexSize * m_verticesCount, m_vertices, GL_STATIC_DRAW);

	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	if (removeData){
		//it's safe to delete vertices info
		delete[] m_vertices;
	}

	//generate indices buffer
	glGenBuffers(1, &m_ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	if (m_vertexBufferDrawType == _DynamicDraw){
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_indicesCount, m_indices, GL_DYNAMIC_DRAW);
	}else if (m_vertexBufferDrawType == _StaticDraw){
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * m_indicesCount, m_indices, GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (removeData){
		//it's safe to delete indices info
		delete[] m_indices;
	}

	//generate vertex array object
	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);

	//set basic atribute
	setVertexArtibutes();

	glBindVertexArray(0);
}

void Mesh::drawRaw() {
	glBindVertexArray(m_vao);
	glDrawElements(m_drawMode, m_indicesCount, GL_UNSIGNED_SHORT, 0);
	glBindVertexArray(0);
}

unsigned int Mesh::getVertexSize(VertexType2 vertexType) {
	switch (vertexType){
		case _Simple:
			return 12;
			break;
		case _Color:
			return 24;
			break;
		case _Textured:
			return 20;
			break;
		case _TextureNormal:
			return 32;
			break;
		case _TextureColor:
			return 32;
			break;
		case _ColorNormal:
			return 40;
			break;
		case _TextureColorNormal:
			return 48;
			break;
		default: return 0;
	}

	return 0;
}