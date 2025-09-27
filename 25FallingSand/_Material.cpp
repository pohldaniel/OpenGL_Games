#include "Material.hpp"

MaterialFall::MaterialFall(int id, std::string name, int physicsType, int slipperyness, uint8_t alpha, float density, int iterations, int emit, uint32_t emitColor, uint32_t color) {
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

MaterialFall::MaterialFall(int id, std::string name, int physicsType, int slipperyness, uint8_t alpha, float density, int iterations, int emit, uint32_t emitColor) : MaterialFall(id, name, physicsType, slipperyness, alpha, density, iterations, emit, emitColor, 0xffffffff) {
}

MaterialFall::MaterialFall(int id, std::string name, int physicsType, int slipperyness, uint8_t alpha, float density, int iterations) : MaterialFall(id, name, physicsType, slipperyness, alpha, density, iterations, 0, 0) {
}

MaterialFall::MaterialFall(int id, std::string name, int physicsType, int slipperyness, float density, int iterations) : MaterialFall(id, name, physicsType, slipperyness, 0xff, density, iterations) {
}

MaterialFall::MaterialFall() : MaterialFall(0, "Air", PhysicsType::AIR, 4, 0, 0) {
}