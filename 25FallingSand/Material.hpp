#pragma once

#include <stdint.h>
#include <string>
#include <unordered_map>

#include "PhysicsType.hpp"

#define INTERACT_NONE 0
#define INTERACT_TRANSFORM_MATERIAL 1 // id, radius
#define INTERACT_SPAWN_MATERIAL 2 // id, radius
#define EXPLODE 3 // radius

#define REACT_TEMPERATURE_BELOW 4 // temperature, id
#define REACT_TEMPERATURE_ABOVE 5 // temperature, id

struct MaterialInteraction {

	

    int type;
    int data1;
    int data2;
    int ofsX;
    int ofsY;

	//MaterialInteraction(int type = INTERACT_NONE, int data1 = 0, int data2 = 0, int ofsX = 0, int ofsY = 0) :type(type), data1(data1), data2(data2), ofsX(ofsX), ofsY(ofsY) {}
};

class MaterialFall {
public:
    std::string name;
    int id = 0;
    int physicsType = 0;
	uint8_t alpha = 0;
    float density = 0;
    int iterations = 0;
    int emit = 0;
	uint32_t emitColor = 0;
	uint32_t color = 0;
	uint32_t addTemp = 0;
    float conductionSelf = 1.0;
    float conductionOther = 1.0;

    bool interact = false;
    int* nInteractions = nullptr;
    std::vector<MaterialInteraction>* interactions = nullptr;
    bool react = false;
    int nReactions = 0;
    std::vector<MaterialInteraction> reactions;

    int slipperyness = 1;

    MaterialFall(int id, std::string name, int physicsType, int slipperyness, uint8_t alpha, float density, int iterations, int emit, uint32_t emitColor, uint32_t color);
    MaterialFall(int id, std::string name, int physicsType, int slipperyness, uint8_t alpha, float density, int iterations, int emit, uint32_t emitColor);
    MaterialFall(int id, std::string name, int physicsType, int slipperyness, uint8_t alpha, float density, int iterations);
    MaterialFall(int id, std::string name, int physicsType, int slipperyness, float density, int iterations);
    MaterialFall();

};
