#include <engine/Texture.h>
#include <engine/Spritesheet.h>

#include "TileSet.h"

TextureAtlasCreator TextureAtlasCreator::s_instance;

void TextureAtlasCreator::init(unsigned int _width, unsigned int _height) {
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

void TextureAtlasCreator::addTexture(unsigned char *texture, unsigned int w, unsigned int h, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects) {

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

	textureRects.push_back({ static_cast<float>(curX) / static_cast<float>(width),
		flipTextureRect ?  static_cast<float>(curY + h) / static_cast<float>(height) : static_cast<float>(curY) / static_cast<float>(height),
		static_cast<float>(w) / static_cast<float>(width),
		flipTextureRect ?  -static_cast<float>(h) / static_cast<float>(height) : static_cast<float>(h) / static_cast<float>(height),
		static_cast<float>(w),
		static_cast<float>(h),
		frame });



	curX += w;
	maxY = (std::max)(maxY, curY + h);
}

void TextureAtlasCreator::addTexture(unsigned char *texture, unsigned int w, unsigned int h, std::vector<TextureRect>& prepacked, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects) {
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
		textureRects.push_back({ (static_cast<float>(curX) + rect.textureOffsetX * static_cast<float>(w)) / static_cast<float>(width),
			flipTextureRect ? (static_cast<float>(curY) + (rect.textureOffsetY + rect.textureHeight) * static_cast<float>(h)) / static_cast<float>(height) : (static_cast<float>(curY) + rect.textureOffsetY * static_cast<float>(h)) / static_cast<float>(height),
			((rect.textureWidth)  * static_cast<float>(w)) / static_cast<float>(width),
			flipTextureRect ?  -((rect.textureHeight) * static_cast<float>(h)) / static_cast<float>(height) : ((rect.textureHeight) * static_cast<float>(h)) / static_cast<float>(height) ,
			rect.width,
			rect.height,
			frame });
	}

	curX += w;
	maxY = (std::max)(maxY, curY + h);
}

void TextureAtlasCreator::addTexture(unsigned char *texture, unsigned int w, unsigned int h, unsigned int _maxWidth, unsigned int _maxHeight) {
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
	
	for (unsigned int row = 0; row < h; ++row) {
		memcpy(bufferPtr + (((curY + row) * width + curX) * 4), texture + (w * row * 4), 4 * w);
	}

	curX += w;
	maxY = (std::max)(maxY, curY + h);
}

void TextureAtlasCreator::resetLine() {
	curX = 0;
	curY = maxY;
}

void TextureAtlasCreator::addFrame() {
	if (curX == 0 && curY == 0) return;

	curX = 0;
	curY = 0;
	maxY = 0;
	fillSpace = 0;

	frame++;

	spritesheet = new unsigned char[width * height * 4 * frame];
	memcpy(spritesheet, spritesheetPtr, width * height * 4 * (frame - 1) * sizeof(unsigned char));
	memcpy(spritesheet + (width * height * 4 * (frame - 1) * sizeof(unsigned char)), bufferPtr, width * height * 4 * sizeof(unsigned char));

	if (spritesheetPtr)
		delete[] spritesheetPtr;

	spritesheetPtr = spritesheet;

	memset(bufferPtr, 0, width * height * 4);
	bufferPtr = buffer;
}

void TextureAtlasCreator::getAtlas(unsigned int& textureRef) {
	addFrame();
	delete[] buffer;
	buffer = nullptr;

	Spritesheet::CreateSpritesheet(spritesheetPtr, width, height, frame, textureRef);

	delete[] spritesheetPtr;
	spritesheet = nullptr;
	spritesheetPtr = nullptr;
}

TextureAtlasCreator& TextureAtlasCreator::Get() {
	return s_instance;
}

////////////////////////////TileSet//////////////////////////////////////////
TileSet::TileSet() : m_init(false) {

}

void TileSet::loadTileSetCpu(std::vector<std::string> texturePaths, unsigned int width, unsigned int height, bool resetLine) {
	if (m_init) return;
	cutOff = texturePaths.size();

	TextureAtlasCreator::Get().init(width, height);
	
	for(auto& path: texturePaths){ 
		int imageWidth, imageHeight;
		unsigned char* bytes = Texture::LoadFromFile(path, imageWidth, imageHeight, true);

		TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, false, 0u, 0u, m_textureRects);
		if (resetLine)
			TextureAtlasCreator::Get().resetLine();

		free(bytes);
	}	
}

