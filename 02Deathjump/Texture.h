#ifndef __textureH__
#define __textureH__

#include <string>

class Texture{

public:
	Texture(){}
	Texture(std::string pictureFile, bool flipVertical = false);
	~Texture();

	unsigned int getTexture();
	void loadFromFile(std::string pictureFile, bool flipVertical = false);
private:
	unsigned int m_texture;

};

#endif