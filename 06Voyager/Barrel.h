#pragma once

#include "RenderableObject.h"

class Barrel : public RenderableObject {
public:
	Barrel(const float& dt, const float& fdt);
	~Barrel();

	void draw(const Camera& camera) override;
	void drawShadow(const Camera& camera) override;
	void drawRaw(const Camera& camera);	
	void drawAABB(const Camera& camera) override;
	void drawRaw() override;

	void update();
	void toggleLightRotation();
	void setDrawBorder(bool flag);
	ModelMatrix modelLight;
	Texture *m_normalMap;

	const float& m_fdt;
	const float& m_dt;
	bool m_rotateLight = false;

	Shader* m_aabbShader;
	Shader* m_colorShader;
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;
	bool m_drawBorder = false;
	float r, g, b;
	unsigned int m_id;
};