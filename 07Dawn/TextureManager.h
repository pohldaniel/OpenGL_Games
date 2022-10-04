#pragma once
#define NOMINMAX
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "engine/Texture.h"
#include "engine/Extension.h"
#include "engine/Batchrenderer.h"
#include "engine/Instancerenderer.h"
#include "engine/Spritesheet.h"

struct TextureRect {
	float textureOffsetX = 0.0f, textureOffsetY = 0.0f, textureWidth = 0.0f, textureHeight = 0.0f;
	int height = 0, width = 0;
	unsigned int frame = 0;
};

class TextureCache {

public:
	static TextureCache& Get();
	TextureRect& getTextureFromCache(std::string filename);

private:

	TextureCache() = default;
	~TextureCache() = default;

	std::map< std::string, TextureRect> textures;
	static TextureCache s_instance;
};

struct TextureAtlasCreator {
	
public:	

	static TextureAtlasCreator& get() {
		return s_instance;
	}

	void init(unsigned int _width = 1024u, unsigned int _height = 1024u) {

		width = _width;
		height = _height;
		curX = 0;
		curY = 0;
		maxY = 0;
		fillSpace = 0;
		buffer = new unsigned char[width * height * 4];
		memset(buffer, 0, width*height * 4);
		spritesheet = Spritesheet();
		frame = 0;
	}

	unsigned int getAtlas() {
		addFrame();
		delete[] buffer;
		buffer = nullptr;
		return spritesheet.getAtlas();
	}

	void addTexture(TextureRect& stexture, char *texture, unsigned int w, unsigned int h){

		if (width - curX < w){
			curX = 0;
			curY = maxY;
		}

		if (height - curY < h){
			addFrame();
		}

		for (unsigned int row = 0; row<h; ++row){
			memcpy(buffer + (((curY + row) * width + curX) * 4), texture + (w * row * 4), 4 * w);
		}

		stexture.textureOffsetX = static_cast<float>(curX) / static_cast<float>(width);
		stexture.textureWidth = static_cast<float>(w) / static_cast<float>(width);
		stexture.textureOffsetY = static_cast<float>(curY) / static_cast<float>(height);
		stexture.textureHeight = static_cast<float>(h) / static_cast<float>(height);
		stexture.width = w;
		stexture.height = h;
		stexture.frame = spritesheet.getTotalFrames();

		
		curX += w;
		maxY = (std::max)(maxY, curY + h);
	}

	void resetLine() {
		curX = 0;
		curY = maxY;
	}

	void addFrame() {
		if (curX == 0 && curY == 0) return;

		//Texture::Safe("mob" + std::to_string(spritesheet.getTotalFrames()) + ".png", buffer, width, height, 4);

		spritesheet.addToSpritesheet(buffer, width, height);
		
		memset(buffer, 0, width*height * 4);

		curX = 0;
		curY = 0;
		maxY = 0;
		fillSpace = 0;
		frame++;
	}

private:

	TextureAtlasCreator() = default;
	static TextureAtlasCreator s_instance;
	unsigned char* buffer;
	unsigned int width;
	unsigned int height;
	unsigned int curX;
	unsigned int curY;
	unsigned int maxY;
	unsigned int fillSpace;
	Spritesheet spritesheet;
	unsigned short frame;

};

class TextureManager{

public:

	static bool IsRectOnScreen(int left, int width, int bottom, int height);
	static void DrawTextureBatched(TextureRect& textureRect, int x, int y, bool cullVieport, bool updateView);
	static void DrawTextureBatched(TextureRect& textureRect, int x, int y, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool cullVieport = true, bool updateView = true);
	static void DrawTextureBatched(TextureRect& textureRect, int x, int y, float width, float height, bool cullVieport, bool updateView);
	static void DrawTextureBatched(TextureRect& textureRect, int x, int y, float width, float height, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool cullVieport = true, bool updateView = true);


	static void DrawTextureInstanced(TextureRect& textureRect, int x, int y, bool checkVieport = true);
	static TextureRect& Loadimage(std::string file, bool isOpenGLThreadInThreadedMode = false);

	static void Loadimage(std::string file, int textureIndex, std::vector<TextureRect>& textureBase, bool isOpenGLThreadInThreadedMode = false);
	static unsigned int& GetTextureAtlas(std::string name);
	static void SetTextureAtlas(std::string name, unsigned int value);

	static TextureManager& Get();

private:
	TextureManager() = default;

	std::map<std::string, unsigned int> m_textureAtlases;
	static TextureManager s_instance;
};