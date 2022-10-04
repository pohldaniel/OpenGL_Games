#pragma once
#include "StateMachine.h"


class LoadingScreen : public State {

public:
	LoadingScreen(StateMachine& machine);
	~LoadingScreen();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;

private:
	std::vector<TextureRect> m_backgroundTextures;
	unsigned int m_textureAtlas;

	short m_posX;
	short m_posY;
	short m_width;
	short m_height;
	unsigned short m_backgroundToDraw;
	std::string m_curText;
	float m_progress;
	const CharacterSet& m_characterSet;
};