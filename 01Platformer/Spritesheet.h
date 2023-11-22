#ifndef __spritesheetH__
#define __spritesheetH__

#include <string>

class Spritesheet {

public:
	Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse = false, bool flipVertical = false);
	~Spritesheet();
	
	unsigned int getAtlas();
	unsigned short getTileCountX();
	unsigned short getTileCountY();

private:
	unsigned int m_texture;

	unsigned short tileCountX;
	unsigned short tileCountY;
};

#endif