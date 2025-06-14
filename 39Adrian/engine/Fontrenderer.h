#pragma once

#include "CharacterSet.h"
#include "Batchrenderer.h"
#include "Texture.h"

class Fontrenderer {
	
public:

	void drawText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), float size = 1.0f, bool flipGlyph = false);
	void addText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), float size = 1.0f, bool flipGlyph = false);
	void addTextTransformed(const CharacterSet& characterSet, const Matrix4f& transformation, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), float size = 1.0f);
	void drawBuffer();
	void bindTexture(const CharacterSet& characterSet);
	void unbindTexture();
	void shutdown();
	void blitTextToTexture(int widthDst, int heightDst, int paddingX, int paddingY, Texture& texture);
	void setBlitSize(unsigned int width, unsigned int height);

	void setShader(Shader* shader);
	void setRenderer(Batchrenderer* renderer);
	void resetRenderer();
	
	std::string floatToString(float val, int precision);
	void init(size_t size = 60, bool drawSingle = false);
	Batchrenderer* getBatchRenderer();

	static Fontrenderer& Get();
	static std::string FloatToString(float val, int precision);

private:
	Fontrenderer();
	~Fontrenderer();

	Batchrenderer* m_batchrenderer;
	Batchrenderer* batchrenderer;

	static Fontrenderer s_instance;
};