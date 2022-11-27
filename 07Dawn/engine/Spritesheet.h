#ifndef __spritesheetH__
#define __spritesheetH__
#include <string>


class Spritesheet {

public:
	Spritesheet() = default;
	Spritesheet(unsigned int textureAtlas);
	Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, bool reverse = false, bool flipVertical = false, int row = 0, int minColumn = 0, int maxColumn = -1, unsigned int format = 0);
	
	~Spritesheet();

	unsigned int getAtlas();
	unsigned short getTileCountX();
	unsigned short getTileCountY();
	unsigned short getTotalFrames();

	void createSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int format = 0);
	void createSpritesheetFromTexture(unsigned int texture, unsigned int format = 0, unsigned int internalFormat = 0, int _unpackAlignment = 4);
	void addToSpritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, bool reverse = false, bool flipVertical = false, int row = 0, int minColumn = 0, int maxColumn = -1, unsigned int format = 0);
	
	void addToSpritesheet(unsigned int texture, unsigned int format = 0, unsigned int internalFormat = 0, int _unpackAlignment = 4);
	void addToSpritesheet(unsigned char* bytes, unsigned int width, unsigned int height, unsigned int format = 0);
	void createNullSpritesheet(unsigned int width, unsigned int height, unsigned short layer);
	
	void setRepeat();
	void setLinear();
	void setLinearMipMap();

	void bind(unsigned int unit);
	void safe(std::string name);

	static void Unbind();
	static void Safe(std::string name, unsigned int textureAtlas);
	static unsigned int Merge(unsigned int& atlas1, unsigned int& atlas2, bool deleteAtlas1 = true, bool deleteAtlas2 = true);
private:

	unsigned int m_texture;
	unsigned short m_tileCountX = 0;
	unsigned short m_tileCountY = 0;
	unsigned short m_totalFrames = 0;
};

#endif