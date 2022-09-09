#pragma once
#define NOMINMAX
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "engine/Texture.h"
#include "engine/Extension.h"
#include "engine/Batchrenderer.h"
#include "engine/Spritesheet.h"


struct DawnTexture {
	unsigned int texture;
	float x1, x2, y1, y2;
	int height, width, textureOffsetX, textureOffsetY;	
	unsigned int frame;
	DawnTexture() {
		texture = 0;
		x1 = 0.0f;
		x2 = 0.0f;
		y1 = 0.0f;
		y2 = 0.0f;
		height = 0;
		width = 0;
		textureOffsetX = 0;
		textureOffsetY = 0;
		frame = 0;
	}
};

class TextureCache {

public:
	static TextureCache& get() {
		return s_instance;
	}

	DawnTexture& getTextureFromCache(std::string filename);

private:

	TextureCache() = default;
	~TextureCache() = default;

	std::map< std::string, DawnTexture > textures;
	static TextureCache s_instance;
};

struct TextureAtlasCreator {
	
public:	

	static TextureAtlasCreator& get() {
		return s_instance;
	}

	void init(unsigned int& _textureAtlas, unsigned _width = 1024u, unsigned int _height = 1024u) {
		textureAtlas = &_textureAtlas;
		width = _width;
		height = _height;
		curX = 0;
		curY = 0;
		maxY = 0;
		buffer = new unsigned char[width * height * 4];
		memset(buffer, 0, width*height * 4);
	}

	void shutdown() {
		addFrame();
		*textureAtlas = spritesheet.getAtlas();
		delete[] buffer;
		buffer = nullptr;
	}

	void addTexture(DawnTexture& stexture, char *texture, size_t w, size_t h){

		if (width - curX < w){
			curX = 0;
			curY = maxY;
		}

		if (height - curY < h){
			addFrame();
		}

		for (size_t row = 0; row<h; ++row){
			memcpy(buffer + (((curY + row)*width + curX) * 4), texture + (w*row * 4), 4 * w);
		}

		stexture.x1 = static_cast<float>(curX) / static_cast<float>(width);
		stexture.x2 = static_cast<float>(curX + w) / static_cast<float>(width);
		stexture.y1 = static_cast<float>(curY) / static_cast<float>(height);
		stexture.y2 = static_cast<float>(curY + h) / static_cast<float>(height);
		stexture.width = w;
		stexture.height = h;
		stexture.frame = spritesheet.getTotalFrames();
		curX += w;
		maxY = (std::max)(maxY, curY + h);
	}

	void addFrame() {
		if (curX == 0 && curY == 0) return;

		spritesheet.addToSpritesheet(buffer, width, height);

		memset(buffer, 0, width*height * 4);
		curX = 0;
		curY = 0;
		maxY = 0;
	}

	size_t getWidth() const {
		return width;
	}

	size_t getHeight() const {
		return height;
	}

private:

	TextureAtlasCreator() = default;
	static TextureAtlasCreator s_instance;
	unsigned char* buffer;
	size_t width;
	size_t height;
	size_t curX;
	size_t curY;
	size_t maxY;
	Spritesheet spritesheet;

	unsigned int* textureAtlas;
};



class TextureManager{

public:
	static bool IsRectOnScreen(int left, int width, int bottom, int height);
	static void DrawTexture(DawnTexture& stexture, int x, int y, float transparency = 1.0f, float red = 1.0f, float green = 1.0f, float blue = 1.0f, float x_scale = 1.0f, float y_scale = 1.0f);
	static DawnTexture& Loadimage(std::string file, bool isOpenGLThreadInThreadedMode = false, int textureOffsetX = 0, int textureOffsetY = 0);
};