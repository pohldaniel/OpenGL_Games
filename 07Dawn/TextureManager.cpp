#include "TextureManager.h"
#include "ViewPort.h"

TextureCache TextureCache::s_instance;
TextureAtlasCreator TextureAtlasCreator::s_instance;

DawnTexture TextureCache::getTextureFromCache(std::string filename){
	if (textures.count(filename) > 0) {
		return textures[filename];
	}

	Texture* tex  = new Texture();
	tex->loadFromFile(filename, true);

	textures[filename].width = tex->getWidth();
	textures[filename].height = tex->getHeight();
	textures[filename].x1 = 0.0f;
	textures[filename].x2 = 1.0f;
	textures[filename].y1 = 0.0f;
	textures[filename].y2 = 1.0f;


	char* bytes = (char*)malloc(tex->getWidth() * tex->getHeight() * tex->getChannels());
	tex->readPixel(bytes);
	TextureAtlasCreator::get().addTexture(textures[filename], bytes, tex->getWidth(), tex->getHeight());
	free(bytes);

	return textures[filename];
}

void TextureManager::DrawTexture(DawnTexture& stexture, int x, int y, float transparency, float red, float green, float blue, float x_scale, float y_scale) {
	if (!TextureManager::IsRectOnScreen(x, stexture.width * x_scale, y, stexture.height * y_scale)) {
		return;
	}
	//glColor4f(red, green, blue, transparency);
	//glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	Batchrenderer::get().addQuad(Vector4f(x, y, stexture.width, stexture.height), Vector4f(stexture.x1, stexture.y1, stexture.x2, stexture.y2), stexture.frame);
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
