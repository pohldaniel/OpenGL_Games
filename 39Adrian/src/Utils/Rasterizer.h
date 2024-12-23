#pragma once

#include <vector>
#include <array>

namespace Rasterizer {

	void supercover_line(int x, int y, int x2, int y2, std::vector<std::array<int, 2>>& points);
	void linev5(int x1, int y1, int x2, int y2, std::vector<std::array<int, 2>>& points);
	void triangle(std::array<int, 2> t0, std::array<int, 2> t1, std::array<int, 2> t2, std::vector<std::array<int, 2>>& points);
	void fillFlatSideTriangleInt(std::array<int, 2>  v1, std::array<int, 2> v2, std::array<int, 2>  v3, std::vector<std::array<int, 2>>& points);
	void rasterTriangle(std::vector<std::array<int, 2>>& initial, std::vector<std::array<int, 2>>& points);
}