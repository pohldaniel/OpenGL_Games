#ifndef __spritesheetH__
#define __spritesheetH__

#include <string>

class Spritesheet {

public:
	Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse = false, bool flipVertical = false);
	unsigned int getAtlas();
private:
	unsigned int m_texture;
};

#endif