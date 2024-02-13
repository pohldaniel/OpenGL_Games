#include "HUD.h"

#include "Globals.h"
#include "TileSet.h"
#include "Application.h"

HUD HUD::s_instance;

HUD::HUD() {

}

void HUD::init() {
	TextureAtlasCreator::Get().init(2048u, 2048u);
	TileSetManager::Get().getTileSet("hud").loadTileSetCpu(std::vector<std::string>({
		"res/textures/0.png",
		"res/textures/1.png",
		"res/textures/2.png",
		"res/textures/3.png",
		"res/textures/4.png",
		"res/textures/5.png",
		"res/textures/6.png",
		"res/textures/7.png",
		"res/textures/8.png",
		"res/textures/9.png",
		"res/textures/ammo.png",
		"res/textures/plus.png",
		"res/textures/crosshair.png",
		"res/textures/backSlash.png",
		"res/textures/lose.png",
		"res/textures/win.png",
		"res/textures/hurt.png" }), true);


	TileSetManager::Get().getTileSet("hud").loadTileSetGpu();
	m_tileSet = TileSetManager::Get().getTileSet("hud").getTextureRects();
	m_hudAtlas = TileSetManager::Get().getTileSet("hud").getAtlas();

	Spritesheet::SetFilter(m_hudAtlas, GL_LINEAR);
	Spritesheet::SetWrapMode(m_hudAtlas, GL_REPEAT);

	//Spritesheet::Safe("hud", m_hudAtlas);

	auto shader = Globals::shaderManager.getAssetPointer("hud");
	shader->use();
	shader->loadInt("u_texture", 1);
	shader->unuse();

	Spritesheet::Bind(m_hudAtlas, 1u);
	Sprite::GetShader()->use();
	Sprite::GetShader()->loadVector("u_color", Vector4f(0.5f, 0.5f, 0.5f, 1.0f));
	Sprite::GetShader()->loadInt("u_texture", 1);
	Sprite::GetShader()->unuse();

	resize(0, 0);
}

void HUD::resize(int deltaW, int deltaH) {
	Sprite::Resize(Application::Width, Application::Height);

	m_screenWidth = static_cast<float>(Application::Width);
	m_screenHeight = static_cast<float>(Application::Height);
	m_aspect = m_screenWidth / m_screenHeight;

	m_crossHairSize.set(0.0625f * m_screenWidth, 0.0625f * m_aspect * m_screenHeight);
	m_plusSize.set(0.04f * m_screenWidth, 0.04f * m_aspect * m_screenHeight);
	m_hpSize.set(0.06f * m_screenWidth, 0.06f * m_aspect * m_screenHeight);

	m_handAmmoSize.set(0.06f * m_screenWidth, 0.06f * m_aspect * m_screenHeight);
	m_ammoSize.set(0.035f * m_screenWidth, 0.035f * m_aspect * m_screenHeight);
	m_bulletSize.set(0.025f * m_screenWidth, 0.025f * m_aspect * m_screenHeight);
}

void HUD::draw(const Camera& camera) {
	if (m_loose) {
		m_sprite.setPosition(0.0f, 0.0f, 0.0f);
		m_sprite.setScale(m_screenWidth, m_screenHeight, 0.0f);
		m_sprite.draw(m_tileSet[14]);
	}else if (m_win) {
		m_sprite.setPosition(0.0f, 0.0f, 0.0f);
		m_sprite.setScale(m_screenWidth, m_screenHeight, 0.0f);
		m_sprite.draw(m_tileSet[15]);
	}else {
		m_sprite.setPosition(m_screenWidth * 0.5f - m_crossHairSize[0] * 0.5f, m_screenHeight * 0.5f - m_crossHairSize[1] * 0.5f, 0.0f);
		m_sprite.setScale(m_crossHairSize[0], m_crossHairSize[1], 0.0f);
		m_sprite.draw(m_tileSet[12]);
		if (isHurting) {
			m_sprite.setPosition(0.0f, 0.0f, 0.0f);
			m_sprite.setScale(m_screenWidth, m_screenHeight, 0.0f);
			m_sprite.draw(m_tileSet[16]);
		}

		drawHP();
		drawAmmo();
	}

	Sprite::UnuseShader();
}

