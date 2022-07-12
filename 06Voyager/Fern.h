#pragma once

#include "RenderableObject.h"

class Fern : public RenderableObject {
public:
	Fern();
	~Fern();

	void render(const Camera& camera) override;
	void renderShadow(const Camera& camera) override;

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	unsigned int m_vao;
	unsigned int m_drawCount;

	ModelMatrix m_modelMatrix;
};