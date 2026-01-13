#pragma once
#include <string>
#include <webgpu.h>

class WgpTexture {

public:

	WgpTexture();
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;

	void loadFromFile(std::string fileName, const bool flipVertical = false);
	void copyToDestination(const WGPUTexture& destTesture);

	static unsigned char* LoadFromFile(std::string fileName, const bool flipVertical = false, short alphaChannel = -1);

private:

	WGPUTexture m_texture;

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	unsigned char* m_data;

	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
};