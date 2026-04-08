#pragma once

#include <vector>
#include <iterator>

enum ModelColor {
	MC_WHITE,
	MC_RED,
	MC_GREEN,
	MC_BLUE,
	MC_BLACK,
	MC_POSITION
};

class Model {
	
	virtual const unsigned int getStride() const = 0;

protected:

	void static GenerateColors(std::vector<float>& vertexBuffer, std::vector<unsigned int>& indexBuffer, unsigned int& stride, unsigned int startIndex, unsigned int endIndex, ModelColor modelColor);
	void static PackBuffer(std::vector<float>& vertexBuffer, unsigned int stride);
};