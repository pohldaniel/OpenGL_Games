#pragma once
//head up display

#include <vector>
#include <engine/Camera.h>
#include <engine/Rect.h>
#include <Scene/Object.h>

class HUD{

public:

	HUD();

	void init();
	void draw(const Camera& camera);
	void drawHP(const Camera& camera);
	void drawAmmo(const Camera& camera);

	void setHP(unsigned int hp);
	void setInHandAmmo(unsigned ammo);
	void setTotalAmmo(unsigned ammo);
	void setIsHurting(bool isHurting);
	void setWin(bool win);
	void setLoose(bool loose);

	Object m_sop;

	std::vector<TextureRect> m_tileSet;
	unsigned int m_hudAtlas;
	int hp;
	int inHandAmmo, totalAmmo;
	bool isHurting = false;
	bool m_win = false;
	bool m_loose = false;

	static HUD& Get();
	static HUD s_instance;
};