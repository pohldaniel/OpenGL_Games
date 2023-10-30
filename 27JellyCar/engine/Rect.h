#ifndef _SHARED_H
#define _SHARED_H

struct Rect {
	float posX;
	float posY;
	float width;
	float height;
};

struct TextureRect {
	float textureOffsetX;
	float textureOffsetY;
	float textureWidth;
	float textureHeight;
	unsigned int height;
	unsigned int width;
	unsigned short frame;
};

#endif
