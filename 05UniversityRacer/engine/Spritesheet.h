#ifndef __spritesheetH__
#define __spritesheetH__
#include <string>


class Spritesheet {

public:
	Spritesheet() = default;
	Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, bool reverse = false, bool flipVertical = false, int row = 0, int minColumn = 0, int maxColumn = -1, unsigned int format = -1);
	~Spritesheet();

	unsigned int getAtlas();
	unsigned short getTileCountX();
	unsigned short getTileCountY();
	unsigned short getTotalFrames();

	void createSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int format = -1);
	void addToSpritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, bool reverse = false, bool flipVertical = false, int row = 0, int minColumn = 0, int maxColumn = -1, unsigned int format = -1);
	void addToSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int format = -1);

private:
	unsigned int m_texture;


	unsigned short m_tileCountX;
	unsigned short m_tileCountY;
	unsigned short m_totalFrames;
};

#endif