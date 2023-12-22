#include "PrimitiveFactory.h"

#include "graphics/vertex-array.hpp"
#include "Globals.h"

PrimitiveFactory::PrimitiveFactory() : m_shaderBasic("res/shaders/basic/basic.vert", "res/shaders/basic/basic.frag"){

}

cmpt::Primitive PrimitiveFactory::createRect(glm::vec4 color, Shape* shape, float scaleX, float scaleY, float pivotX, float pivotY) {
    return cmpt::Primitive(color, &m_shaderBasic, shape, scaleX, scaleY, pivotX, pivotY);
}