#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "Rect.h"
#include "CharacterSet.h"

struct TextureAtlasCreator {

public:

	void init(unsigned int _width = 1024u, unsigned int _height = 1024u);

	void addTexture(unsigned char *texture, unsigned int w, unsigned int h, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects);
	void addTexture(unsigned char *texture, unsigned int w, unsigned int h, std::vector<TextureRect>& prepacked, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects);
	void addCharset(unsigned char *texture, unsigned int w, unsigned int h, CharacterSet& characterSet, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects);
	void addTexture(unsigned char *texture, unsigned int w, unsigned int h, unsigned int maxWidth, unsigned int maxHeight);
	void resetLine();
	void addFrame();
	void getAtlas(unsigned int& textureRef);

	static TextureAtlasCreator& Get();

private:

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

	static TextureAtlasCreator s_instance;
};

class TileSet {

	friend class TileSetManager;

public:

	TileSet();
	void loadTileSetCpu(const std::vector<std::string>& texturePaths, bool init, bool flipVertical = true, bool flipTextureRect = false, bool resetLine = false);
	void loadTileSetCpu(std::string mapPath, std::string texturePath, bool init, unsigned int columns, unsigned int rows, float tileSize, bool flipVertical = true, bool flipTextureRects = false);
	void loadTileSetCpu(std::string texturePath, bool init, float tileHWidth, float tileHeight, bool flipVertical = true, bool flipTextureRects = false);
	void loadTileCpu(std::string texturePath, bool init, bool flipVertical = true, bool flipTextureRects = false);
	void loadTileCpu(std::string texturePath, bool init, unsigned int posX, unsigned int posY, unsigned int width, unsigned int height, bool flipVertical = true, bool flipTextureRects = false);
	void addCharset(CharacterSet& characterSet, bool init);
	
	const std::vector<TextureRect> getLastTextureRects() const;

	void loadTileSetGpu();
	const std::vector<TextureRect>& getTextureRects() const;
	const unsigned int& getAtlas() const;

private:

	unsigned int m_atlas = 0;
	std::vector<TextureRect> m_textureRects;
	bool m_init;
	unsigned int cutOff = 0;
};

class TileSetManager {

public:

	TileSet& getTileSet(std::string name);
	static TileSetManager& Get();

	bool containsTileset(std::string name);

private:
	TileSetManager() = default;

	std::unordered_map<std::string, TileSet> m_tileSets;
	static TileSetManager s_instance;
};