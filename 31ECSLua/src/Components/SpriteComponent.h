#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>
#include <vector>
#include <engine/Rect.h>

struct SpriteComponent {
    std::string assetId;
    int zIndex;
	bool isFixed;
	TextureRect textureRect;
	std::vector<TextureRect> animationRects;
    
	SpriteComponent(std::string assetId = "", const TextureRect& rect = DefaultRect, int zIndex = 0, bool isFixed = false) {
        this->assetId = assetId;
        this->zIndex = zIndex;
		this->isFixed = isFixed;
		this->textureRect = rect;
		
    }

	void SpriteComponent::init(std::vector<TextureRect>& textureRects) {
		this->animationRects = textureRects;
	}

	inline static TextureRect DefaultRect = {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0u};
};

#endif
