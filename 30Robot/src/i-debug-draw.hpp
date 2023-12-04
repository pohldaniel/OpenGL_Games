#pragma once

#include <Box2D/Box2D.h>
#include <glm/glm.hpp>

enum class BasicShaderType {
	BASIC,
	LASER
};

class IDebugDraw : public b2Draw {
public:
	/* Processing-Like API */
	virtual void triangle(float x1, float y1, float x2, float y2, float x3, float y3) = 0;
	virtual void rect(float x1, float y1, float x2, float y2,int zIndex) = 0;
	virtual void square(float x, float y, float extent) = 0;
	virtual void ellipse(float a, float b, float c, float d) = 0;
	virtual void circleWithGlow(float x, float y, float r) = 0;
	virtual void circleExplosion(float x, float y, float r, float maxR) = 0;
	virtual void quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4) = 0;
	virtual void line(float x1, float y1, float x2, float y2, BasicShaderType shaderType = BasicShaderType::BASIC) = 0;
	virtual void point(float x, float y) = 0;
	virtual void shape(glm::vec2* vertices) = 0;

	/* Setters */
	virtual void setProjMat(glm::mat4 mat) = 0;
	virtual void setViewMat(glm::mat4 mat) = 0;
	virtual void setColor(glm::vec4 color) = 0;
	virtual void setColor(float r, float g, float b, float a) = 0;
};
