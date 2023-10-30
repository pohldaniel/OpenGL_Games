#pragma once
#include <map>
#include "Texture2.h"

class TextureManager
{
private:

	static TextureManager* _textureManager;
	std::map<std::string, Texture2*> _images;

	TextureManager();

	std::string GenerateName(int len);

public:

	static TextureManager* Instance();

	Texture2* LoadFromFile(std::string fileName, TextureFilerType filterType = TextureFilerType::LinearFilter, TextureColorType textureColor = TextureColorType::Texture_RGBA, TextureWrapType textureWrap = TextureWrapType::Repeat, int mipLevel = 0);
	Texture2* LoadFromMemory(std::string newName, unsigned char *data, size_t size, TextureFilerType filterType = TextureFilerType::LinearFilter, TextureColorType textureColor = TextureColorType::Texture_RGBA, TextureWrapType textureWrap = TextureWrapType::Repeat, int mipLevel = 0);

	Texture2* CreateEmpty(std::string name, int width, int height, TextureFilerType filterType, TextureColorType textureColor = TextureColorType::Texture_RGBA, TextureWrapType textureWrap = TextureWrapType::Repeat, int mipLevel = 0);

	Texture2* Get(std::string fileName);

	void Add(Texture2* image);

	void Remove(std::string fileName);
	void Remove(Texture2* image);
	void RemoveAll();

};