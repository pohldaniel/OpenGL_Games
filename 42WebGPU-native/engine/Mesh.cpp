#include "Mesh.h"

Mesh::Mesh() : m_hasTextureCoords(false), 
               m_hasNormals(false), 
	           m_hasTangents(false),
               m_stride(0u),
               m_drawCount(0u),
               m_baseVertex(0u),
               m_baseIndex(0u) {

}

Mesh::Mesh(Mesh const& rhs) : m_hasTextureCoords(rhs.m_hasTextureCoords),
                              m_hasNormals(rhs.m_hasNormals),
                              m_hasTangents(rhs.m_hasTangents),
                              m_stride(rhs.m_stride),
                              m_drawCount(rhs.m_drawCount),
                              m_baseVertex(rhs.m_baseVertex),
                              m_baseIndex(rhs.m_baseIndex) {

}

Mesh::Mesh(Mesh&& rhs) noexcept : m_hasTextureCoords(rhs.m_hasTextureCoords),
                                  m_hasNormals(rhs.m_hasNormals),
                                  m_hasTangents(rhs.m_hasTangents),
                                  m_stride(rhs.m_stride),
                                  m_drawCount(rhs.m_drawCount),
                                  m_baseVertex(rhs.m_baseVertex),
                                  m_baseIndex(rhs.m_baseIndex) {

}

Mesh& Mesh::operator=(const Mesh& rhs) {
    m_hasTextureCoords = rhs.m_hasTextureCoords;
    m_hasNormals = rhs.m_hasNormals;
    m_hasTangents = rhs.m_hasTangents;
    m_stride = rhs.m_stride;
    m_drawCount = rhs.m_drawCount;
    m_baseVertex = rhs.m_baseVertex;
    m_baseIndex = rhs.m_baseIndex;
    return *this;
}

Mesh& Mesh::operator=(Mesh&& rhs) noexcept {
    m_hasTextureCoords = rhs.m_hasTextureCoords;
    m_hasNormals = rhs.m_hasNormals;
    m_hasTangents = rhs.m_hasTangents;
    m_stride = rhs.m_stride;
    m_drawCount = rhs.m_drawCount;
    m_baseVertex = rhs.m_baseVertex;
    m_baseIndex = rhs.m_baseIndex;
    return *this;
}

const std::vector<float>& Mesh::getVertexBuffer() const {
	return m_vertexBuffer;
}

const std::vector<unsigned int>& Mesh::getIndexBuffer() const {
	return m_indexBuffer;
}

const unsigned int Mesh::getStride() const {
	return m_stride;
}