#include "TextureManager.h"
#include "RenderManager.h"
#include "MemoryManager.h"

TextureManager * TextureManager::_textureManager;

TextureManager::TextureManager()
{

}

TextureManager* TextureManager::Instance()
{
	if (_textureManager == 0)
	{
		_textureManager = new TextureManager();
	}

	return _textureManager;
}

Texture2* TextureManager::LoadFromFile(std::string fileName, TextureFilerType filterType, TextureColorType textureColor, TextureWrapType textureWrap, int mipLevel)
{
	if (_images.find(fileName) == _images.end())
	{
		//Utils::Logger::Instance()->Log("TextureManager::LoadFromFile: %s \n", fileName.c_str());

		Texture2* image = new Texture2();
		if (image->LoadFromFile(fileName, textureColor, filterType, textureWrap, mipLevel))
		{
			//generate texture
			RenderManager::Instance()->CreateTexture(image);

			//add to collection
			_images.insert(std::pair<std::string, Texture2*>(fileName, image));

			//Utils::Logger::Instance()->Log("TextureManager::LoadFromFile: Load success\n");

			return image;
		}
		else
		{
			//error
			//Utils::Logger::Instance()->Log("TextureManager::LoadFromFile: Load failure!\n");
			return 0;
		}
	}

	return _images[fileName];
}

Texture2* TextureManager::LoadFromMemory(std::string newName, unsigned char *data, std::size_t size, TextureFilerType filterType, TextureColorType textureColor, TextureWrapType textureWrap, int mipLevel)
{
	if (_images.find(newName) == _images.end())
	{
		//Utils::Logger::Instance()->Log("TextureManager::LoadFromMemory: %s \n", newName.c_str());

		Texture2* image = new Texture2();
		if (image->LoadFromMemory(newName, data, size, textureColor, filterType, textureWrap, mipLevel))
		{
			//generate texture
			RenderManager::Instance()->CreateTexture(image);

			//add to collection
			_images.insert(std::pair<std::string, Texture2*>(newName, image));

			//Utils::Logger::Instance()->Log("TextureManager::LoadFromMemory: Load success\n");

			return image;
		}
		else
		{
			//error
			//Utils::Logger::Instance()->Log("TextureManager::LoadFromMemory: Load failuer!\n");
			return 0;
		}
	}

	return _images[newName];
}

Texture2* TextureManager::CreateEmpty(std::string name, int width, int height, TextureFilerType filterType, TextureColorType textureColor, TextureWrapType textureWrap, int mipLevel)
{
	if (name.length() == 0)
	{
		name = GenerateName(16);
	}

	if (_images.find(name) == _images.end())
	{
		//Utils::Logger::Instance()->Log("TextureManager::CreateEmpty: %s \n", name.c_str());

		Texture2* image = new Texture2();

		image->_fileName = name;
		image->_width = width;
		image->_height = height;
		image->_textureWidth = width;
		image->_textureHeight = height;
		image->_filterType = filterType;
		image->_textureColor = textureColor;
		image->_mipLevel = mipLevel;

		//allocate memory to that texture
		MemoryManager::Instance()->AllocTexture(image);

		//add to collection
		_images.insert(std::pair<std::string, Texture2*>(name, image));

		//Utils::Logger::Instance()->Log("TextureManager::CreateEmpty: Load success!\n");

		return image;
	}

	return 0;
}

Texture2* TextureManager::Get(std::string fileName)
{
	if (_images.find(fileName) == _images.end())
	{
		return 0;
	}

	return _images[fileName];
}

void TextureManager::Add(Texture2* image)
{
	if (_images.find(image->_fileName) == _images.end())
	{
		//add to collection
		_images.insert(std::pair<std::string, Texture2*>(image->_fileName, image));
	}
}

void TextureManager::Remove(std::string fileName)
{
	std::map<std::string, Texture2*>::iterator it = _images.find(fileName);

	if (it == _images.end())
	{
		return;
	}

	//Utils::Logger::Instance()->Log("TextureManager::Remove: %s \n", fileName.c_str());

	delete _images[fileName];
	_images.erase(it);
}

void TextureManager::Remove(Texture2* image)
{
	if (image == nullptr)
	{
		return;
	}


	std::map<std::string, Texture2*>::iterator it = _images.find(image->_fileName);

	if (it == _images.end())
	{
		return;
	}

	//Utils::Logger::Instance()->Log("TextureManager::Remove: %s \n", it->first.c_str());

	_images.erase(it);

	delete image;
	image = nullptr;
}

void TextureManager::RemoveAll()
{
	std::map<std::string, Texture2*>::iterator iter;

	for (iter = _images.begin(); iter != _images.end(); ++iter)
	{
		//Utils::Logger::Instance()->Log("Delete texture: %s \n", iter->first.c_str());

		delete iter->second;
	}

	_images.clear();
}

std::string TextureManager::GenerateName(int len)
{
	std::string generated;
	generated.resize(len);

	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";

	for (int i = 0; i < len; ++i)
	{
		generated[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	generated[len] = 0;

	return generated;
}