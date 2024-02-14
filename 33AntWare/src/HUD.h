#pragma once
//head up display

#include <vector>
#include <engine/scene/Object.h>
#include <engine/Camera.h>
#include <engine/Rect.h>
#include <engine/Sprite.h>

class HUD{

public:

	HUD();

	void init();
	void draw(const Camera& camera);
	void drawHP();
	void drawAmmo();

	void setHP(unsigned int hp);
	void setInHandAmmo(unsigned ammo);
	void setTotalAmmo(unsigned ammo);
	void setIsHurting(bool isHurting);
	void setWin(bool win);
	void setLoose(bool loose);
	void resize(int deltaW, int deltaH);

	Object m_sop;
	Sprite m_sprite;
	Vector2f m_crossHairSize, m_plusSize, m_hpSize, m_handAmmoSize, m_ammoSize, m_bulletSize;
	std::vector<TextureRect> m_tileSet;
	unsigned int m_hudAtlas;
	int hp;
	int inHandAmmo, totalAmmo;
	bool isHurting = false;
	bool m_win = false;
	bool m_loose = false;
	float m_aspect;
	float m_screenWidth;
	float m_screenHeight;

	static HUD& Get();
	static HUD s_instance;
};