#ifndef _SHARED_H
#define _SHARED_H

struct Rect {
	float posX;
	float posY;
	float width;
	float height;
	mutable bool hasCollision;
};

struct TextureRect {
	float textureOffsetX;
	float textureOffsetY;
	float textureWidth;
	float textureHeight;	
	float width;
	float height;
	unsigned short frame;
};

#endif
