#ifndef TEXTLABELCOMPONENT_H
#define TEXTLABELCOMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include <engine/Vector.h>

struct TextLabelComponent {
    glm::vec2 position;
    std::string text;
    std::string assetId;
	Vector4f color;
    bool isFixed;

    TextLabelComponent(glm::vec2 position = glm::vec2(0), const std::string& text = "", const std::string& assetId = "", const Vector4f& color = {0.0f, 0.0f, 0.0f, 0.0f}, bool isFixed = true) {
        this->position = position;
        this->text = text;
        this->assetId = assetId;
        this->color = color;
        this->isFixed = isFixed;
    }
};

#endif