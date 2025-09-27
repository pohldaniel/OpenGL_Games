#pragma once

#include <vector>

#include "Material.hpp"


class Materials {
public:
    static int nMaterials;
    static std::vector<MaterialFall*> MATERIALS;
    static MaterialFall** MATERIALS_ARRAY;

    static MaterialFall GENERIC_AIR;
    static MaterialFall GENERIC_SOLID;
    static MaterialFall GENERIC_SAND;
    static MaterialFall GENERIC_LIQUID;
    static MaterialFall GENERIC_GAS;
    static MaterialFall GENERIC_PASSABLE;
    static MaterialFall GENERIC_OBJECT;
    static MaterialFall TEST_SAND;
    static MaterialFall TEST_TEXTURED_SAND;
    static MaterialFall TEST_LIQUID;
    static MaterialFall STONE;
    static MaterialFall GRASS;
    static MaterialFall DIRT;
    static MaterialFall SMOOTH_STONE;
    static MaterialFall COBBLE_STONE;
    static MaterialFall SMOOTH_DIRT;
    static MaterialFall COBBLE_DIRT;
    static MaterialFall SOFT_DIRT;
    static MaterialFall WATER;
    static MaterialFall LAVA;
    static MaterialFall CLOUD;
    static MaterialFall GOLD_ORE;
    static MaterialFall GOLD_MOLTEN;
    static MaterialFall GOLD_SOLID;
    static MaterialFall IRON_ORE;
    static MaterialFall OBSIDIAN;
    static MaterialFall STEAM;
    static MaterialFall SOFT_DIRT_SAND;
    static MaterialFall FIRE;
    static MaterialFall FLAT_COBBLE_STONE;
    static MaterialFall FLAT_COBBLE_DIRT;

    static void init();

};
