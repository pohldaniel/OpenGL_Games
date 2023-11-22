#pragma once

#include <vector>

std::vector<std::array<int,2>> aStar(const std::array<int, 2> &start,
	const std::array<int,2> &end,
	int width,
	int height,
	int granularity = 10,
	int maxIterations = 1000);