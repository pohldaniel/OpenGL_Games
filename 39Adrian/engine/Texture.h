#ifndef __textureH__
#define __textureH__

#include <vector>
#include <string>
#include <map>
#include <array>
#include "PerlinNoise.h"

class Texture{

	friend bool operator== (const Texture& t1, const Texture& t2);
	friend class Framebuffer;
	friend class Fontrenderer;

public:

	Texture() = default;
	Texture(std::string fileName, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0, unsigned int SOIL_FLAG = 0u);
	Texture(Texture const& rhs);
	Texture(Texture&& source);
	Texture &operator=(const Texture& rhs);
	~Texture();
	
	unsigned int& getTextureAccess();
	const unsigned int& getTexture() const;
	const unsigned int& getTextureHandle() const;
	const unsigned int& makeTextureHandleResident();
	unsigned int getWidth() const;
	unsigned int getHeight() const;
	unsigned int getChannels() const;
	unsigned int getInternalFormat() const;
	unsigned int getFormat() const;
	unsigned int getType() const;
	unsigned int getTarget() const;

	void setWidth(unsigned int width);
	void setHeight(unsigned int height);
	void setChannels(unsigned short channels);
	void setInternalFormat(unsigned int internalFormat);
	void setFormat(unsigned int format);
	void setType(unsigned int type);
	void setTarget(unsigned int target);

	void bind(unsigned int unit = 0u, bool forceBind = false) const;
	void unbind(unsigned int unit = 0u) const;

	void loadFromFile(std::string fileName, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0, unsigned int SOIL_FLAG = 0u);
	void loadFromFileCpu(std::string fileName, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0, unsigned int SOIL_FLAG = 0u);
	void loadFromFileGpu();
	
	void loadFromFile(std::string fileName, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0u, unsigned int posY = 0u, unsigned int posX = 0u, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u);
	void loadFromFileCpu(std::string fileName, unsigned short tileWidth, unsigned short tileHeight, unsigned short spacing = 0u, unsigned int posY = 0u, unsigned int posX = 0u, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u);
	void loadFromFile(std::string fileName, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0u, unsigned int height = 0u, const bool flipVertical = false, unsigned int internalFormat = 0, unsigned int format = 0u);
	void loadFromFileCpu(std::string fileName, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0u, unsigned int height = 0u, const bool flipVertical = false, unsigned int internalFormat = 0, unsigned int format = 0u);
	void loadFromFileGpu(unsigned short width, unsigned short height);

	void loadHDRIFromFile(std::string fileName, const bool flipVertical = true, unsigned int internalFormat = 0u, unsigned int format = 0u);
	void loadHDRIFromFileCpu(std::string fileName, const bool flipVertical = true, unsigned int internalFormat = 0u, unsigned int format = 0u);
	void loadHDRIFromFileGpu();

	void loadCrossCubeFromFile(std::string fileName, const bool _flipVertical = false, unsigned int _internalFormat = 0u, unsigned int _format = 0u);
	void loadCrossCubeFromFileCpu(std::string fileName, const bool _flipVertical = false, unsigned int _internalFormat = 0u, unsigned int _format = 0u);
	void loadCrossCubeFromFileGpu();

