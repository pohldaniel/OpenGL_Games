#pragma once

#include "RenderableObject.h"

class Tree : public RenderableObject {
public:
	Tree();
	~Tree();

	void draw(const Camera& camera) override;
	void setDrawBorder(bool flag);

private:
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;
	bool m_drawBorder = false;
};