#pragma once
#include "engine/CharacterSet.h"
#include "engine/Batchrenderer.h"

class Fontrenderer {

public:

	void drawText(int posX, int posY, std::string text);
	void setCharacterSet(const CharacterSet& characterSet);
	void setShader(Shader* shader);
	unsigned int getFontHeight();
	std::string FloatToString(float val, int precision);

	static Fontrenderer& get();
	
private:
	Fontrenderer() = default;
	~Fontrenderer();

	const CharacterSet* m_characterSet;
	Shader *m_shader;

	static Fontrenderer s_instance;
};