#pragma once

#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include <engine/Rect.h>

struct TextureAtlasCreator {

public:

	void init(unsigned int _width = 1024u, unsigned int _height = 1024u);

	void addTexture(unsigned char *texture, unsigned int w, unsigned int h, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects);
	void addTexture(unsigned char *texture, unsigned int w, unsigned int h, std::vector<TextureRect>& prepacked, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects);
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
	void loadTileSet(std::vector<std::string>  texturePaths, unsigned int width = 1024u, unsigned int height = 1024u, bool resetLine = false);
	const std::vector<TextureRect>& getTextureRects() const;
	const unsigned int& getAtlas() const;

private:

	unsigned int m_atlas;
	std::vector<TextureRect> m_textureRects;
	bool m_init;
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