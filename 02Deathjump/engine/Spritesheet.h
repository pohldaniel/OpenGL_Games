#ifndef __spritesheetH__
#define __spritesheetH__
#include <string>


class Spritesheet {

public:
	Spritesheet() = default;
	Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse = false, bool flipVertical = false, int row = 0, int maxColumn = -1, unsigned int format = -1);
	~Spritesheet();

	unsigned int getAtlas();
	unsigned short getTileCountX();
	unsigned short getTileCountY();
	unsigned short getTotalFrames();

private:
	unsigned int m_texture;

	unsigned short m_tileCountX;
	unsigned short m_tileCountY;
	unsigned short m_totalFrames;
};

#endif