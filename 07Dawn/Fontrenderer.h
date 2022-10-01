#pragma once
#include "engine/CharacterSet.h"
#include "engine/Batchrenderer.h"

class Fontrenderer {
	
public:

	void drawText(const CharacterSet& characterSet, int posX, int posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = false);
	void addText(const CharacterSet& characterSet, int posX, int posY, std::string text, Vector4f color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f), bool updateView = false);
	void drawBuffer(bool updateView = false);
	void bindTexture(const CharacterSet& characterSet);
	void unbindTexture();

	void setShader(Shader* shader);
	std::string FloatToString(float val, int precision);
	void init(size_t size = 400);
	void setCamera(const Camera& camera);

	static Fontrenderer& get();
	
private:
	Fontrenderer();
	~Fontrenderer();

	Batchrenderer* m_batchrenderer;
	static Fontrenderer s_instance;
};