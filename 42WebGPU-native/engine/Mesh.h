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

	mutable std::vector<unsigned int> m_indexBuffer;
	mutable std::vector<float> m_vertexBuffer;
	mutable unsigned int m_stride;

	unsigned int m_drawCount, m_baseVertex, m_baseIndex;
	bool m_hasTextureCoords, m_hasNormals, m_hasTangents;

	
};