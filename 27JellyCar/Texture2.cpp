#include <SOIL2/SOIL2.h>

#include "Texture2.h"
#include "RenderManager.h"
#include "MemoryManager.h"
#include "FileSystem/BaseFile.h"
#include "FileSystem/FileManager.h"

Texture2::Texture2() {
	_width = 0;
	_height = 0;
	_textureHeight = 0;
	_textureWidth = 0;
	_id = 0;
	_pixels = nullptr;
	_mipLevel = 0;

	_filterType = TextureFilerType::LinearFilter;
	_textureColor = TextureColorType::Texture_RGBA;
	_textureWrap = TextureWrapType::ClampToEdge;

	_gxmId[0] = 0;
	_gxmId[1] = 0;
	_gxmId[2] = 0;
	_gxmId[3] = 0;

	_fileName = "";
}

Texture2::~Texture2() {
	RenderManager::Instance()->DestroyTexture(this);

	if (_pixels != nullptr)
	{
		MemoryManager::Instance()->FreeTexture(this);
	}
}

bool Texture2::LoadFromFile(std::string fileName, TextureColorType textureColor, TextureFilerType filterType, TextureWrapType textureWrap, int mipLevel)
{
	_fileName = fileName;

	if (_pixels != 0)
	{
		delete[] _pixels;
	}

	BaseFile* file = FileManager::Instance()->GetFile(_fileName);

	if (file == 0)
		return false;

	file->Open(Read, Binary);

	int dataSize = 0;
	unsigned char* _buffer = file->GetData(dataSize);

	file->Close();
	delete file;

	return LoadFromMemory(fileName, _buffer, dataSize, textureColor, filterType, textureWrap, mipLevel);
}

bool Texture2::LoadFromMemory(std::string newName, unsigned char* data, std::size_t size, TextureColorType textureColor, TextureFilerType filterType, TextureWrapType textureWrap, int mipLevel)
{
	if (data && size)
	{
		_fileName = newName;

		if (_pixels != 0)
		{
			delete[] _pixels;
		}

		// Load the image and get a pointer to the pixels in memory
		int imgWidth, imgHeight, imgChannels;
		const unsigned char* buffer = static_cast<const unsigned char*>(data);
		unsigned char* ptr = SOIL_load_image_from_memory(buffer, static_cast<int>(size), &imgWidth, &imgHeight, &imgChannels, SOIL_LOAD_RGBA);

		if (ptr && imgWidth && imgHeight)
		{
			// Assign the image properties
			_width = imgWidth;
			_height = imgHeight;

			_filterType = filterType;
			_textureColor = textureColor;
			_textureWrap = textureWrap;

			_mipLevel = mipLevel;

			_textureWidth = _width;
			_textureHeight = _height;

			//check if texture size is power of 2
			bool test1 = IsPowerOfTwo(_width);
			bool test2 = IsPowerOfTwo(_height);

			/*if (!test1 || !test2)
			{
			int srcWidth = _width;
			int srcHeigh = _height;

			if (!test1)
			{
			_width = NextPowOf2(_width);
			}

			if (!test2)
			{
			_height = NextPowOf2(_height);
			}

			System::MemoryManager::Instance()->AllocTexture(this);


			int          pitch = srcWidth * 4;
			int          rows = srcHeigh;
			int          srcStride = srcWidth * 4;
			int          dstStride = _width * 4;
			unsigned char*	srcPixels = &ptr[0] + (0 + 0 * srcWidth) * 4;
			unsigned char*  dstPixels = &_pixels[0] + (0 + 0 * _width) * 4;

			// Optimized copy ignoring alpha values, row by row (faster)
			for (int i = 0; i < rows; ++i)
			{
			std::memcpy(dstPixels, srcPixels, pitch);
			srcPixels += srcStride;
			dstPixels += dstStride;
			}
			}
			else*/
			{
				MemoryManager::Instance()->AllocTexture(this);

				if (_pixels != 0)
				{
					memcpy(_pixels, ptr, _width * _height * 4);
				}
			}

			// Free the loaded pixels (they are now in our own pixel buffer)
			SOIL_free_image_data(ptr);
			delete[] data;

			return true;
		}
		else
		{
			return false;//error
		}
	}

	return false;
}

int Texture2::GetWidth()
{
	return _width;
}

int Texture2::GetHeight()
{
	return _height;
}

int Texture2::GetTextureWidth()
{
	return _textureWidth;
}

int Texture2::GetTextureHeight()
{
	return _textureHeight;
}

void Texture2::SetMipLevel(int mipLevel)
{
	_mipLevel = mipLevel;
}

int Texture2::GetMipLevel()
{
	return _mipLevel;
}

std::string Texture2::GetFileName()
{
	return _fileName;
}

bool Texture2::Resized()
{
	return _width != _textureWidth || _height != _textureHeight;
}

TextureFilerType Texture2::GetFilterType()
{
	return _filterType;
}

TextureColorType Texture2::GetColorType()
{
	return _textureColor;
}

TextureWrapType Texture2::GetWrapType()
{
	return _textureWrap;
}

unsigned int* Texture2::GetIdPointer()
{
	return &_id;
}

unsigned int Texture2::GetId()
{
	return _id;
}

unsigned char* Texture2::GetImageData()
{
	return _pixels;
}

void Texture2::SetImageData(void *data)
{
	_pixels = static_cast<unsigned char*>(data);
}

void Texture2::CopyImageData(void *data)
{
	memcpy(_pixels, data, _width * _height * 4);
}