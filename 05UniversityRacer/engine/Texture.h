#ifndef __textureH__
#define __textureH__

#include <string>

class Texture{

public:
	Texture() = default;
	Texture(std::string pictureFile, const bool flipVertical = false, unsigned int format = -1);
	~Texture();

	unsigned int getTexture();
	void loadFromFile(std::string pictureFile, const bool flipVertical = false, const bool linear = false, unsigned int format = -1);
	void loadFromFile(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, unsigned int row = 0, unsigned int colummn = 0, const bool flipVertical = false, bool reverse = false, unsigned int format = -1, const bool linear = false);


	void loadFromFile(std::string pictureFile, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0, unsigned int height = 0, const bool flipVertical = false, unsigned int format = -1, const bool linear = false);

	static void CutSubimage(std::string fileIn, std::string fileOut, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0, unsigned int height = 0, const bool flipVertical = false);
	static void AddHorizontally(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool flipVertical = false);
private:
	unsigned int m_texture;

};

#endif