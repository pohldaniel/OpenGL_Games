#include "Shape.h"
#include "Cube.h"
#include "Sphere.h"
#include "Torus.h"
#include "TorusKnot.h"
#include "Spiral.h"

Shape::Shape() : m_stride(0u) {

}

Shape::Shape(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int stride) : m_stride(stride){
	fromBuffer(vertexBuffer, indexBuffer, stride);
}

Shape::Shape(Shape const& rhs) {
	m_vertexBuffer = rhs.m_vertexBuffer;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
}

Shape::Shape(Shape&& rhs) noexcept {
	m_vertexBuffer = rhs.m_vertexBuffer;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
}

Shape& Shape::operator=(const Shape& rhs) {
	m_vertexBuffer = rhs.m_vertexBuffer;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
	return *this;
}

Shape& Shape::operator=(Shape&& rhs) noexcept {
	m_vertexBuffer = rhs.m_vertexBuffer;
	m_indexBuffer = rhs.m_indexBuffer;
	m_stride = rhs.m_stride;
	return *this;
}

Shape::~Shape() {

}

void Shape::fromBuffer(const std::vector<float>& vertexBuffer, const std::vector<unsigned int>& indexBuffer, unsigned int stride) {
	m_stride = stride;
	
	m_vertexBuffer.reserve(vertexBuffer.size());
	std::copy(vertexBuffer.begin(), vertexBuffer.end(), std::back_inserter(m_vertexBuffer));

	m_indexBuffer.reserve(indexBuffer.size());
	std::copy(indexBuffer.begin(), indexBuffer.end(), std::back_inserter(m_indexBuffer));
}

void Shape::buildCube(const std::array<float, 3>& position, const std::array<float, 3>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Cube::BuildMesh4Q(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildSphere(const std::array<float, 3>& position, float radius, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Sphere::BuildMesh(position, radius, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildTorus(const std::array<float, 3>& position, float radius, float tubeRadius, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Torus::BuildMesh(position, radius, tubeRadius, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildTorusKnot(const std::array<float, 3>& position, float radius, float tubeRadius, unsigned int p, unsigned int q, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	TorusKnot::BuildMesh(position, radius, tubeRadius, p, q, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildSpiral(const std::array<float, 3>& position, float radius, float tubeRadius, float length, unsigned int numRotations, bool repeatTexture, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Spiral::BuildMesh(position, radius, tubeRadius, length, numRotations, repeatTexture, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

const std::vector<float>& Shape::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& Shape::getIndexBuffer() const {
	return m_indexBuffer;
}