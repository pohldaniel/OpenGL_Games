#pragma once
#include <vector>
#include <string>

class Cubemap {
	
public:
	Cubemap() = default;
	~Cubemap();

	unsigned int& getTexture();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getChannels();
	void bind(unsigned int unit);
	void loadFromFile(std::string* textureFiles, const bool flipVertical = false, unsigned int format = 0u);
	void createNullCubemap(unsigned int width, unsigned int height, unsigned int color = 255);
	void setRepeat();
	void setNearest();
	void setTexture(unsigned int texture);
	void setFilter(unsigned int mode);
	void setWrapMode(unsigned int mode);

	static void CreateEmptyCubemap(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u);
	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	static void Unbind();
	static void Resize(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u);

private:
	void flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);

	unsigned int m_texture = 0;
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	unsigned short m_channels = 0;
};