#include <GL/glew.h>
#include "BoundingBox.h"

BoundingBox::BoundingBox() : min(Vector3f(FLT_MAX, FLT_MAX, FLT_MAX)), max(Vector3f(-FLT_MAX, -FLT_MAX, -FLT_MAX)) {

}

BoundingBox::BoundingBox(BoundingBox const& rhs) : min(rhs.min), max(rhs.max){
}

BoundingBox::BoundingBox(BoundingBox&& rhs) : min(rhs.min), max(rhs.max){

}

BoundingBox& BoundingBox::operator=(const BoundingBox& rhs) {
	min = rhs.min;
	max = rhs.max;
	return *this;
}

BoundingBox::BoundingBox(const Vector3f& min, const Vector3f& max) : min(min), max(max){

}

BoundingBox::BoundingBox(float min, float max) : min(Vector3f(min)), max(Vector3f(max)){

}

Vector3f BoundingBox::getSize() const {
	return max - min;
}

Vector3f BoundingBox::center() const {
	return (max + min) * 0.5f;
}

void BoundingBox::merge(const Vector3f* vertices, size_t count) {
	while (count--)
		merge(*vertices++);
}

void BoundingBox::merge(const Vector3f& point) {

	if (point[0] < min[0])
		min[0] = point[0];
	if (point[1] < min[1])
		min[1] = point[1];
	if (point[2] < min[2])
		min[2] = point[2];
	if (point[0] > max[0])
		max[0] = point[0];
	if (point[1] > max[1])
		max[1] = point[1];
	if (point[2] > max[2])
		max[2] = point[2];
}

void BoundingBox::define(const Vector3f& point) {
	min = max = point;
}

void BoundingBox::undefine() {
	min = Vector3f(FLT_MAX, FLT_MAX, FLT_MAX);
	max = -min;
}

void BoundingBox::setMin(const Vector3f& _min) const {
	BoundingBox* ptr = const_cast<BoundingBox*>(this);
	ptr->min = _min;
}

void BoundingBox::setMax(const Vector3f& _max) const {
	BoundingBox* ptr = const_cast<BoundingBox*>(this);
	ptr->max = _max;
}

void BoundingBox::inset(const Vector3f& _min, const Vector3f& _max) const {
	BoundingBox* ptr = const_cast<BoundingBox*>(this);
	ptr->min += _min;
	ptr->max -= _max;
}

BoundingBox BoundingBox::transformed(const Matrix4f& transform) const {
	Vector3f oldCenter = center();
	Vector3f oldEdge = max - oldCenter;
	Vector3f newCenter = transform ^ oldCenter;
	Vector3f newEdge(
		std::fabs(transform[0][0]) * oldEdge[0] + std::fabs(transform[1][0]) * oldEdge[1] + std::fabs(transform[2][0]) * oldEdge[2],
		std::fabs(transform[0][1]) * oldEdge[0] + std::fabs(transform[1][1]) * oldEdge[1] + std::fabs(transform[2][1]) * oldEdge[2],
		std::fabs(transform[0][2]) * oldEdge[0] + std::fabs(transform[1][2]) * oldEdge[1] + std::fabs(transform[2][2]) * oldEdge[2]
	);
	return BoundingBox(newCenter - newEdge, newCenter + newEdge);
}

BoundingBox::~BoundingBox() {
	cleanup();
}

void BoundingBox::createBuffer() {
	std::vector<float> vertexBuffer;
	std::vector<unsigned int> indexBuffer;

	Vector3f size = getSize();

	vertexBuffer.push_back(min[0]);			  vertexBuffer.push_back(min[1]);			vertexBuffer.push_back(min[2]);
	vertexBuffer.push_back(min[0] + size[0]); vertexBuffer.push_back(min[1]);			vertexBuffer.push_back(min[2]);
	vertexBuffer.push_back(min[0] + size[0]); vertexBuffer.push_back(min[1] + size[1]); vertexBuffer.push_back(min[2]);
	vertexBuffer.push_back(min[0]);			  vertexBuffer.push_back(min[1] + size[1]); vertexBuffer.push_back(min[2]);
	vertexBuffer.push_back(min[0]);			  vertexBuffer.push_back(min[1]);			vertexBuffer.push_back(min[2] + size[2]);
	vertexBuffer.push_back(min[0] + size[0]); vertexBuffer.push_back(min[1]);			vertexBuffer.push_back(min[2] + size[2]);
	vertexBuffer.push_back(min[0] + size[0]); vertexBuffer.push_back(min[1] + size[1]); vertexBuffer.push_back(min[2] + size[2]);
	vertexBuffer.push_back(min[0]);			  vertexBuffer.push_back(min[1] + size[1]); vertexBuffer.push_back(min[2] + size[2]);

	indexBuffer.push_back(1); indexBuffer.push_back(0); indexBuffer.push_back(2);
	indexBuffer.push_back(3); indexBuffer.push_back(2); indexBuffer.push_back(0);

	indexBuffer.push_back(5); indexBuffer.push_back(1); indexBuffer.push_back(6);
	indexBuffer.push_back(2); indexBuffer.push_back(6); indexBuffer.push_back(1);

	indexBuffer.push_back(6); indexBuffer.push_back(7); indexBuffer.push_back(5);
	indexBuffer.push_back(4); indexBuffer.push_back(5); indexBuffer.push_back(7);

	indexBuffer.push_back(0); indexBuffer.push_back(4); indexBuffer.push_back(3);
	indexBuffer.push_back(7); indexBuffer.push_back(3); indexBuffer.push_back(4);

	indexBuffer.push_back(5); indexBuffer.push_back(4); indexBuffer.push_back(1);
	indexBuffer.push_back(0); indexBuffer.push_back(1); indexBuffer.push_back(4);

	indexBuffer.push_back(2); indexBuffer.push_back(3); indexBuffer.push_back(6);
	indexBuffer.push_back(7); indexBuffer.push_back(6); indexBuffer.push_back(3);

	short stride = 3; short offset = 0;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertexBuffer.size() * sizeof(float), &vertexBuffer[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexBuffer.size() * sizeof(unsigned int), &indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void BoundingBox::drawRaw() const {
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void BoundingBox::cleanup() {
	if (m_vao) {
		glDeleteVertexArrays(1, &m_vao);
		m_vao = 0;
	}

	if (m_vbo) {
		glDeleteBuffers(1, &m_vbo);
		m_vbo = 0;
	}

	if (m_ibo) {
		glDeleteBuffers(1, &m_ibo);
		m_ibo = 0;
	}
}