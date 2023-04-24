#pragma once
#include "CharacterSet.h"
#include "Batchrenderer.h"

class Fontrenderer {
	
public:

	void drawText(const CharacterSet& characterSet, int posX, int posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = false);
	void addText(const CharacterSet& characterSet, int posX, int posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = false);
	void drawBuffer();
	void bindTexture(const CharacterSet& characterSet);
	void unbindTexture();
	void shutdown();

	void setShader(Shader* shader);
	void setRenderer(Batchrenderer* renderer);
	void resetRenderer();

	std::string floatToString(float val, int precision);
	void init(size_t size = 60);
	void setCamera(const Camera& camera);
	static Fontrenderer& Get();
	static std::string FloatToString(float val, int precision);

private:
	Fontrenderer();
	~Fontrenderer();

	Batchrenderer* m_batchrenderer;
	Batchrenderer* batchrenderer;

	static Fontrenderer s_instance;
};