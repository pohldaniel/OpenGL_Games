#pragma once

#include "RenderableObject.h"

class Fern : public RenderableObject {
public:
	Fern();
	~Fern();

	void draw(const Camera& camera) override;
	void drawShadow(const Camera& camera) override;
	void drawRaw() override;
};