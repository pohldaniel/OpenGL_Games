#pragma once
#include <string>
#include <filesystem>
#include <webgpu.h>

class WgpTexture {

public:

	WgpTexture();
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;
	~WgpTexture();

	void loadFromFile(std::string fileName, const bool flipVertical = false, const short alphaChannel = -1);
	void loadFromMemory(unsigned char* data, uint32_t size, const bool flipVertical = false, const short alphaChannel = -1);
	void createEmpty(uint32_t width, uint32_t height, WGPUTextureUsage textureUsage, WGPUTextureFormat textureFormat);
	void cleanup();
	void markForDelete();

	const WGPUTexture& getTexture() const;
	const WGPUTextureView& getTextureView() const;

	const unsigned int getWidth() const;
	const unsigned int getHeight() const;
	const WGPUTextureFormat getFormat() const;

	static unsigned char* LoadFromFile(std::string fileName, const bool flipVertical = false, const short alphaChannel = -1);
	static unsigned char* LoadFromFile(std::string fileName, uint32_t& width, uint32_t& height, const bool flipVertical = false, const short alphaChannel = -1);
	static unsigned char* LoadFromMemory(unsigned char* data, uint32_t size, uint32_t& width, uint32_t& height, const bool flipVertical = false, const short alphaChannel = -1);
	static void Safe(std::string fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels);

private:

	WGPUTexture m_texture;
	WGPUTextureFormat m_format;
	WGPUTextureView m_textureView;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	bool m_markForDelete;
};