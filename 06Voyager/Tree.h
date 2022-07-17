#pragma once

#include "RenderableObject.h"

class Tree : public RenderableObject {
public:
	Tree();
	~Tree();

	void draw(const Camera& camera) override;
	void drawShadow(const Camera& camera) override;
	void drawRaw() override;

};