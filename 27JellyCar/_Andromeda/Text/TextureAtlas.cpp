#include <SOIL2/SOIL2.h>
#include "TextureAtlas.h"
#include "../TextureManager.h"
#include "../RenderManager.h"

TextureAtlas::TextureAtlas(int width, int height){
			_width = width;
			_height = height;

			_texture = 0;

			_atlas = texture_atlas_new(width, height, 1);
}

TextureAtlas::~TextureAtlas(){
			//remove atlas
			texture_atlas_delete(_atlas);

			//remove texture
			if (_texture != 0)
			{
				TextureManager::Instance()->Remove(_texture);
			}
}

void TextureAtlas::SaveTextur(std::string fileName){
            ////convert data to rgba
            unsigned char* newData = Convert(_atlas->data, 1, 4, _width, _height);

            ////save image
			SOIL_save_image(fileName.c_str(), SOIL_SAVE_TYPE_PNG, _width, _height, 4, newData);

            ////dalete data
            delete[] newData;
}

void TextureAtlas::LoadTexture(std::string fileName){
            _texture = TextureManager::Instance()->LoadFromFile(fileName);
}

void TextureAtlas::SetTexture(Texture2* texture){
            _texture = texture;
}

texture_atlas_t* TextureAtlas::GetAtlas(){
			return _atlas;
}

Texture2* TextureAtlas::CreateTexture(){
	if (_texture == 0){

		_texture = TextureManager::Instance()->CreateEmpty("", _width, _height, TextureFilerType::LinearFilter);

		//convert data to rgba
		unsigned char* newData = Convert(_atlas->data, 1, 4, _width, _height);

		//set image data
		_texture->CopyImageData(newData);

		//delete converted data
		delete[] newData;

		//save test image
		//stbi_write_png("atlas.png", _width, _height, 4, _texture->GetImageData(), _width * 4);

		RenderManager::Instance()->CreateTexture(_texture);
	}



	return _texture;
}

Texture2* TextureAtlas::GetTexture(){
	return _texture;
}

unsigned char TextureAtlas::ComputeY(int r, int g, int b){
	return (unsigned char)(((r * 77) + (g * 150) + (29 * b)) >> 8);
}

unsigned char* TextureAtlas::Convert(unsigned char *data, int img_n, int req_comp, int x, int y){
	int i, j;
	unsigned char *good;

	if (req_comp == img_n) return data;

	good = new unsigned char[req_comp * x * y];//(unsigned char *) malloc(req_comp * x * y);

	for (j = 0; j < (int)y; ++j){

		unsigned char *src = data + j * x * img_n;
		unsigned char *dest = good + j * x * req_comp;

#define COMBO(a,b)  ((a)*8+(b))
#define CASE(a,b)   case COMBO(a,b): for(i=x-1; i >= 0; --i, src += a, dest += b)
				// convert source image with img_n components to one with req_comp components;
				// avoid switch per pixel, so use switch per scanline and massive macros
		switch (COMBO(img_n, req_comp))
		{
					CASE(1, 2) dest[0] = src[0], dest[1] = 255; break;
					CASE(1, 3) dest[0] = dest[1] = dest[2] = src[0]; break;
					CASE(1, 4)
					{
						dest[0] = dest[1] = dest[2] = 255;
						dest[3] = src[0];
					}
					break;
					CASE(2, 1) dest[0] = src[0]; break;
					CASE(2, 3) dest[0] = dest[1] = dest[2] = src[0]; break;
					CASE(2, 4) dest[0] = dest[1] = dest[2] = src[0], dest[3] = src[1]; break;
					CASE(3, 4) dest[0] = src[0], dest[1] = src[1], dest[2] = src[2], dest[3] = 255; break;
					CASE(3, 1) dest[0] = ComputeY(src[0], src[1], src[2]); break;
					CASE(3, 2) dest[0] = ComputeY(src[0], src[1], src[2]), dest[1] = 255; break;
					CASE(4, 1) dest[0] = ComputeY(src[0], src[1], src[2]); break;
					CASE(4, 2) dest[0] = ComputeY(src[0], src[1], src[2]), dest[1] = src[3]; break;
					CASE(4, 3) dest[0] = src[0], dest[1] = src[1], dest[2] = src[2]; break;

				}
#undef CASE
	}

	//free(data);
	//data = NULL;

	return good;
}
