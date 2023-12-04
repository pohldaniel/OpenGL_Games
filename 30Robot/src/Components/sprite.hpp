#pragma once

#include <GL/glew.h>
#include "graphics/shaderRo.hpp"
#include "graphics/index-buffer.hpp"

namespace cmpt {
struct Sprite {
    Sprite(unsigned int textureID = 0, unsigned int vaID = 0, GLenum target = 0, ShaderRo* shader = nullptr, IndexBuffer* ib = nullptr)
    : textureID(textureID), vaID(vaID), target(target), shader(shader), ib(ib) {}

    unsigned int textureID;
    unsigned int vaID;
    GLenum target;
    ShaderRo* shader;
    IndexBuffer* ib;
};
}
