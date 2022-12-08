#pragma once

#include "engine/Fontrenderer.h"

#include "Widget.h"
#include "Player.h"

struct Tabs{
	int posX, posY, width, height;
	TextureRect tabimage;
};

class CharacterInfo : public Widget {

public:
	
	~CharacterInfo() = default;

	void draw() override;
	void drawExpBar();
	void drawTabs();
	void processInput() override;

	void init(std::vector<TextureRect> textures);
	void setPlayer(Player* player);

	static CharacterInfo& Get();

private:

	CharacterInfo();

	Player* m_player;
	unsigned short currentTab;
	std::vector<TextureRect> m_textures;

	CharacterSet* m_infoFont;
	Tabs tabs[3];

	static CharacterInfo s_instance;
};