void TileSet::loadTileSetCpu(std::string mapPath, std::string texturePath, unsigned int columns, unsigned int rows, float tileSize, const bool flipVertical, bool flipTextureRects) {
	if (m_init) return;
	cutOff = columns * rows;

	std::ifstream file(mapPath);

	unsigned int* ids = new unsigned int[columns * rows];
	
	if (file){
		for (unsigned int i, index = 0; file >> i; index++) {
			ids[index] = i;
			if (file.peek() == ',')
				file.ignore();
		}
		file.close();
	}

	int imageWidth, imageHeight;
	unsigned char* bytes = Texture::LoadFromFile(texturePath, imageWidth, imageHeight, true);
	int tileCountX = imageWidth / static_cast<int>(tileSize);
	int tileCountY = imageHeight / static_cast<int>(tileSize);

	float tileWidth = tileSize;
	float tileHeight = tileSize;
	std::vector<TextureRect> textureRects;
	for (int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {

			unsigned int id = ids[y * columns + x];
			float srcRectX = (id % tileCountX) * tileSize;
			float srcRectY = (id / tileCountX) * tileSize;

			if (!flipTextureRects) {
				textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
										   (static_cast<float>(imageHeight) - srcRectY - tileHeight) / static_cast<float>(imageHeight),
										   (tileWidth - 0.5f) / static_cast<float>(imageWidth),
										   tileHeight / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}else {
				textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
										   (static_cast<float>(imageHeight) - srcRectY) / static_cast<float>(imageHeight),
										   (tileWidth - 0.5f) / static_cast<float>(imageWidth),
										   -tileHeight / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}
		}
	}
	delete[] ids;

	
	//TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, 0u, 0u);
	TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, textureRects, false, 0u, 0u, m_textureRects);
}

void TileSet::loadTileSetCpu(std::string texturePath, float tileWidth, float tileHeight, const bool flipVertical, bool flipTextureRects) {
	if (m_init) return;
		
	int imageWidth, imageHeight;
	unsigned char* bytes = Texture::LoadFromFile(texturePath, imageWidth, imageHeight, true);
	int tileCountX = imageWidth / static_cast<int>(tileWidth);
	int tileCountY = imageHeight / static_cast<int>(tileHeight);
	cutOff = tileCountX * tileCountY;

	std::vector<TextureRect> textureRects;

	for (int y = 0; y < tileCountY; y++) {
		for (int x = 0; x < tileCountX; x++) {
			unsigned int id = y * tileCountX + x;
			float srcRectX = (id % tileCountX) * tileWidth;
			float srcRectY = (id / tileCountX) * tileHeight;

			if (!flipTextureRects) {
				textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
										   (static_cast<float>(imageHeight) - srcRectY - tileHeight) / static_cast<float>(imageHeight),
										   (tileWidth - 0.5f) / static_cast<float>(imageWidth),
										   tileHeight / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}else {
				textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
										   (static_cast<float>(imageHeight) - srcRectY) / static_cast<float>(imageHeight),
										   (tileWidth - 0.5f) / static_cast<float>(imageWidth),
										   -tileHeight / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}
		}
	}
	TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, textureRects, false, 0u, 0u, m_textureRects);
}

void TileSet::loadTileSetCpu2(std::string texturePath, const bool flipVertical, bool flipTextureRects) {
	if (m_init) return;

	int imageWidth, imageHeight;
	unsigned char* bytes = Texture::LoadFromFile(texturePath, imageWidth, imageHeight, true);
	cutOff = 1u;
	std::vector<TextureRect> textureRects;

	float tileWidth = static_cast<float>(imageWidth);
	float tileHeight = static_cast<float>(imageHeight);

	float srcRectX = 0.0f;
	float srcRectY = 0.0f;

	if (!flipTextureRects) {
		textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
								   (static_cast<float>(imageHeight) - srcRectY - tileHeight) / static_cast<float>(imageHeight),
								   (tileWidth - 0.5f) / static_cast<float>(imageWidth),
								   tileHeight / static_cast<float>(imageHeight),
								   tileWidth,
								   tileHeight,
								   0u });
	}else {
		textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
								   (static_cast<float>(imageHeight) - srcRectY) / static_cast<float>(imageHeight),
								   (tileWidth - 0.5f) / static_cast<float>(imageWidth),
								   -tileHeight / static_cast<float>(imageHeight),
								   tileWidth,
								   tileHeight,
								   0u });
	}
	TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, textureRects, false, 0u, 0u, m_textureRects);
}

void TileSet::loadTileSetGpu() {
	if (m_init) return;
	TextureAtlasCreator::Get().getAtlas(m_atlas);
	m_init = true;
}

const unsigned int& TileSet::getAtlas() const {
	return m_atlas;
}

const std::vector<TextureRect>& TileSet::getTextureRects() const {
	return m_textureRects;
}

const std::vector<TextureRect>& TileSet::getLastTextureRects() const {
	return std::vector<TextureRect>(m_textureRects.end() - cutOff, m_textureRects.end());
}

///////////////////////TileSetManager//////////////////////////
TileSetManager TileSetManager::s_instance;

TileSet& TileSetManager::getTileSet(std::string name) {
	return m_tileSets[name];
}

bool TileSetManager::containsTileset(std::string name) {
	return m_tileSets.count(name) == 1;
}

TileSetManager& TileSetManager::Get() {
	return s_instance;
}