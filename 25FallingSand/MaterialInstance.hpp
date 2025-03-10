#pragma once

#include "Materials.hpp"

class MaterialInstance {

public:

	MaterialInstance();
	MaterialInstance(Material* mat, uint32_t color);
    MaterialInstance(Material* mat, uint32_t color, int32_t temperature);
	
    bool operator==(const MaterialInstance& other);

	Material* mat;
	uint32_t color;
	int32_t temperature;
	uint32_t id = 0;
	bool moved = false;
	float fluidAmount = 2.0f;
	float fluidAmountDiff = 0.0f;
	uint8_t settleCount = 0;

	static int _curID;
};
