#pragma once

#include <engine/Texture.h>
#include <engine/Vector.h>

class Background {
public:

	Background(){}
	~Background(){}
	void init(Texture* texture, unsigned int zoomX, unsigned int zoomY, float offsetX, float offsetY);
	void setOffset(float offset);
	void draw();

	unsigned int m_zoomX;
	std::vector<Vector4f> m_textureRects;
	float m_offset = 0.0f;
};