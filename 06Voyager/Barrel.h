#pragma once

#include "RenderableObject.h"

class Barrel : public RenderableObject {
public:
	Barrel(const float& dt, const float& fdt);
	~Barrel();

	void draw(const Camera& camera) override;
	void drawShadow(const Camera& camera) override;
	void drawRaw() override;

	void update();
	void toggleLightRotation();

	ModelMatrix modelLight;
	Texture *m_normalMap;

	const float& m_fdt;
	const float& m_dt;
	bool m_rotateLight = false;
};