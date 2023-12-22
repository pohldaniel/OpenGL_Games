#include "PrimitiveFactory.h"
#include "Globals.h"

PrimitiveFactory::PrimitiveFactory() : m_shaderBasic("res/shaders/basic/basic.vert", "res/shaders/basic/basic.frag"){

}

cmpt::Primitive PrimitiveFactory::createRect(glm::vec4 color, float scaleX, float scaleY, float pivotX, float pivotY) {
    return cmpt::Primitive(color, &m_shaderBasic, &Globals::shapeManager.get("quad"), scaleX, scaleY, pivotX, pivotY);
}