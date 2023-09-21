#pragma once

#include "Materials.hpp"

class MaterialInstance {
public:
    static int _curID;

    Material* mat;
    uint8_t color;
    int32_t temperature;
    uint32_t id = 0;
    bool moved = false;
    float fluidAmount = 2.0f;
    float fluidAmountDiff = 0.0f;
    uint8_t settleCount = 0;

    MaterialInstance(Material* mat, uint32_t color, int32_t temperature);
    MaterialInstance(Material* mat, uint32_t color) : MaterialInstance(mat, color, 0) {};
    MaterialInstance() : MaterialInstance(&Materials::GENERIC_AIR, 0x000000, 0) {};
    bool operator==(const MaterialInstance& other);
};