void HUD::drawHP() {

	m_sprite.setPosition(0.06f * m_screenWidth, 0.017f * m_aspect * m_screenHeight, 0.0f);
	m_sprite.setScale(m_plusSize[0], m_plusSize[1], 0.0f);
	m_sprite.draw(m_tileSet[11]);

	if (hp >= 100) {
		m_sprite.setPosition(0.06f * m_screenWidth + m_hpSize[0] * 0.5f, 0.01f * m_aspect * m_screenHeight, 0.0f);
		m_sprite.setScale(m_hpSize[0], m_hpSize[1], 0.0f);
		m_sprite.draw(m_tileSet[hp / 100]);
	}

	if (hp >= 10) {
		m_sprite.setPosition(0.06f * m_screenWidth + (m_hpSize[0] + 2.0f), 0.01f * m_aspect * m_screenHeight, 0.0f);
		m_sprite.setScale(m_hpSize[0], m_hpSize[1], 0.0f);
		m_sprite.draw(m_tileSet[(hp / 10) % 10]);
	}

	m_sprite.setPosition(0.06f * m_screenWidth + (m_hpSize[0] + 7.0f) * 1.5f, 0.01f * m_aspect * m_screenHeight, 0.0f);
	m_sprite.setScale(m_hpSize[0], m_hpSize[1], 0.0f);
	m_sprite.draw(m_tileSet[hp % 10]);
}

void HUD::drawAmmo() {
	if (inHandAmmo >= 100) {
		m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth, 0.01f * m_aspect * m_screenHeight, 0.0f);
		m_sprite.setScale(m_handAmmoSize[0], m_handAmmoSize[1], 0.0f);
		m_sprite.draw(m_tileSet[inHandAmmo / 100]);
	}

	if (inHandAmmo >= 10) {
		m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth + m_handAmmoSize[0] * 0.5f, 0.01f * m_aspect * m_screenHeight, 0.0f);
		m_sprite.setScale(m_handAmmoSize[0], m_handAmmoSize[1], 0.0f);
		m_sprite.draw(m_tileSet[(inHandAmmo / 10) % 10]);
	}

	m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth + m_handAmmoSize[0] * 1.1f, 0.01f * m_aspect * m_screenHeight, 0.0f);
	m_sprite.setScale(m_handAmmoSize[0], m_handAmmoSize[1], 0.0f);
	m_sprite.draw(m_tileSet[inHandAmmo % 10]);

	m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth + m_handAmmoSize[0] * 1.85f, 0.015f * m_aspect * m_screenHeight, 0.0f);
	m_sprite.setScale(m_ammoSize[0], m_ammoSize[1], 0.0f);
	m_sprite.draw(m_tileSet[13]);


	float offset = 0.0f;
	if (totalAmmo >= 100) {
		m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth + m_handAmmoSize[0] * 1.85f + m_ammoSize[0] * 0.5f, 0.012f * m_aspect * m_screenHeight, 0.0f);
		m_sprite.setScale(m_ammoSize[0], m_ammoSize[1], 0.0f);
		m_sprite.draw(m_tileSet[totalAmmo / 100]);
		offset = 0.6f;

	}

	if (totalAmmo >= 10) {
		m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth + m_handAmmoSize[0] * 1.85f + m_ammoSize[0] * (offset + 0.5), 0.012f * m_aspect * m_screenHeight, 0.0f);
		m_sprite.setScale(m_ammoSize[0], m_ammoSize[1], 0.0f);
		m_sprite.draw(m_tileSet[(totalAmmo / 10) % 10]);
		offset += 0.6f;

	}

	m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth + m_handAmmoSize[0] * 1.85f + m_ammoSize[0] * (offset + 0.5), 0.012f * m_aspect * m_screenHeight, 0.0f);
	m_sprite.setScale(m_ammoSize[0], m_ammoSize[1], 0.0f);
	m_sprite.draw(m_tileSet[totalAmmo % 10]);

	offset += 0.9f;
	m_sprite.setPosition(m_screenWidth - 0.22f * m_screenWidth + m_handAmmoSize[0] * 1.85f + m_ammoSize[0] * (offset + 0.5), 0.015f * m_aspect * m_screenHeight, 0.0f);
	m_sprite.setScale(m_bulletSize[0], m_bulletSize[1], 0.0f);
	m_sprite.draw(m_tileSet[10]);
}

void HUD::setHP(unsigned int hp) {
	this->hp = hp;
}

void HUD::setInHandAmmo(unsigned ammo) {
	this->inHandAmmo = ammo;
}

void HUD::setTotalAmmo(unsigned ammo) {
	this->totalAmmo = ammo;
}

void HUD::setIsHurting(bool isHurting) {
	this->isHurting = isHurting;
}

void HUD::setWin(bool win) {
	m_win = win;
}

void HUD::setLoose(bool loose) {
	m_loose = loose;
}

HUD& HUD::Get() {
	return s_instance;
}