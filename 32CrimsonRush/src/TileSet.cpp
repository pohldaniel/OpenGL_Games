#include <iterator>
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

	textureRects.push_back({ (static_cast<float>(curX) + 0.5f) / static_cast<float>(width),
		flipTextureRect ?  static_cast<float>(curY + h) / static_cast<float>(height) : static_cast<float>(curY) / static_cast<float>(height),
		(static_cast<float>(w) - 0.5002f) / static_cast<float>(width),
		flipTextureRect ? (0.5f - static_cast<float>(h)) / static_cast<float>(height) : static_cast<float>(h) / static_cast<float>(height),
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

void TextureAtlasCreator::addCharset(unsigned char *texture, unsigned int w, unsigned int h, CharacterSet& characterSet, bool flipTextureRect, unsigned int _maxWidth, unsigned int _maxHeight, std::vector<TextureRect>& textureRects) {
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
	std::map<char, Char>::iterator it;
	for (it = characterSet.characters.begin(); it != characterSet.characters.end(); it++) {
		if (flipTextureRect) {
			it->second.textureOffset[0] = (static_cast<float>(curX) + it->second.textureOffset[0] * static_cast<float>(w)) / static_cast<float>(width);
			it->second.textureOffset[1] = (static_cast<float>(curY) + (it->second.textureOffset[1] + it->second.textureSize[1]) * static_cast<float>(h)) / static_cast<float>(height);
			it->second.textureSize[0] =  ((it->second.textureSize[0]) * static_cast<float>(w)) / static_cast<float>(width);
			it->second.textureSize[1] = -((it->second.textureSize[1]) * static_cast<float>(h)) / static_cast<float>(height);
		}else {
			it->second.textureOffset[0] = (static_cast<float>(curX) + it->second.textureOffset[0] * static_cast<float>(w)) / static_cast<float>(width);
			it->second.textureOffset[1] = (static_cast<float>(curY) + it->second.textureOffset[1] * static_cast<float>(h)) / static_cast<float>(height);
			it->second.textureSize[0] = ((it->second.textureSize[0]) * static_cast<float>(w)) / static_cast<float>(width);
			it->second.textureSize[1] = ((it->second.textureSize[1]) * static_cast<float>(h)) / static_cast<float>(height);
		}
	}

	characterSet.frame = frame;

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

void TileSet::loadTileSetCpu(const std::vector<std::string>& texturePaths, bool flipVertical, bool flipTextureRect, bool resetLine) {
	if (m_init) return;
	cutOff = texturePaths.size();

	for(auto& path: texturePaths){ 
		int imageWidth, imageHeight;
		unsigned char* bytes = Texture::LoadFromFile(path, imageWidth, imageHeight, flipVertical);

		TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, flipTextureRect, 0u, 0u, m_textureRects);
		if (resetLine)
			TextureAtlasCreator::Get().resetLine();

		free(bytes);
	}	
}

void TileSet::loadTileSetCpu(std::string mapPath, std::string texturePath, unsigned int columns, unsigned int rows, float tileSize, bool flipVertical, bool flipTextureRects) {
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
	unsigned char* bytes = Texture::LoadFromFile(texturePath, imageWidth, imageHeight, flipVertical);
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
										   (tileWidth - 0.5002f) / static_cast<float>(imageWidth),
										   tileHeight / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}else {
				textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
										   (static_cast<float>(imageHeight) - srcRectY - 0.5f) / static_cast<float>(imageHeight),
										   (tileWidth - 0.5002f) / static_cast<float>(imageWidth),
										   -tileHeight / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}
		}
	}
	delete[] ids;

	TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, textureRects, false, 0u, 0u, m_textureRects);
}

void TileSet::loadTileSetCpu(std::string texturePath, float tileWidth, float tileHeight, bool flipVertical, bool flipTextureRects) {
	if (m_init) return;
		
	int imageWidth, imageHeight;
	unsigned char* bytes = Texture::LoadFromFile(texturePath, imageWidth, imageHeight, flipVertical);
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
										   (tileWidth - 0.5002f) / static_cast<float>(imageWidth),
										   tileHeight / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}else {
				textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
										   (static_cast<float>(imageHeight) - srcRectY - 0.5f) / static_cast<float>(imageHeight),
										   (tileWidth - 0.5002f) / static_cast<float>(imageWidth),
										   (0.5f - tileHeight) / static_cast<float>(imageHeight),
										   tileWidth,
										   tileHeight,
										   0u });
			}
		}
	}
	TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, textureRects, false, 0u, 0u, m_textureRects);
}

void TileSet::loadTileSetCpu(std::string texturePath, bool flipVertical, bool flipTextureRects) {
	if (m_init) return;

	int imageWidth, imageHeight;
	unsigned char* bytes = Texture::LoadFromFile(texturePath, imageWidth, imageHeight, flipVertical);
	cutOff = 1u;
	std::vector<TextureRect> textureRects;

	float tileWidth = static_cast<float>(imageWidth);
	float tileHeight = static_cast<float>(imageHeight);

	float srcRectX = 0.0f;
	float srcRectY = 0.0f;

	if (!flipTextureRects) {
		textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
								   (static_cast<float>(imageHeight) - srcRectY - tileHeight) / static_cast<float>(imageHeight),
								   (tileWidth - 0.5002f) / static_cast<float>(imageWidth),
								   (tileHeight - 0.5f) / static_cast<float>(imageHeight),
								   tileWidth,
								   tileHeight,
								   0u });
	}else {
		textureRects.push_back({ (srcRectX + 0.5f) / static_cast<float>(imageWidth),
								   (static_cast<float>(imageHeight) - srcRectY - 0.5f) / static_cast<float>(imageHeight),
								   (tileWidth - 0.5002f) / static_cast<float>(imageWidth),
								   -tileHeight / static_cast<float>(imageHeight),
								   tileWidth,
								   tileHeight,
								   0u });
	}
	TextureAtlasCreator::Get().addTexture(bytes, imageWidth, imageHeight, textureRects, false, 0u, 0u, m_textureRects);
}

void TileSet::addCharset(CharacterSet& characterSet) {
	if (m_init) return;

	int cutOff = characterSet.characters.size();
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	unsigned char* bytes = (unsigned char*)malloc(characterSet.maxWidth * characterSet.maxHeight * sizeof(unsigned char));
	glBindTexture(GL_TEXTURE_2D_ARRAY, characterSet.spriteSheet);
	glGetTexImage(GL_TEXTURE_2D_ARRAY, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	
	unsigned char* bytesNew = (unsigned char*)malloc(characterSet.maxWidth * characterSet.maxHeight * 4);

	for (unsigned int i = 0, k = 0; i < characterSet.maxWidth * characterSet.maxHeight * 4; i = i + 4, k++) {
		bytesNew[i] = (int)bytes[k];
		bytesNew[i + 1] = (int)bytes[k];
		bytesNew[i + 2] = (int)bytes[k];
		bytesNew[i + 3] = bytes[k] >= 220 ? 255 : 0;
	}

	TextureAtlasCreator::Get().addCharset(bytesNew, characterSet.maxWidth, characterSet.maxHeight, characterSet, false, 0u, 0u, m_textureRects);
	free(bytes);
	free(bytesNew);
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

const std::vector<TextureRect> TileSet::getLastTextureRects() const {
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