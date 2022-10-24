#include "TextureManager.h"
#include "ViewPort.h"

TextureCache TextureCache::s_instance;
TextureAtlasCreator TextureAtlasCreator::s_instance;
TextureManager TextureManager::s_instance;
float TextureManager::TransPos[8] = { 0.0f ,0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };

TextureCache& TextureCache::Get() {
	return s_instance;
}

TextureRect& TextureCache::getTextureFromCache(std::string filename, unsigned int maxWidth, unsigned maxHeight, bool reload, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {

	if (textures.count(filename) > 0 && !reload) {
		return textures[filename];
	}
	
	Texture tex;
	(paddingLeft != 0 || paddingRight != 0 || paddingTop != 0 || paddingBottom != 0) ? tex.loadFromFile(filename, true, 0 , 0, paddingLeft, paddingRight, paddingTop, paddingBottom) : tex.loadFromFile(filename, true);
	
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
	TextureAtlasCreator::get().addTexture(textures[filename], reinterpret_cast<char*>(bytes), tex.getWidth(), tex.getHeight(), maxWidth, maxHeight, paddingLeft, paddingTop);
	free(bytes);

	return textures[filename];
}

/////////////////////////////////////////////////////////////////////////////
TextureManager& TextureManager::Get() {
	return s_instance;
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}
	Batchrenderer::Get().addQuadAA(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}
	Batchrenderer::Get().addQuadAA(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, float width, float height, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(x), static_cast<float>(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, int x, int y, float width, float height, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	
	Batchrenderer::Get().addQuadAA(Vector4f(static_cast< float >(x), static_cast< float >(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, float pos[8], bool updateView) {
	Batchrenderer::Get().addQuad(pos, Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTextureBatched(const TextureRect& textureRect, float pos[8], Vector4f color, bool updateView) {
	Batchrenderer::Get().addQuad(pos, Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}

	float balance[4] = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(textureRect.width), static_cast<float>(textureRect.height) };

	Batchrenderer::Get().drawSingleQuadAA(Vector4f(static_cast<float>(x), static_cast<float>(y), static_cast<float>(textureRect.width), static_cast<float>(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);

}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && cullVieport) {
		return;
	}
	Batchrenderer::Get().drawSingleQuadAA(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, float width, float height, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	Batchrenderer::Get().drawSingleQuadAA(Vector4f(static_cast<float>(x), static_cast<float>(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, int x, int y, float width, float height, Vector4f color, bool cullVieport, bool updateView) {
	if (!TextureManager::IsRectOnScreen(x, static_cast<int>(width), y, static_cast<int>(height)) && cullVieport) {
		return;
	}
	Batchrenderer::Get().drawSingleQuadAA(Vector4f(static_cast< float >(x), static_cast< float >(y), width, height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, float pos[8], bool updateView) {
	Batchrenderer::Get().drawSingleQuad(pos, Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), textureRect.frame, updateView);
}

void TextureManager::DrawTexture(const TextureRect& textureRect, float pos[8], Vector4f color, bool updateView) {
	Batchrenderer::Get().drawSingleQuad(pos, Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame, updateView);
}

void TextureManager::DrawTextureInstanced(const TextureRect& textureRect, int x, int y, bool checkVieport) {
	if (!TextureManager::IsRectOnScreen(x, textureRect.width, y, textureRect.height) && checkVieport) {
		return;
	}

	Instancedrenderer::Get().addQuad(Vector4f(static_cast< float >(x), static_cast< float >(y), static_cast< float >(textureRect.width), static_cast< float >(textureRect.height)), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), textureRect.frame);
}

void TextureManager::DrawBuffer(bool updateView) {
	Batchrenderer::Get().drawBuffer(updateView);
}

TextureRect& TextureManager::Loadimage(std::string file, unsigned int maxWidth, unsigned maxHeight, bool reload, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {
	return TextureCache::Get().getTextureFromCache(file, maxWidth, maxHeight, reload, paddingLeft, paddingRight, paddingTop, paddingBottom);
}

TextureRect& TextureManager::Loadimage(std::string file) {
	return TextureCache::Get().getTextureFromCache(file, 0u, 0u, false, 0, 0, 0, 0);
}

void TextureManager::Loadimage(std::string file, int textureIndex, std::vector<TextureRect>& textureBase) {
	if (textureIndex >= textureBase.size()) {
		textureBase.resize(textureIndex + 1);
	}

	textureBase[textureIndex] = TextureCache::Get().getTextureFromCache(file);
}

unsigned int& TextureManager::GetTextureAtlas(std::string name) {
	return TextureManager::Get().m_textureAtlases[name];
}

void TextureManager::SetTextureAtlas(std::string name, unsigned int value) {
	TextureManager::Get().m_textureAtlases[name] = value;
}

void TextureManager::BindTexture(unsigned int texture, bool isTextureArray) {
	Batchrenderer::Get().bindTexture(texture, isTextureArray);
}

void TextureManager::UnbindTexture(bool isTextureArray) {
	Batchrenderer::Get().unbindTexture(isTextureArray);
}

void TextureManager::BindTexture(unsigned int texture, bool isTextureArray, unsigned int unit) {
	Batchrenderer::Get().bindTexture(texture, isTextureArray, unit);
}

void TextureManager::UnbindTexture(bool isTextureArray, unsigned int unit) {
	Batchrenderer::Get().unbindTexture(isTextureArray, unit);
}

void TextureManager::SetShader(Shader* shader) {
	Batchrenderer::Get().setShader(shader);
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

void TextureManager::RotateTextureRect(const TextureRect& origin, float posX, float posY, float angle, float rotX, float rotY, float(&pos)[8]) {
	angle *= PI_ON_180;
	float s = sinf(angle);
	float c = cosf(angle);

	pos[0] = (1.0f - c) * rotX + rotY * s + posX;
	pos[1] = (1.0f - c) * rotY - rotX * s + posY;

	pos[2] = (origin.width - rotX) * c + rotY * s + rotX + posX;
	pos[3] = (origin.width - rotX) * s + (1.0f - c) * rotY + posY;

	pos[4] = (origin.width - rotX) * c - (origin.height - rotY) * s + rotX + posX;
	pos[5] = (origin.width - rotX) * s + (origin.height - rotY) * c + rotY + posY;

	pos[6] = -rotX * c - (origin.height - rotY) * s + rotX + posX;
	pos[7] = -rotX * s + (origin.height - rotY) * c + rotY + posY;
}
