#pragma once

#include <iostream>
#include <algorithm>
#include <vector>

#include <engine/Rect.h>

class TileSet {

public:

	void addTexture(unsigned char *texture, unsigned int w, unsigned int h, bool flipTextureRect = false, unsigned int _maxWidth = 0u, unsigned int _maxHeight = 0u);
	void addTexture(unsigned char *texture, unsigned int w, unsigned int h, std::vector<TextureRect>& prepacked, bool flipTextureRect = false, unsigned int _maxWidth = 0u, unsigned int _maxHeight = 0u);
	void init(unsigned int _width = 1024u, unsigned int _height = 1024u);
	void resetLine();
	void addFrame();
	unsigned int getAtlas();
	const std::vector<TextureRect>& getTextureRects() const;

	static TileSet& Get();

private:

	TileSet() = default;

	unsigned char* buffer;
	unsigned char* bufferPtr;
	unsigned char* spritesheetPtr;
	unsigned char* spritesheet;

	unsigned int width;
	unsigned int height;
	unsigned int curX;
	unsigned int curY;
	unsigned int maxY;
	unsigned int fillSpace;
	unsigned short frame;

	std::vector<TextureRect> m_textureRects;

	static TileSet s_instance;
};