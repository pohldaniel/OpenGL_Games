#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>
#include <engine/Rect.h>

struct SpriteComponent {
    std::string assetId;
    int width;
    int height;
    int zIndex;
    //SDL_RendererFlip flip;
    bool isFixed;
    Rect srcRect;
    
    SpriteComponent(std::string assetId = "", float width = 0.0f, float height = 0.0f, int zIndex = 0, bool isFixed = false, float srcRectX = 0.0f, float srcRectY = 0.0f) {
        this->assetId = assetId;
        this->width = width;
        this->height = height;
        this->zIndex = zIndex;
        //this->flip = SDL_FLIP_NONE;
        this->isFixed = isFixed;
        this->srcRect = {srcRectX, srcRectY, width, height};
    }
};

#endif
