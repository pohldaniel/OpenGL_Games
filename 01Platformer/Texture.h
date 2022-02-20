#ifndef __textureH__
#define __textureH__

#include <string>

class Texture{

public:
	Texture(std::string pictureFile, bool flipVertical = false);
	~Texture();

	unsigned int getTexture();

private:
	unsigned int m_texture;

};

#endif