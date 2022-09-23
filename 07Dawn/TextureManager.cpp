#include "TextureManager.h"
#include "ViewPort.h"
#include "Constants.h"

TextureCache TextureCache::s_instance;
TextureAtlasCreator TextureAtlasCreator::s_instance;
TextureManager TextureManager::s_instance;

TextureCache& TextureCache::Get() {
	return s_instance;
}

TextureRect& TextureCache::getTextureFromCache(std::string filename){

	if (textures.count(filename) > 0) {
		return textures[filename];
	}
	
	Texture tex;
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

void TextureManager::DrawTextureBatched(TextureRect& stexture, int x, int y, bool checkVieport, Vector4f color) {
	if (!TextureManager::IsRectOnScreen(x, stexture.width, y, stexture.height) && checkVieport) {
		return;
	}
	Batchrenderer::get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(stexture.width), static_cast< float >(stexture.height)), Vector4f(stexture.textureOffsetX, stexture.textureOffsetY, stexture.textureWidth, stexture.textureHeight), color, stexture.frame);
}

void TextureManager::DrawTextureBatched(TextureRect& stexture, int x, int y, float width, float height, bool checkVieport, Vector4f color) {
	if (!TextureManager::IsRectOnScreen(x, width, y, height) && checkVieport) {
		return;
	}

	Batchrenderer::get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), width, height), Vector4f(stexture.textureOffsetX, stexture.textureOffsetY, stexture.textureWidth, stexture.textureHeight), color, stexture.frame);
}

void TextureManager::DrawTextureInstanced(TextureRect& stexture, int x, int y, bool checkVieport) {
	if (!TextureManager::IsRectOnScreen(x, stexture.width, y, stexture.height) && checkVieport) {
		return;
	}

	Instancedrenderer::get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(stexture.width), static_cast< float >(stexture.height)), Vector4f(stexture.textureOffsetX, stexture.textureOffsetY, stexture.textureWidth, stexture.textureHeight), stexture.frame);
}

TextureRect& TextureManager::Loadimage(std::string file, bool isOpenGLThreadInThreadedMode) {
	return TextureCache::Get().getTextureFromCache(file);
}

void TextureManager::Loadimage(std::string file, int textureIndex, std::vector<TextureRect>& textureBase, bool isOpenGLThreadInThreadedMode) {
	if (textureIndex >= textureBase.size()) {
		textureBase.resize(textureIndex + 1);
	}

	textureBase[textureIndex] = TextureCache::Get().getTextureFromCache(file);
}

TextureManager& TextureManager::Get() {
	return s_instance;
}

unsigned int& TextureManager::GetTextureAtlas(std::string name) {
	return TextureManager::Get().m_textureAtlases[name];
}

void TextureManager::SetTextureAtlas(std::string name, unsigned int value) {
	TextureManager::Get().m_textureAtlases[name] = value;
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
