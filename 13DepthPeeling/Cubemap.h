#pragma once

#include <vector>
#include <string>

class Cubemap {
	
public:
	Cubemap() = default;
	~Cubemap();

	unsigned int getCubemap();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getChannels();
	void bind(unsigned int unit);
	void loadFromFile(std::string* textureFiles, const bool flipVertical = false, unsigned int format = -1);
	void createNullCubemap(unsigned int width, unsigned int height, unsigned int color = 255);
	void setRepeat();
	void setNearest();

	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	static void Unbind();
private:
	void flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);

	unsigned int m_texture = 0;
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	unsigned short m_channels = 0;
};