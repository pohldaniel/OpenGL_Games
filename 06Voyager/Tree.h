#pragma once

#include "RenderableObject.h"

class Tree : public RenderableObject {
public:
	Tree();
	~Tree();

	void draw(const Camera& camera) override;
	void drawShadow(const Camera& camera) override;
	void drawRaw(const Camera& camera);
	void drawAABB(const Camera& camera) override;
	void drawSphere(const Camera& camera);
	void drawRaw() override;
	void setDrawBorder(bool flag);

	Texture *m_nullTexture;
	Shader* m_aabbShader;
	Shader* m_colorShader;
	Shader* m_sphereShader;
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;
	bool m_drawBorder = false;
	float r, g, b;
	unsigned int m_id;
};