#include "TextureManager.h"
#include "ViewPort.h"
#include "Constants.h"

TextureCache TextureCache::s_instance;
TextureAtlasCreator TextureAtlasCreator::s_instance;

DawnTexture& TextureCache::getTextureFromCache(std::string filename){

	if (textures.count(filename) > 0) {
		return textures[filename];
	}
	
	Texture tex = Texture();
	tex.loadFromFile(filename, true);

	if (tex.getChannels() == 3) {
		tex.addAlphaChannel();
	}

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

void TextureManager::DrawTextureBatched(DawnTexture& stexture, int x, int y, float transparency, float red, float green, float blue, float x_scale, float y_scale, bool checkVieport) {
	if (!TextureManager::IsRectOnScreen(x, stexture.width * x_scale, y, stexture.height * y_scale) && checkVieport) {
		return;
	}

	//glColor4f(red, green, blue, transparency);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	Batchrenderer::get().addQuad(Vector4f(x, y, stexture.width, stexture.height), Vector4f(stexture.textureOffsetX, stexture.textureOffsetY, stexture.textureWidth, stexture.textureHeight), stexture.frame);
}

void TextureManager::DrawTextureInstanced(DawnTexture& stexture, int x, int y, float transparency, float red, float green, float blue, float x_scale, float y_scale, bool checkVieport) {
	if (!TextureManager::IsRectOnScreen(x, stexture.width * x_scale, y, stexture.height * y_scale) && checkVieport) {
		return;
	}

	Instancedrenderer::get().addQuad(Vector4f(x, y, stexture.width, stexture.height), Vector4f(stexture.textureOffsetX, stexture.textureOffsetY, stexture.textureWidth, stexture.textureHeight), stexture.frame);
}

DawnTexture& TextureManager::Loadimage(std::string file, bool isOpenGLThreadInThreadedMode) {
	return TextureCache::get().getTextureFromCache(file);
}

void TextureManager::Loadimage(std::string file, int textureIndex, std::vector<DawnTexture>& textureBase, bool isOpenGLThreadInThreadedMode) {
	if (textureIndex >= textureBase.size()) {
		textureBase.resize(textureIndex + 1);
	}

	textureBase[textureIndex] = TextureCache::get().getTextureFromCache(file);
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
