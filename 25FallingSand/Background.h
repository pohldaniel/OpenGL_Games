#pragma once

#include <engine/Texture.h>
#include <engine/Vector.h>

struct BackgroundLayer {

	void init(Texture* texture, unsigned int zoomX, float offset = 0.0f, Vector2f textureOffset = Vector2f(0.0f, 1.0f));
	void addOffset(float offset);
	void draw();


private:

	float clampOffset(float input);

	unsigned int m_zoomX;
	float m_imageWidth;
	Texture* m_texture;
	std::vector<Vector4f> m_textureRects;
	float m_offset;
	float m_store;
};