#pragma once

#include "CharacterSet.h"
#include "Batchrenderer.h"
#include "Framebuffer.h"

class Fontrenderer {
	
public:

	void drawText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), float size = 1.0f, bool flipGlyph = false);
	void addText(const CharacterSet& characterSet, float posX, float posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), float size = 1.0f, bool flipGlyph = false);
	void addTextTransformed(const CharacterSet& characterSet, const Matrix4f& transformation, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), float size = 1.0f);
	void drawBuffer();
	void bindTexture(const CharacterSet& characterSet);
	void unbindTexture();
	void shutdown();
	void blitText(int widthDst, int heightDst, int paddingX = 0, int paddingY = 0);
	const unsigned int& getColorTexture(unsigned short attachment = 0) const;
	void bindColorTexture(unsigned short attachment = 0u, unsigned int unit = 0u, bool forceBind = false) const;

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
	Framebuffer renderTarget, blitTarget;

	static Fontrenderer s_instance;
};