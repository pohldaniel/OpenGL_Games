#pragma once

#include <string>

class WgpTexture {

public:

	WgpTexture();
	WgpTexture(WgpTexture const& rhs);
	WgpTexture(WgpTexture&& rhs) noexcept;

	void loadFromFile(std::string fileName, const bool flipVertical = false);

private:

	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
	unsigned char* data;

	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
};