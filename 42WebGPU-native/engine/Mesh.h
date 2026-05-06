#pragma once
#include <vector>

class Mesh {

	friend class Model;
	friend class ObjModel;
	friend class AssimpModel;

public:

	Mesh();
	Mesh(Mesh const& rhs);
	Mesh(Mesh&& rhs) noexcept;
	Mesh& operator=(const Mesh& rhs);
	Mesh& operator=(Mesh&& rhs) noexcept;
	virtual ~Mesh() = default;

public:

	const std::vector<float>& getVertexBuffer() const;
	const std::vector<unsigned int>& getIndexBuffer() const;
	const unsigned int getStride() const;

protected:

	std::vector<unsigned int> m_indexBuffer;
	std::vector<float> m_vertexBuffer;

	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;
	unsigned int m_stride, m_drawCount, m_baseVertex, m_baseIndex;

};