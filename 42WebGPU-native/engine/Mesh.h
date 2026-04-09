#pragma once
#include <vector>

class Mesh {

public:

	virtual ~Mesh() = default;

protected:
	virtual const std::vector<float>& getVertexBuffer() const = 0;
	virtual const std::vector<unsigned int>& getIndexBuffer() const = 0;
	virtual const unsigned int getStride() const = 0;
};