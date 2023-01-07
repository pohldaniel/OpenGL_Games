#ifndef __spritesheet_oldH__
#define __spritesheet_oldH__

#include <vector>
#include <string>

#include "Vector.h"

class SpritesheetOld {

public:
	SpritesheetOld(std::string pictureFile, unsigned short tileWidth, unsigned short tileHeight, bool reverse = false, bool flipVertical = false);
	~SpritesheetOld();
	
	unsigned int getTexture();
	const Matrix4f &getFrameTransform(unsigned short index) const;

	unsigned short getTileCountX();
	unsigned short getTileCountY();

private:
	Matrix4f calcTileFrameTransform(unsigned short id, float tileScaleX, float tileScaleY, unsigned short tileCountX);

	unsigned int m_texture;
	std::vector<Matrix4f> tileFrameTransforms;

	unsigned short tileCountX;
	unsigned short tileCountY;
};

#endif