#pragma once

#include "RenderableObject.h"

class Fern : public RenderableObject {
public:
	Fern();
	~Fern();

	void draw(const Camera& camera) override;
	void drawShadow(const Camera& camera) override;
	void setDrawBorder(bool flag);

private:
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;
	bool m_drawBorder = false;	
};