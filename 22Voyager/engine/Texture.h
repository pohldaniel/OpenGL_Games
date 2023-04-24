#ifndef __textureH__
#define __textureH__

#include <vector>
#include <string>
#include "PerlinNoise.h"

class Texture{
	friend class Mesh;
	friend bool operator== (const Texture& t1, const Texture& t2);
public:
	Texture() = default;
	Texture(std::string pictureFile, const bool flipVertical = false, unsigned int internalFormat = 0, unsigned int format = 0);
	Texture(Texture const& rhs);
	~Texture();

	unsigned int& getTexture();
	unsigned int getWidth();
	unsigned int getHeight();
	unsigned int getChannels();
	unsigned int getInternalFormat();
	unsigned int getFormat();
	unsigned int getType();
	unsigned int getTarget();

	void bind(unsigned int unit) const;
	void loadFromFile(std::string fileName, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0, unsigned int SOIL_FLAG = 0u);
	void loadCrossHDRIFromFile(std::string fileName, const bool flipVertical = true, unsigned int internalFormat = 0u, unsigned int format = 0u, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	void loadHDRIFromFile(std::string fileName, const bool flipVertical = true, unsigned int internalFormat = 0u, unsigned int format = 0u, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	void loadCrossDDSFromFile(std::string fileName);
	void loadDDSRawFromFile(std::string fileName, const int knownInternal = NULL);
	void loadCubeFromFile(std::string* textureFiles, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u);

	void loadFromFile(std::string fileName, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0u, unsigned int posY = 0u, unsigned int posX = 0u, const bool flipVertical = false, unsigned int _internalFormat = 0u, unsigned int format = 0u);
	void loadFromFile(std::string fileName, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0u, unsigned int height = 0u, const bool flipVertical = false, unsigned int _internalFormat = 0, unsigned int format = 0u);
	void createNullTexture(unsigned int width, unsigned int height, unsigned int color = 255u);
	void createEmptyTexture(unsigned int width, unsigned int heightu, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u);
	void createPerlinNoise(unsigned int width, unsigned int height, unsigned int seed = 0u);
	void createNoise(unsigned int width, unsigned int height);
	void createNullCubemap(unsigned int width, unsigned int height, unsigned int color = 255);
	
	void addAlphaChannel(unsigned int value = 255u);
	unsigned char* readPixel();

	void setLinear(unsigned int mode = 9987u /*GL_LINEAR_MIPMAP_LINEAR*/);
	void setNearest();
	void setFilter(unsigned int minFilter, unsigned int magFilter = 0u);
	void setWrapMode(unsigned int mode = 10497);
	void setAnisotropy(float aniso);
	void cleanup();

	static void Unbind(unsigned int target = 3553u);
	static void CutSubimage(std::string fileIn, std::string fileOut, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0u, unsigned int height = 0u, const bool flipVertical = false);
	static void AddHorizontally(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool flipVertical = false);
	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	
	static unsigned char* AddRemoveBottomPadding(unsigned char* imageData, int width, int& height, int numCompontents, int padding);
	static unsigned char* AddRemoveTopPadding(unsigned char* imageData, int width, int& height, int numCompontents, int padding);
	static unsigned char* AddRemoveRightPadding(unsigned char* imageData, int& width, int height, int numCompontents, int padding);
	static unsigned char* AddRemoveLeftPadding(unsigned char* imageData, int& width, int height, int numCompontents, int padding);
	static unsigned char* LoadFromFile(std::string fileName, const bool flipVertical = false, short alphaChannel = -1);
	static unsigned char* LoadFromFile(std::string fileName, int& width, int& height, const bool flipVertical = true, bool transparent = false, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	static unsigned char* LoadHDRIFromFile(std::string fileName, int& width, int& height, const bool flipVertical = true, unsigned int internalFormat = 0, unsigned int format = 0, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);

	static void Safe(std::string fileOut, unsigned int& texture);
	static void Safe(std::string fileOut, unsigned char* bytes, unsigned int width, unsigned int height, unsigned int channels);

	static void CreateEmptyTexture(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0, unsigned int type = 0u);
	static void CreateEmptyCubemap(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u);
	static void CreateTexture3D(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int depth, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u, unsigned char* data = NULL);

	static void Resize(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u, unsigned int target = 3553u, unsigned int depth = 0u);

	static void SetFilter(unsigned int& textureRef, unsigned int minFilter, unsigned int magFilter = 0u, unsigned int target = 3553u);
	static void SetWrapMode(unsigned int& textureRef, unsigned int mode, unsigned int target = 3553u);

private:

	void flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	
	unsigned int m_texture = 0;
	unsigned int m_width = 0;
	unsigned int m_height = 0;
	unsigned int m_depth = 0;
	unsigned short m_channels = 0;
	unsigned int m_format = 0;
	unsigned int m_internalFormat = 0;
	unsigned int m_type = 0;
	unsigned int m_target;
};

#endif