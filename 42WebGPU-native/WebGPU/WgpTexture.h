#pragma once
#include <string>
#include <filesystem>
#include <array>
#include <webgpu.h>

struct FIBITMAP;
class WgpTexture {

public:

	WgpTexture();
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;
	~WgpTexture();

	void loadFromFile(const std::string& fileName, const bool flipVertical = false, const short alphaChannel = -1);
	void loadFromMemory(unsigned char* data, uint32_t size, const bool flipVertical = false, const short alphaChannel = -1);
	void loadHDRICubeFromFile(const std::string& fileName, const bool flipVertical = false, const bool halfBPP = false);
	void loadHDRIFromFile(const std::string& fileName, const bool flipVertical = false, const bool halfBPP = false);
	void loadCubeFromFiles(std::string* fileNames, const bool flipVertical = false);

	void createEmpty(uint32_t width, uint32_t height, uint32_t depth, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat, uint32_t mipLevelCount = 1u);
	void cleanup();
	void markForDelete();
	void setTextureUsage(WGPUTextureUsage textureUsage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst);

	const WGPUTexture& getTexture() const;
	const WGPUTextureView& getTextureView() const;

	const unsigned int getWidth() const;
	const unsigned int getHeight() const;
	const WGPUTextureFormat getFormat() const;
	const uint32_t getMipLevelCount() const;

	const unsigned int& width() const;
	const unsigned int& height() const;

	static unsigned char* LoadFromFile(std::string fileName, const bool flipVertical = false, const short alphaChannel = -1);
	static unsigned char* LoadFromFile(std::string fileName, uint32_t& width, uint32_t& height, const bool flipVertical = false, const short alphaChannel = -1);
	static unsigned char* LoadFromMemory(unsigned char* data, uint32_t size, uint32_t& width, uint32_t& height, const bool flipVertical = false, const short alphaChannel = -1);
	
	static void Safe(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels);
	static void Safe(const std::string& fileOut, unsigned char* bytes, uint32_t size);
	static void SafeHDRI(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels);

private:
	
	static FIBITMAP* AddAlphaChannel(FIBITMAP* bitmap, const short alphaChannel = -1);
	static FIBITMAP* RemoveAlphaChannel(FIBITMAP* bitmap);
	static void FlipVertical(unsigned char* data, uint32_t width, uint32_t bytesPerChannel, uint32_t height);
	static void FlipHorizontal(unsigned char* data, uint32_t width, uint32_t bytesPerChannel, uint32_t height);
	static void Rotate90(unsigned char*& sourceInOut, uint32_t width, uint32_t bytesPerChannel, uint32_t height, bool ccw = false);
	static std::vector<unsigned char*> CrossToFaces(unsigned char* source, uint32_t width, uint32_t bytesPerChannel, uint32_t height);
	static uint32_t BitWidth(uint32_t m);

	WGPUTexture m_texture;
	WGPUTextureFormat m_format;
	WGPUTextureView m_textureView;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	bool m_markForDelete;
	WGPUTextureUsage m_textureUsage;
};

