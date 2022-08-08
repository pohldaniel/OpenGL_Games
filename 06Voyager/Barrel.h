#pragma once

#include "RenderableObject.h"

class Barrel : public RenderableObject {
public:
	Barrel();
	~Barrel();
	
	void draw(const Camera& camera) override;
	void drawHull(const Camera& camera) override;
	

	void update(float dt);
	void toggleLightRotation();
	void setDrawBorder(bool flag);
	ModelMatrix modelLight;
	Texture *m_normalMap;

private:
	Matrix4f m_transformOutline = Matrix4f::IDENTITY;
	bool m_rotateLight = false;
	bool m_drawBorder = false;	
};