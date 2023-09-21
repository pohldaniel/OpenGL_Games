#include "Material.hpp"

Material::Material(int id, std::string name, int physicsType, int slipperyness, uint8_t alpha, float density, int iterations, int emit, uint32_t emitColor, uint32_t color) {
    this->name = name;
    this->id = id;
    this->physicsType = physicsType;
    this->slipperyness = slipperyness;
    this->alpha = alpha;
    this->density = density;
    this->iterations = iterations;
    this->emit = emit;
    this->emitColor = emitColor;
    this->color = color;
}
