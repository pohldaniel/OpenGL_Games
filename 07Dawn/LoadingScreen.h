#pragma once

#include "LoadingManager.h"
#include "StateMachine.h"


class LoadingScreen : public State {

public:

	LoadingScreen(StateMachine& machine, LoadingManager::Entry entry = LoadingManager::Entry::GAME);
	~LoadingScreen();

	void fixedUpdate() override;
	void update() override;
	void render() override;

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
	bool m_initEditor;
	LoadingManager::Entry m_entry;
};