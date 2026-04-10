#pragma once
#include <string>
#include <filesystem>
#include <array>
#include <webgpu.h>

class WgpTexture {

	union UFloat {
		float flt;
		unsigned char c[4];
	};

public:

	WgpTexture();
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;
	~WgpTexture();

	void loadFromFile(const std::string& fileName, const bool flipVertical = false, const short alphaChannel = -1);
	void loadFromMemory(unsigned char* data, uint32_t size, const bool flipVertical = false, const short alphaChannel = -1);
	void loadHDRIFromFile(const std::string& fileName, const bool flipVertical = false);

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
	static void Safe(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels);
	static void Safe(const std::string& fileOut, unsigned char* bytes, uint32_t size);
	static void SafeHDRI(const std::string& fileOut, unsigned char* bytes, uint32_t width, uint32_t height, uint32_t channels);

private:

	std::array<float, 3> outImgToXYZ(int i, int j, int face, float edge);
	unsigned char* convertBack(unsigned char* source, uint32_t width, uint32_t height);
	unsigned char* convertBackHDRI(unsigned char* source, uint32_t width, uint32_t height);
	float bytesToFloatLE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);
	float bytesToFloatBE(unsigned char b0, unsigned char b1, unsigned char b2, unsigned char b3);

	WGPUTexture m_texture;
	WGPUTextureFormat m_format;
	WGPUTextureView m_textureView;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	bool m_markForDelete;
};