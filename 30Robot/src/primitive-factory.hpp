#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <array>

#include "graphics/index-buffer.hpp"
#include "graphics/shaderRo.hpp"
#include "components/primitive.hpp"
#include "constants.hpp"

enum class PivotPoint {
	CENTER,
	MIDDLE_LEFT
};

class PrimitiveFactory {
public:
    PrimitiveFactory();

    cmpt::Primitive createRect(glm::vec4 color, glm::vec2 displaySize, PivotPoint pivot = PivotPoint::CENTER);
    cmpt::Primitive createRectOutline(glm::vec4 color, glm::vec2 displaySize, PivotPoint pivot = PivotPoint::CENTER);

    // TODO circle and line

private:
	std::array<float, 8> getVertexPositions(glm::vec2 displaySize, PivotPoint pivot);

private:
    ShaderRo m_shaderBasic;
};
