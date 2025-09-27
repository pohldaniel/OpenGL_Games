#pragma once

#include <SDL.h>
#include "Material.hpp"

class MaterialInstance;

class Structure {
public:
    MaterialInstance* tiles;
    int w;
    int h;

    Structure(int w, int h, MaterialInstance* tiles);
    Structure(SDL_Surface* texture, MaterialFall templ);
    Structure() = default;
};
