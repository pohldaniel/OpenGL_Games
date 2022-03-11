#ifndef __textureH__
#define __textureH__

#include <string>

class Texture{

public:
	Texture() = default;
	Texture(std::string pictureFile, const bool flipVertical = false);
	~Texture();

	unsigned int getTexture();
	void loadFromFile(std::string pictureFile, const bool flipVertical = false, const bool linear = false);
private:
	unsigned int m_texture;

};

#endif