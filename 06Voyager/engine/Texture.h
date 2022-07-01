#ifndef __textureH__
#define __textureH__

#include <vector>
#include <string>

class Texture{
	friend class Mesh;

public:
	Texture() = default;
	Texture(std::string pictureFile, const bool flipVertical = false, unsigned int format = -1);
	~Texture();

	unsigned int getTexture();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getChannels();
	void bind(unsigned int unit);
	void loadFromFile(std::string pictureFile, const bool flipVertical = false, unsigned int format = -1);
	void loadFromFile(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, unsigned int posY = 0, unsigned int posX = 0, const bool flipVertical = false, unsigned int format = -1);
	void loadFromFile(std::string pictureFile, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0, unsigned int height = 0, const bool flipVertical = false, unsigned int format = -1);
	void createNullTexture(unsigned int width, unsigned int height);
	
	void setRepeat();
	void setLinear();

	static void CutSubimage(std::string fileIn, std::string fileOut, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0, unsigned int height = 0, const bool flipVertical = false);
	static void AddHorizontally(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool flipVertical = false);
	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	static void Unbind();
private:
	void flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	
	unsigned int m_texture;
	unsigned int m_width;
	unsigned int m_height;
	unsigned short m_channels;
};

#endif