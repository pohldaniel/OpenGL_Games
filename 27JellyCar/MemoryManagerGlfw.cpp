#include "MemoryManagerGlfw.h"
#include "GraphicsEnums.h"

void MemoryManagerGlfw::AllocTexture(Texture2* texture) {
	int textureSize = texture->GetColorType() == TextureColorType::Texture_RGBA ? 4 : 3;

	unsigned char* data = new unsigned char[texture->GetWidth() * texture->GetHeight() * textureSize];
	texture->SetImageData(data);
}

void MemoryManagerGlfw::FreeTexture(Texture2* texture) {
	if (texture != nullptr){
		if (texture->GetImageData() != nullptr){
			delete[] texture->GetImageData();
			texture->SetImageData(nullptr);
		}
	}
}