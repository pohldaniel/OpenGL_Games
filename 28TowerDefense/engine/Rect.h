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
	unsigned int width;
	unsigned int height;
	unsigned short frame;
};

#endif
