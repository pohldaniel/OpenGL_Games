#include "Shape.h"
#include "Cube.h"
#include "Sphere.h"
#include "Torus.h"
#include "TorusKnot.h"
#include "Spiral.h"
#include "Quad.h"
#include "Cylinder.h"
#include "Capsule.h"
#include "Segment.h"

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

void Shape::buildSphere(const std::array<float, 3>& position, float radius, float randomness, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Sphere::BuildMesh(position, radius, randomness, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
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

void Shape::buildQuadXY(const  std::array<float, 3>& position, const std::array<float, 2>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Quad::BuildMeshXY(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}
void Shape::buildQuadXZ(const  std::array<float, 3>& position, const std::array<float, 2>& size, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Quad::BuildMeshXZ(position, size, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildCylinder(const std::array<float, 3>& position, float baseRadius, float topRadius, float length, bool bottom, bool top, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Cylinder::BuildMesh(position, baseRadius, topRadius, length, bottom, top, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildCapsule(const std::array<float, 3>& position, float radius, float length, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Capsule::BuildMesh(position, radius, length, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildSegmentXY(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Segment::BuildMeshXY(position, radius, startAngle, endAngle, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

void Shape::buildSegmentXZ(const std::array<float, 3>& position, float radius, float startAngle, float endAngle, unsigned int uResolution, unsigned int vResolution, bool generateTexels, bool generateNormals, bool generateTangents) {
	Segment::BuildMeshXZ(position, radius, startAngle, endAngle, uResolution, vResolution, generateTexels, generateNormals, generateTangents, m_vertexBuffer, m_indexBuffer);
	m_stride = 3u + 2u * generateTexels + 3u * generateNormals + 6u * generateTangents;
}

const std::vector<float>& Shape::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& Shape::getIndexBuffer() const {
	return m_indexBuffer;
}

const unsigned int Shape::getStride() const {
	return m_stride;
}

void Shape::rewind() {
	Shape::Rewind(m_indexBuffer);
}

void Shape::flatShading() {
	if (m_stride < 6u)
		return;

	std::vector<unsigned int> indexBuffer;
	std::vector<float> vertexBuffer;

	size_t elements = m_indexBuffer.size();
	vertexBuffer.resize(elements * m_stride);
	indexBuffer.resize(elements);

	for (size_t i = 0; i < elements; ++i) {
		size_t src_off = m_indexBuffer[i] * m_stride;
		size_t dst_off = i * m_stride;
		for (uint32_t j = 0; j < m_stride; ++j) {
			vertexBuffer[dst_off + j] = m_vertexBuffer[src_off + j];
		}
		indexBuffer[i] = static_cast<unsigned int>(i);
	}

	for (size_t i = 0; i < vertexBuffer.size() / m_stride; i = i + 3) {
		float* v0 = &vertexBuffer[i * m_stride];
		float* v1 = &vertexBuffer[(i + 1) * m_stride];
		float* v2 = &vertexBuffer[(i + 2) * m_stride];

		std::array<float, 3> e0 = Normalize({ v0[0] - v1[0], v0[1] - v1[1], v0[2] - v1[2] });
		std::array<float, 3> e1 = Normalize({ v0[0] - v2[0], v0[1] - v2[1], v0[2] - v2[2] });
		std::array<float, 3> n = Cross(e0, e1);
		
		size_t offset = m_stride == 6u ? 3u : m_stride >= 8u ? 5u : 0u;

		v0[offset    ] = n[0];
		v0[offset + 1] = n[1];
		v0[offset + 2] = n[2];
		v1[offset    ] = n[0];
		v1[offset + 1] = n[1];
		v1[offset + 2] = n[2];
		v2[offset    ] = n[0];
		v2[offset + 1] = n[1];
		v2[offset + 2] = n[2];
	}

	m_vertexBuffer.clear();
	m_vertexBuffer.shrink_to_fit();
	copy(vertexBuffer.begin(), vertexBuffer.end(), back_inserter(m_vertexBuffer));

	m_indexBuffer.clear();
	m_indexBuffer.shrink_to_fit();
	copy(indexBuffer.begin(), indexBuffer.end(), back_inserter(m_indexBuffer));
}

void Shape::Rewind(std::vector<unsigned int>& indexBuffer) {
	for (unsigned int i = 0; i < indexBuffer.size(); i = i + 3) {
		unsigned int index2 = indexBuffer[i + 1];
		indexBuffer[i + 1] = indexBuffer[i + 2];
		indexBuffer[i + 2] = index2;
	}
}

std::array<float, 3> Shape::Normalize(const std::array<float, 3>& v) {
	float length = sqrtf((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
	float invMag = length != 0.0f ? 1.0f / length : 1.0f;
	return { v[0] * invMag, v[1] * invMag, v[2] * invMag };
}

std::array<float, 3> Shape::Cross(const std::array<float, 3>& p, const std::array<float, 3>& q) {
	return { (p[1] * q[2]) - (p[2] * q[1]),
			 (p[2] * q[0]) - (p[0] * q[2]),
			 (p[0] * q[1]) - (p[1] * q[0]) };
}