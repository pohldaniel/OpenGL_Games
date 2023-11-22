#ifndef __textureH__
#define __textureH__

#include <vector>
#include <string>
#include "PerlinNoise.h"

class Texture{
	friend class Mesh;

public:
	Texture() = default;
	Texture(std::string pictureFile, const bool flipVertical = false, unsigned int format = -1);
	~Texture();

	unsigned int& getTexture();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getChannels();
	unsigned int getInternalFormat();
	unsigned int getFormat();
	unsigned int getType();

	void bind(unsigned int unit);
	//void loadFromFile(std::string pictureFile, const bool flipVertical = false, unsigned int internalFormat = 0, unsigned int _format = 0);
	void loadFromFile(std::string fileName, const bool flipVertical = false, unsigned int internalFormat = 0, unsigned int format = 0, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);

	void loadFromFile(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, unsigned int posY = 0, unsigned int posX = 0, const bool flipVertical = false, unsigned int _internalFormat = 0, unsigned int format = 0);
	void loadFromFile(std::string pictureFile, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0, unsigned int height = 0, const bool flipVertical = false, unsigned int _internalFormat = 0, unsigned int format = 0);
	void createNullTexture(unsigned int width, unsigned int height, unsigned int color = 255);
	void createPerlinTexture(unsigned int width, unsigned int height, unsigned int seed = 0);
	void createEmptyTexture(unsigned int width, unsigned int height, unsigned int internalFormat = 0, unsigned int format = 0, unsigned int type = 0);
	void addAlphaChannel(unsigned int value = 255);
	unsigned char* readPixel();

	void setRepeat();
	void setLinear();

	static void Unbind();
	static void CutSubimage(std::string fileIn, std::string fileOut, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0, unsigned int height = 0, const bool flipVertical = false);
	static void AddHorizontally(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool flipVertical = false);
	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	

	static unsigned char* AddRemoveBottomPadding(unsigned char* imageData, int width, int& height, int numCompontents, int padding);
	static unsigned char* AddRemoveTopPadding(unsigned char* imageData, int width, int& height, int numCompontents, int padding);
	static unsigned char* AddRemoveRightPadding(unsigned char* imageData, int& width, int height, int numCompontents, int padding);
	static unsigned char* AddRemoveLeftPadding(unsigned char* imageData, int& width, int height, int numCompontents, int padding);
	static unsigned char* LoadFromFile(std::string pictureFile, const bool flipVertical = false, short alphaChannel = -1);
	static unsigned char* LoadFromFile(std::string fileName, int& width, int& height, const bool _flipVertical = true, bool transparent = false, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);

	static void Safe(std::string fileOut, unsigned int& texture);
	static void Safe(std::string fileOut, unsigned char* bytes, unsigned int width, unsigned int height, unsigned int channels);

private:
	void flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	
	unsigned int m_texture = 0;
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	unsigned short m_channels = 0;
	unsigned int m_format = 0;
	unsigned int m_internalFormat = 0;
	unsigned int m_type = 0;
};

#endif