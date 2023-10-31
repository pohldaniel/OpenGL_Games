#ifndef TextureAtlas_H
#define TextureAtlas_H

#include "../TextureManager.h"

#include <texture-atlas.h>
using namespace ftgl;

class TextureAtlas{

private:

	int _width;
	int _height;

	texture_atlas_t* _atlas;

	Texture2* _texture;

	unsigned char ComputeY(int r, int g, int b);
	unsigned char* Convert(unsigned char *data, int img_n, int req_comp, int x, int y);

public:

	TextureAtlas(int width, int height);
	~TextureAtlas();

	void SaveTextur(std::string fileName);
	void LoadTexture(std::string fileName);

	void SetTexture(Texture2* texture);

	texture_atlas_t* GetAtlas();

	Texture2* CreateTexture();
	Texture2* GetTexture();
};
#endif
