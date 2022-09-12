#include "TextureManager.h"
#include "ViewPort.h"

TextureCache TextureCache::s_instance;
TextureAtlasCreator TextureAtlasCreator::s_instance;

DawnTexture& TextureCache::getTextureFromCache(std::string filename){
	if (textures.count(filename) > 0) {
		return textures[filename];
	}

	Texture tex;
	tex.loadFromFile(filename, true);

	textures[filename].width = tex.getWidth();
	textures[filename].height = tex.getHeight();
	textures[filename].textureOffsetX = 0.0f;
	textures[filename].textureWidth = 1.0f;
	textures[filename].textureOffsetY = 0.0f;
	textures[filename].textureHeight = 1.0f;

	unsigned char* bytes = tex.readPixel();
	TextureAtlasCreator::get().addTexture(textures[filename], reinterpret_cast<char*>(bytes), tex.getWidth(), tex.getHeight());
	free(bytes);
	return textures[filename];
}

void TextureManager::DrawTextureBatched(DawnTexture& stexture, int x, int y, float transparency, float red, float green, float blue, float x_scale, float y_scale) {
	if (!TextureManager::IsRectOnScreen(x, stexture.width * x_scale, y, stexture.height * y_scale)) {
		return;
	}
	//glColor4f(red, green, blue, transparency);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	Batchrenderer::get().addQuad(Vector4f(x, y, stexture.width, stexture.height), Vector4f(stexture.textureOffsetX, stexture.textureOffsetY, stexture.textureWidth, stexture.textureHeight), stexture.frame);
}

void TextureManager::DrawTextureInstanced(DawnTexture& stexture, int x, int y, float transparency, float red, float green, float blue, float x_scale, float y_scale) {
	if (!TextureManager::IsRectOnScreen(x, stexture.width * x_scale, y, stexture.height * y_scale)) {
		return;
	}

	Instancedrenderer::get().addQuad(Vector4f(x, y, stexture.width, stexture.height), Vector4f(stexture.textureOffsetX, stexture.textureOffsetY, stexture.textureWidth, stexture.textureHeight), stexture.frame);
}

DawnTexture& TextureManager::Loadimage(std::string file, bool isOpenGLThreadInThreadedMode, int textureOffsetX, int textureOffsetY) {
	return TextureCache::get().getTextureFromCache(file);
}

bool TextureManager::IsRectOnScreen(int left, int width, int bottom, int height){
	ViewPort& viewPort = ViewPort::get();
	float _left = viewPort.getLeft();
	float _right = viewPort.getRight();
	float _bottom = viewPort.getBottom();
	float _top = viewPort.getTop();

	if (left >= (_right) ||   								// object right of screen
		bottom >= (_top) ||									// object above screen
		(left + width) <= _left ||                        	// object left of screen
		(bottom + height) <= _bottom)                      	// object below screen
	{
		return false;
	}
	return true;
}
