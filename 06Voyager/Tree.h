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
	void drawHull(const Camera& camera);
	void drawRaw() override;
	void setDrawBorder(bool flag);

	Shader* m_colorShader;

	Matrix4f m_transformOutline = Matrix4f::IDENTITY;
	bool m_drawBorder = false;
	unsigned int m_id;
	Vector4f m_pickColor;
};