	//todo: split up in cpu gpu
	void loadCrossHDRIFromFile(std::string fileName, const bool flipVertical = true, unsigned int internalFormat = 0u, unsigned int format = 0u, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	void loadCubeFromFile(std::string* textureFiles, const bool flipVertical = false, unsigned int internalFormat = 0u, unsigned int format = 0u);
	
	
	void createNullTexture(unsigned int width, unsigned int height, unsigned int color = 255u);
	void createEmptyTexture(unsigned int width, unsigned int heightu, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u);
	void createTexture3D(unsigned int width, unsigned int height, unsigned int depth, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u, unsigned char* data = NULL);
	void createPerlinNoise(unsigned int width, unsigned int height, unsigned int seed = 0u);
	void createNoise(unsigned int width, unsigned int height);
	void createNullCubemap(unsigned int width, unsigned int height, unsigned int color = 255);

	void loadCrossDDSFromFile(std::string fileName);
	void loadDDSRawFromFile(std::string fileName, const int knownInternal = 0);

	void copy(const Texture& rhs);

	void addAlphaChannel(unsigned int value = 255u);
	unsigned char* readPixel();

	void setLinear(unsigned int mode = 9987u /*GL_LINEAR_MIPMAP_LINEAR*/) const;
	void setNearest() const;
	void setFilter(unsigned int minFilter, unsigned int magFilter = 0u) const;
	void setWrapMode(unsigned int mode = 10497 /*GL_REPEAT*/) const;
	void setAnisotropy(float aniso) const;
	void setCompareFunc(unsigned int func = 0u) const;
	void cleanup();
	void setDeepCopy(bool deepCopy);
	void markForDelete();
	void generateDisplacements();
	void generateNormals();
	void flipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	void flipHorizontal(unsigned char* data, unsigned int width, unsigned int height, int numComponents);

	static void Bind(unsigned int textureRef, unsigned int unit = 0u, unsigned int target = 3553u, bool forceBind = false);
	static void Unbind(unsigned int unit = 0u, unsigned int target = 3553u);
	static void SetActivateTexture(unsigned int unit = 0u);

	static void CutSubimage(std::string fileIn, std::string fileOut, unsigned int offsetX, unsigned int offsetY, unsigned int width = 0u, unsigned int height = 0u, const bool flipVertical = false);
	static void AddHorizontally(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool flipVertical = false);
	static void AddVertically(std::string fileIn1, std::string fileIn2, std::string fileOut, const bool flipVertical = false);

	static void FlipVertical(unsigned char* data, unsigned int padWidth, unsigned int height);
	static void FlipHorizontal(unsigned char* data, unsigned int width, unsigned int height, int numComponents);

	static void AddHighlight(std::string fileIn, std::string fileOut, int borderWidth);
	static void GrayScale(std::string fileIn, std::string fileOut);
	static void AddRemoveBottomPadding(std::string fileIn, std::string fileOut, int padding);
	static void AddRemoveTopPadding(std::string fileIn, std::string fileOut, int padding);
	static void AddRemoveRightPadding(std::string fileIn, std::string fileOut, int padding);
	static void AddRemoveLeftPadding(std::string fileIn, std::string fileOut, int padding);

	static unsigned char* AddRemoveBottomPadding(unsigned char* imageData, int width, int& height, int numComponents, int padding);
	static unsigned char* AddRemoveTopPadding(unsigned char* imageData, int width, int& height, int numComponents, int padding);
	static unsigned char* AddRemoveRightPadding(unsigned char* imageData, int& width, int height, int numComponents, int padding);
	static unsigned char* AddRemoveLeftPadding(unsigned char* imageData, int& width, int height, int numComponents, int padding);
	static unsigned char* LoadFromFile(std::string fileName, const bool flipVertical = false, short alphaChannel = -1);
	static unsigned char* LoadFromFile(std::string fileName, int& width, int& height, const bool flipVertical = true, bool transparent = false, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);
	static unsigned char* LoadFromFile(std::string fileName, unsigned int posX, unsigned int posY, unsigned int width, unsigned int height, const bool flipVertical = true, bool transparent = false, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);

	static unsigned char* LoadHDRIFromFile(std::string fileName, int& width, int& height, const bool flipVertical = true, unsigned int internalFormat = 0, unsigned int format = 0, int paddingLeft = 0, int paddingRight = 0, int paddingTop = 0, int paddingBottom = 0);

	static void Safe(std::string fileOut, const unsigned int& texture);
	static void Safe(std::string fileOut, unsigned char* bytes, unsigned int width, unsigned int height, unsigned int channels);
	static void PremultiplyAlpha(std::string fileIn, std::string fileOut, const bool flipVertical = false);

	static void CreateTexture2D(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0, unsigned int type = 0u);
	static void CreateEmptyCubemap(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u);
	static void CreateTexture3D(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int depth, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u, unsigned char* data = NULL);
	static void CreateTextureArray(unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int depth, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u, unsigned char* data = NULL);
	static void CreateTextureCube(unsigned int& textureRef, unsigned int size, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u, unsigned char* data = NULL);

	static void Resize(const unsigned int& textureRef, unsigned int width, unsigned int height, unsigned int internalFormat = 0u, unsigned int format = 0u, unsigned int type = 0u, unsigned int target = 3553u, unsigned int depth = 0u);

	static void SetFilter(const unsigned int& textureRef, unsigned int minFilter, unsigned int magFilter = 0u, unsigned int target = 3553u);
	static void SetWrapMode(const unsigned int& textureRef, unsigned int mode, unsigned int target = 3553u);
	static void ArrayTo3D(const unsigned int& textureRef1, unsigned int& textureRef2);
	static void SetCompareFunc(const unsigned int& textureRef, unsigned int func = 0u, unsigned int target = 3553u);
	static void SetLinear(const unsigned int& textureRef, unsigned int mode = 9987u /*GL_LINEAR_MIPMAP_LINEAR*/, unsigned int target = 3553u);
	static void CreateBars(std::string fileOut, unsigned int width, unsigned int height, unsigned int numBars, unsigned int heightBar);

private:

	static std::array<float, 3> Cross(const std::array<float, 3>& p, const std::array<float, 3>& q);
	static std::array<float, 3> Normalize(const std::array<float, 3>& p);

	unsigned int m_texture = 0u;
	unsigned int m_width = 0u;
	unsigned int m_height = 0u;
	unsigned int m_depth = 0u;
	unsigned short m_channels = 0u;
	unsigned int m_format = 0u;
	unsigned int m_internalFormat = 0u;
	unsigned int m_type = 0u;
	unsigned int m_target;
	unsigned int m_textureHandle = 0u;
	bool m_deepCopy = false;
	bool m_markForDelete = false;
	unsigned char* imageData = nullptr;
	unsigned char* subImage = nullptr;
	std::vector<unsigned char*> facData;

	static std::map<unsigned int, unsigned int> ActiveTextures;
};
#endif