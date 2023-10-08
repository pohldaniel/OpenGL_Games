#ifndef __spritesheetH__
#define __spritesheetH__
#include <string>

class Spritesheet {

public:
	Spritesheet() = default;
	Spritesheet(unsigned int& textureAtlas);
	Spritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, bool reverse = false, bool flipVertical = false, int row = 0, int minColumn = 0, int maxColumn = -1, unsigned int format = 0);

	~Spritesheet();

	unsigned int getAtlas();
	unsigned short getTileCountX();
	unsigned short getTileCountY();
	unsigned short getTotalFrames();

	void setAtlas(unsigned int texture);

	void createSpritesheet(unsigned int texture, unsigned int width, unsigned int height, unsigned int format = 0);
	void createSpritesheetFromTexture(unsigned int texture, unsigned int format = 0, unsigned int internalFormat = 0, int unpackAlignment = 4);
	void addToSpritesheet(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0, bool reverse = false, bool flipVertical = false, int row = 0, int minColumn = 0, int maxColumn = -1, unsigned int format = 0);

	void createSpritesheet(std::string fileName, unsigned int format = 0, unsigned int internalFormat = 0, bool flipVertical = true, int unpackAlignment = 4);
	void addToSpritesheet(std::string fileName, unsigned int format = 0, unsigned int internalFormat = 0, bool flipVertical = true, int unpackAlignment = 4);
	void addToSpritesheet(unsigned int texture, unsigned int format = 0, unsigned int internalFormat = 0, int unpackAlignment = 4);
	void addToSpritesheet(unsigned char* bytes, unsigned int width, unsigned int height, unsigned int format = 0);

	void createNullSpritesheet(unsigned int width, unsigned int height, unsigned short layer);
	void createEmptySpritesheet(unsigned int width, unsigned int height, unsigned int format = 0);

	void setRepeat();
	void setLinear();
	void setLinearMipMap();

	void bind(unsigned int unit = 0u) const;
	void unbind(unsigned int unit = 0u) const;
	void safe(std::string name);

	static void Bind(const unsigned int& textureRef, unsigned int unit = 0u);
	static void Unbind(const unsigned int unit = 0u);
	static void Safe(std::string name, unsigned int textureAtlas);
	static unsigned int Merge(const unsigned int& atlas1, const unsigned int& atlas2, bool deleteAtlas1 = true, bool deleteAtlas2 = true);
	static unsigned int CreateSpritesheet(unsigned char* bytes, unsigned int width, unsigned int height, unsigned int layer);
	static void CreateSpritesheet(unsigned char* bytes, unsigned int width, unsigned int height, unsigned int layer, unsigned int& textureRef);

private:

	unsigned int m_texture;
	unsigned short m_tileCountX = 0;
	unsigned short m_tileCountY = 0;
	unsigned short m_totalFrames = 0;
};

#endif