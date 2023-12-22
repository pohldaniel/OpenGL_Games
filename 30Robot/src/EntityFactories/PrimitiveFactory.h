#pragma once

#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <engine/Shader.h>
#include <engine/MeshObject/Shape.h>
#include <Components/Components.h>

#include "Constants.h"

class PrimitiveFactory {

public:

    PrimitiveFactory();
    cmpt::Primitive createRect(glm::vec4 color, Shape* shape = nullptr, float scaleX = 1.0f, float scaleY = 1.0f, float pivotX = 0.0f, float pivotY = 0.0f);

private:
    Shader m_shaderBasic;

};
