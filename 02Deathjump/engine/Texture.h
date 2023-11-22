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
private:
	unsigned int m_texture;

};

#endif