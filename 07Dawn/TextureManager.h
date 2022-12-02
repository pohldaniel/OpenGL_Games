#pragma once
#define NOMINMAX
#include <map>
#include <vector>
#include <iostream>
#include <algorithm>

#include "engine/Rect.h"
#include "engine/Texture.h"
#include "engine/Extension.h"
#include "engine/Batchrenderer.h"
#include "engine/Instancerenderer.h"
#include "engine/Spritesheet.h"


class TextureCache {

public:
	static TextureCache& Get();
	TextureRect& getTextureFromCache(std::string filename, unsigned int maxWidth = 0, unsigned maxHeight = 0, bool reload = false, bool transparent = false, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);

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
		bufferPtr = buffer;
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

	void safeAtlas(std::string name) {
		spritesheet.safe(name);
	}

	void addTexture(TextureRect& stexture, char *texture, unsigned int w, unsigned int h, unsigned int _maxWidth = 0, unsigned int _maxHeight = 0, unsigned int textureOffsetX = 0, unsigned int textureOffsetY = 0){

		unsigned int maxWidth = _maxWidth > 0 ? _maxWidth : width;
		unsigned int maxHeight = _maxHeight > 0 ? _maxHeight : height;

		if (maxWidth - curX < w) {
			curX = 0;
			curY = maxY;
		}
		
		if (maxHeight - curY < h){
			addFrame();
		}

		if (maxHeight < h || maxWidth < w) {
			std::cout << "Texture to large for TextrueAtlas" << std::endl;
			return;
		}

		for (unsigned int row = 0; row<h; ++row){
			memcpy(bufferPtr + (((curY + row) * width + curX) * 4), texture + (w * row * 4), 4 * w);
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
		spritesheet.addToSpritesheet(bufferPtr, width, height);
		memset(bufferPtr, 0, width*height * 4);
		bufferPtr = buffer;
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
	unsigned char* bufferPtr;
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
	static bool CheckPointInRect(float px, float py, int left, int width, int bottom, int height);
	
	static void DrawTextureBatched(const TextureRect& textureRect, int x, int y, bool cullVieport, bool updateView);
	static void DrawTextureBatched(const TextureRect& textureRect, int x, int y, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool cullVieport = true, bool updateView = true);
	static void DrawTextureBatched(const TextureRect& textureRect, int x, int y, float width, float height, bool cullVieport, bool updateView);
	static void DrawTextureBatched(const TextureRect& textureRect, int x, int y, float width, float height, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool cullVieport = true, bool updateView = true);
	static void DrawRotatedTextureBatched(const TextureRect& textureRect, int x, int y, float angle, float rotX, float rotY, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = true);

	static void DrawTexture(const TextureRect& textureRect, int x, int y, bool cullVieport, bool updateView);
	static void DrawTexture(const TextureRect& textureRect, int x, int y, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool cullVieport = true, bool updateView = true);
	static void DrawTexture(const TextureRect& textureRect, int x, int y, float width, float height, bool cullVieport, bool updateView);
	static void DrawTexture(const TextureRect& textureRect, int x, int y, float width, float height, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool cullVieport = true, bool updateView = true);
	static void DrawRotatedTexture(const TextureRect& textureRect, int x, int y, float angle, float rotX, float rotY, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = true);

	static void DrawTextureInstanced(const TextureRect& textureRect, int x, int y, bool checkVieport = true);
	static void DrawBuffer(bool updateView = true);

	static TextureRect& Loadimage(std::string file, unsigned int maxWidth, unsigned maxHeight, bool reload, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom);
	static TextureRect& Loadimage(std::string file);
	static void Loadimage(std::string file, int textureIndex, std::vector<TextureRect>& textureBase, bool reload = false, bool transparent = false);

	static unsigned int& GetTextureAtlas(std::string name);
	static void SetTextureAtlas(std::string name, unsigned int value);
	
	static void BindTexture(unsigned int texture, bool isTextureArray = false);
	static void UnbindTexture(bool isTextureArray = false);

	static void BindTexture(unsigned int texture, bool isTextureArray, unsigned int unit);
	static void UnbindTexture(bool isTextureArray, unsigned int unit);

	static void ActiveTexture(unsigned int unit);

	static void SetShader(Shader* shader);
	static TextureManager& Get();

	

private:
	TextureManager() = default;

	std::map<std::string, unsigned int> m_textureAtlases;
	static TextureManager s_instance;

	static float(&QuadPos)[8];
	static float(&TexPos)[8];
	static float(&Color)[4];
	static unsigned int& Frame;
	static unsigned int& Sampler;
};