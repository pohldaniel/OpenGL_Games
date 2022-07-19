#pragma once

#include "RenderableObject.h"

class Fern : public RenderableObject {
public:
	Fern();
	~Fern();

	void draw(const Camera& camera) override;
	void drawShadow(const Camera& camera) override;
	void drawRaw(const Camera& camera);
	void drawAABB(const Camera& camera) override;
	void drawRaw() override;
	void setDrawBorder(bool flag);

	Shader* m_aabbShader;
	Shader* m_colorShader;
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;
	bool m_drawBorder = false;
	float r, g, b;
	unsigned int m_id;
};