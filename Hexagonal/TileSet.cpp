#include <engine/Spritesheet.h>

#include "TileSet.h"

TileSet TileSet::s_instance;

void TileSet::init(unsigned int _width, unsigned int _height) {
	width = _width;
	height = _height;
	curX = 0;
	curY = 0;
	maxY = 0;
	fillSpace = 0;
	buffer = new unsigned char[width * height * 4];
	bufferPtr = buffer;
	memset(buffer, 0, width * height * 4);
	frame = 0;

	spritesheetPtr = nullptr;
	spritesheet = nullptr;
}

void TileSet::addTexture(unsigned char *texture, unsigned int w, unsigned int h, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight) {

	unsigned int maxWidth = _maxWidth > 0 ? _maxWidth : width;
	unsigned int maxHeight = _maxHeight > 0 ? _maxHeight : height;

	if (maxWidth - curX < w) {
		curX = 0;
		curY = maxY;
	}

	if (maxHeight - curY < h) {
		addFrame();
	}

	if (maxHeight < h || maxWidth < w) {
		std::cout << "Texture to large for TextrueAtlas" << std::endl;
		return;
	}

	for (unsigned int row = 0; row < h; ++row) {
		memcpy(bufferPtr + (((curY + row) * width + curX) * 4), texture + (w * row * 4), 4 * w);
	}

	m_textureRects.push_back({ static_cast<float>(curX) / static_cast<float>(width), 
                               flipTextureRect ? static_cast<float>(curY + h) / static_cast<float>(height) : static_cast<float>(curY) / static_cast<float>(height),
                               static_cast<float>(w) / static_cast<float>(width), 
                               flipTextureRect ? -static_cast<float>(h) / static_cast<float>(height) : static_cast<float>(h) / static_cast<float>(height),
                               w, 
                               h,
                               frame });

	

	curX += w;
	maxY = (std::max)(maxY, curY + h);
}

void TileSet::addTexture(unsigned char *texture, unsigned int w, unsigned int h, std::vector<TextureRect>& prepacked, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight) {
	unsigned int maxWidth = _maxWidth > 0 ? _maxWidth : width;
	unsigned int maxHeight = _maxHeight > 0 ? _maxHeight : height;

	if (maxWidth - curX < w) {
		curX = 0;
		curY = maxY;
	}

	if (maxHeight - curY < h) {
		addFrame();
	}

	if (maxHeight < h || maxWidth < w) {
		std::cout << "Texture to large for TextrueAtlas" << std::endl;
		return;
	}

	for (unsigned int row = 0; row < h; ++row) {
		memcpy(bufferPtr + (((curY + row) * width + curX) * 4), texture + (w * row * 4), 4 * w);
	}

	for (auto rect : prepacked) {
		m_textureRects.push_back({ (static_cast<float>(curX) + rect.textureOffsetX * static_cast<float>(w)) / static_cast<float>(width),
                                    flipTextureRect ? (static_cast<float>(curY) + (rect.textureOffsetY + rect.textureHeight) * static_cast<float>(h)) / static_cast<float>(height) : (static_cast<float>(curY) + rect.textureOffsetY * static_cast<float>(h)) / static_cast<float>(height),
								    ((rect.textureWidth)  * static_cast<float>(w)) / static_cast<float>(width),
                                    flipTextureRect ? -((rect.textureHeight) * static_cast<float>(h)) / static_cast<float>(height) : ((rect.textureHeight) * static_cast<float>(h)) / static_cast<float>(height),
                                    rect.width,
                                    rect.height,
                                    frame });
	}

	



	curX += w;
	maxY = (std::max)(maxY, curY + h);
}

void TileSet::resetLine() {
	curX = 0;
	curY = maxY;
}

void TileSet::addFrame() {
	if (curX == 0 && curY == 0) return;

	curX = 0;
	curY = 0;
	maxY = 0;
	fillSpace = 0;

	frame++;

	spritesheet = new unsigned char[width * height * 4 * frame];
	memcpy(spritesheet, spritesheetPtr, width * height * 4 * (frame - 1) * sizeof(unsigned char));
	memcpy(spritesheet + (width * height * 4 * (frame - 1) * sizeof(unsigned char)), bufferPtr, width * height * 4 * sizeof(unsigned char));
	
	if(spritesheetPtr)
		delete[] spritesheetPtr;
	
	spritesheetPtr = spritesheet;

	memset(bufferPtr, 0, width * height * 4);
	bufferPtr = buffer;
}

unsigned int TileSet::getAtlas() {
	addFrame();
	delete[] buffer;
	buffer = nullptr;
	
	unsigned int texture = Spritesheet::CreateSpritesheet(spritesheetPtr, width, height, frame);

	delete[] spritesheetPtr;
	spritesheet = nullptr;
	spritesheetPtr = nullptr;

	return texture;
}

const std::vector<TextureRect>& TileSet::getTextureRects() const {
	return m_textureRects;
}

TileSet& TileSet::Get() {
	return s_instance;
}