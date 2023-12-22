#pragma once

#include <array>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <engine/Shader.h>
#include <engine/MeshObject/Shape.h>
#include <Components/Components.h>
#include "graphics/index-buffer.hpp"

#include "Constants.h"

enum class PivotPoint {
	CENTER,
	MIDDLE_LEFT
};

class PrimitiveFactory {

public:

    PrimitiveFactory();
    cmpt::Primitive createRect(glm::vec4 color, Shape* shape = nullptr);

private:
    Shader m_shaderBasic;

};
