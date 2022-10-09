#include "TextureManager.h"
#include "ViewPort.h"

TextureCache TextureCache::s_instance;
TextureAtlasCreator TextureAtlasCreator::s_instance;
TextureManager TextureManager::s_instance;

TextureCache& TextureCache::Get() {
	return s_instance;
}

TextureRect& TextureCache::getTextureFromCache(std::string filename, unsigned int maxWidth, unsigned maxHeight) {

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
	TextureAtlasCreator::get().addTexture(textures[filename], reinterpret_cast<char*>(bytes), tex.getWidth(), tex.getHeight(), maxWidth, maxHeight);
	free(bytes);

	return textures[filename];
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}
	Batchrenderer::Get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}
	Batchrenderer::Get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, float width, float height, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	Batchrenderer::Get().addQuad(Vector4f(static_cast<float>(x), static_cast<float>(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, float width, float height, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	Batchrenderer::Get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}
	Batchrenderer::Get().drawSingleQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);

}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}
	Batchrenderer::Get().drawSingleQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, float width, float height, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	Batchrenderer::Get().drawSingleQuad(Vector4f(static_cast<float>(x), static_cast<float>(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, float width, float height, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	Batchrenderer::Get().drawSingleQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTextureInstanced(const TextureRect& textureRect, int x, int y, bool checkVieport) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && checkVieport) {
		return;
	}

	Instancedrenderer::Get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), textureRect.frame);
}

TextureRect& TextureManager::Loadimage(std::string file) {
	return TextureCache::Get().getTextureFromCache(file);
}

TextureRect& TextureManager::Loadimage(std::string file, unsigned int maxWidth, unsigned maxHeight) {
	return TextureCache::Get().getTextureFromCache(file, maxWidth, maxHeight);
}

void TextureManager::Loadimage(std::string file, int textureIndex, std::vector<TextureRect>& textureBase) {
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

bool TextureManager::IsRectOnScreen(int left, int width, int bottom, int height) {
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

bool TextureManager::CheckPointInRect(float px, float py, int left, int width, int bottom, int height) {
	return (left < px && left + width > px && bottom < py && bottom + height > py);
}
