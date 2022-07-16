#pragma once

#include "RenderableObject.h"

class Barrel : public RenderableObject {
public:
	Barrel(const float& dt, const float& fdt);
	~Barrel();

	void render(const Camera& camera) override;
	void renderShadow(const Camera& camera) override;
	void update();
	void toggleLightRotation();

	void rotate(const Vector3f &axis, float degrees);
	void translate(float dx, float dy, float dz);
	void scale(float a, float b, float c);
	const Matrix4f &getTransformationMatrix() const;
	const Matrix4f &getInvTransformationMatrix() const;

	unsigned int m_vao;
	unsigned int m_drawCount;

	ModelMatrix m_modelMatrix;
	ModelMatrix modelLight;

	Texture *m_normalMap;

	const float& m_fdt;
	const float& m_dt;
	bool m_rotateLight = false;
};