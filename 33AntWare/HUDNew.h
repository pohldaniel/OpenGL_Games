#pragma once
//head up display

#include <vector>
#include <engine/Camera.h>
#include <engine/Rect.h>
#include <Scene/Object.h>

class HUDNew{

public:

	HUDNew();

	void draw(const Camera& camera);

	Object m_ammo;
	std::vector<TextureRect> m_tileSet;
	unsigned int m_hudAtlas;
};