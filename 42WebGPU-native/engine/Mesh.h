#pragma once
#include <vector>

class Mesh {
	virtual const std::vector<float>& getVertexBuffer() const = 0;
	virtual const std::vector<unsigned int>& getIndexBuffer() const = 0;
	virtual unsigned int getStride() = 0;
};