#include "MaterialInstance.hpp"

int MaterialInstance::_curID = 1;

MaterialInstance::MaterialInstance() : MaterialInstance(&Materials::GENERIC_AIR, 0x000000, 0) {

}

MaterialInstance::MaterialInstance(MaterialFall* mat, uint32_t color) : MaterialInstance(mat, color, 0) {

}

MaterialInstance::MaterialInstance(MaterialFall* mat, uint32_t color, int32_t temperature) {
    this->id = _curID++;
    this->mat = mat;
    this->color = color;
    this->temperature = temperature;
}

bool MaterialInstance::operator==(const MaterialInstance & other) {
    return this->id == other.id;
}
