#include "HUD.h"

#include "Globals.h"
#include "TileSet.h"

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
		"res/textures/loading.png",
		"res/textures/lose.png",
		"res/textures/win.png",
		"res/textures/hurt.png" }));


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
}

void HUD::draw(const Camera& camera) {
	auto shader = Globals::shaderManager.getAssetPointer("hud");
	shader->use();
	TextureRect rect;
	if (m_loose) {
		rect = m_tileSet[15];
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad").drawRaw();
	}else if (m_win) {
		rect = m_tileSet[16];
		shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad").drawRaw();
	}else {
		rect = m_tileSet[12];
		shader->loadMatrix("u_transform", Matrix4f::Orthographic(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f));
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		if (isHurting) {


			rect = m_tileSet[17];
			shader->loadMatrix("u_transform", Matrix4f::IDENTITY);
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad").drawRaw();
		}

		drawHP(camera);
		drawAmmo(camera);
	}
}

void HUD::drawHP(const Camera& camera) {

	float hpY = -3.9f - 0.4f + 4.5f, hpSize = 0.9f;
	float hpDiff = 0.6f * hpSize;
	float plusX = -7.0f -0.6f + 8.0f, plusY = -3.75f - 0.85f + 4.5f, plusSize = 1.8f;
	float hp1X = plusX + hpDiff + 0.1f + 0.5f, hp2X = hp1X + hpDiff, hp3X = hp2X + hpDiff;

	if (hp < 0)
		hp = 0;
	
	auto shader = Globals::shaderManager.getAssetPointer("hud");
	shader->use();

	m_sop.setPosition(plusX, plusY, 0.0f);
	m_sop.setScale(plusSize, plusSize, 0.0f);

	TextureRect rect = m_tileSet[11];
	shader->loadMatrix("u_transform", camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Globals::shapeManager.get("quad_aligned").drawRaw();

	if (hp >= 100) {
		m_sop.setPosition(hp1X, hpY, 0.0f);
		m_sop.setScale(hpSize, hpSize, 0.0f);

		rect = m_tileSet[hp / 100];
		shader->loadMatrix("u_transform", camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_aligned").drawRaw();
	}

	if (hp >= 10) {
		m_sop.setPosition(hp2X, hpY, 0.0f);
		m_sop.setScale(hpSize, hpSize, 0.0f);

		rect = m_tileSet[(hp / 10) % 10];
		shader->loadMatrix("u_transform", camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_aligned").drawRaw();
	}

	m_sop.setPosition(hp3X, hpY, 0.0f);
	m_sop.setScale(hpSize, hpSize, 0.0f);

	rect = m_tileSet[hp % 10];
	shader->loadMatrix("u_transform", camera.getOrthographicMatrix() * m_sop.getTransformationSOP());
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);
	Globals::shapeManager.get("quad_aligned").drawRaw();
	shader->unuse();
}

void HUD::drawAmmo(const Camera& camera) {
	Matrix4f ortho = Matrix4f::Orthographic(-8.0f, 8.0f, -4.5f, 4.5f, -1.0f, 1.0f);

	float inHandAmmoY = -3.9f, totalAmmoY = -4.0f;
	float inHandAmmoSize = 0.8f, totalAmmoSize = 0.5f;
	float inHandDiff = 0.6 * inHandAmmoSize;
	float totalDiff = 0.6 * totalAmmoSize;
	float slashX = 6.0f;
	float inHand3X = slashX - inHandDiff, inHand2X = inHand3X - inHandDiff, inHand1X = inHand2X - inHandDiff;
	float total1X = slashX + totalDiff, total2X = total1X + totalDiff, total3X = total2X + totalDiff;
	float bulletDiff = totalDiff - 0.18f, bulletX = total3X + totalDiff + 0.1f, bulletY = totalAmmoY - 0.1f, bulletSize = 0.4f;

	if (totalAmmo < 0)
		totalAmmo = 0;
	if (inHandAmmo < 0)
		inHandAmmo = 0;

	if (totalAmmo >= 100) {
		auto shader = Globals::shaderManager.getAssetPointer("hud");
		shader->use();

		//drawQuad(ammo, { bulletX, bulletY }, { bulletSize, bulletSize });
		m_sop.setPosition(bulletX, bulletY, 0.0f);
		m_sop.setScale(bulletSize, bulletSize, 0.0f);

		TextureRect rect = m_tileSet[10];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		
		//drawQuad(digits[totalAmmo / 100], { total1X, totalAmmoY }, { totalAmmoSize, totalAmmoSize });
		m_sop.setPosition(total1X, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);
		rect = m_tileSet[totalAmmo / 100];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		
		//drawQuad(digits[(totalAmmo / 10) % 10], { total2X, totalAmmoY }, { totalAmmoSize, totalAmmoSize });
		m_sop.setPosition(total2X, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);
		rect = m_tileSet[(totalAmmo / 10) % 10];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		//drawQuad(digits[totalAmmo % 10], { total3X, totalAmmoY }, { totalAmmoSize, totalAmmoSize });
		m_sop.setPosition(total3X, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);
		rect = m_tileSet[totalAmmo % 10];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		//drawQuad(backSlash, { slashX, totalAmmoY }, { totalAmmoSize, totalAmmoSize });
		m_sop.setPosition(slashX, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);
		rect = m_tileSet[13];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		if (inHandAmmo >= 100){
			//drawQuad(digits[inHandAmmo / 100], { inHand1X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

			m_sop.setPosition(inHand1X, inHandAmmoY, 0.0f);
			m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);
			rect = m_tileSet[inHandAmmo / 100];
			shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();
		}

		if (inHandAmmo >= 10){
			//drawQuad(digits[(inHandAmmo / 10) % 10], { inHand2X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

			m_sop.setPosition(inHand2X, inHandAmmoY, 0.0f);
			m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);
			rect = m_tileSet[(inHandAmmo / 10) % 10];
			shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();
		}
		//drawQuad(digits[inHandAmmo % 10], { inHand3X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

		m_sop.setPosition(inHand3X, inHandAmmoY, 0.0f);
		m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);
		rect = m_tileSet[inHandAmmo % 10];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

	}else if (totalAmmo >= 10) {
		auto shader = Globals::shaderManager.getAssetPointer("hud");
		shader->use();

		slashX += totalDiff;
		inHand3X = slashX - inHandDiff, inHand2X = inHand3X - inHandDiff, inHand1X = inHand2X - inHandDiff;
		total1X = slashX + totalDiff, total2X = total1X + totalDiff, total3X = total2X + totalDiff;
		bulletDiff = totalDiff - 0.18f, bulletX = total2X + totalDiff + 0.1f, bulletY = totalAmmoY - 0.1f, bulletSize = 0.4f;

		//drawQuad(ammo, { bulletX, bulletY }, { bulletSize, bulletSize });

		m_sop.setPosition(bulletX, bulletY, 0.0f);
		m_sop.setScale(bulletSize, bulletSize, 0.0f);
		TextureRect rect =  m_tileSet[10];

		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		//drawQuad(digits[(totalAmmo / 10) % 10], { total1X, totalAmmoY }, { totalAmmoSize, totalAmmoSize });

		m_sop.setPosition(total1X, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);
		rect = m_tileSet[(totalAmmo / 10) % 10];

		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		//drawQuad(digits[totalAmmo % 10], { total2X, totalAmmoY }, { totalAmmoSize, totalAmmoSize });

		m_sop.setPosition(total2X, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);
		rect = m_tileSet[totalAmmo % 10];

		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		//drawQuad(backSlash, { slashX, totalAmmoY }, { totalAmmoSize, totalAmmoSize });

		m_sop.setPosition(slashX, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);
		rect = m_tileSet[13];

		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		if (inHandAmmo >= 100){
			//drawQuad(digits[inHandAmmo / 100], { inHand1X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

			m_sop.setPosition(inHand1X, inHandAmmoY, 0.0f);
			m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);
			rect = m_tileSet[inHandAmmo / 100];

			shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();
		}

		if (inHandAmmo >= 10){
			//drawQuad(digits[(inHandAmmo / 10) % 10], { inHand2X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

			m_sop.setPosition(inHand2X, inHandAmmoY, 0.0f);
			m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);
			rect = m_tileSet[(inHandAmmo / 10) % 10];

			shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();
		}
		//drawQuad(digits[inHandAmmo % 10], { inHand3X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

		m_sop.setPosition(inHand3X, inHandAmmoY, 0.0f);
		m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);
		rect = m_tileSet[inHandAmmo % 10];

		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

	}else {

		auto shader = Globals::shaderManager.getAssetPointer("hud");
		shader->use();

		slashX += totalDiff * 2;
		inHand3X = slashX - inHandDiff, inHand2X = inHand3X - inHandDiff, inHand1X = inHand2X - inHandDiff;
		total1X = slashX + totalDiff, total2X = total1X + totalDiff, total3X = total2X + totalDiff;
		bulletDiff = totalDiff - 0.18f, bulletX = total1X + totalDiff + 0.1f, bulletY = totalAmmoY - 0.1f, bulletSize = 0.4f;

		//drawQuad(ammo, { bulletX, bulletY }, { bulletSize, bulletSize });
		m_sop.setPosition(bulletX, bulletY, 0.0f);
		m_sop.setScale(bulletSize, bulletSize, 0.0f);

		TextureRect rect = m_tileSet[10];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		//drawQuad(digits[totalAmmo % 10], { total1X, totalAmmoY }, { totalAmmoSize, totalAmmoSize });

		m_sop.setPosition(total1X, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);

		rect = m_tileSet[totalAmmo % 10];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		//drawQuad(backSlash, { slashX, totalAmmoY }, { totalAmmoSize, totalAmmoSize });

		m_sop.setPosition(slashX, totalAmmoY, 0.0f);
		m_sop.setScale(totalAmmoSize, totalAmmoSize, 0.0f);

		rect = m_tileSet[13];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();

		if (inHandAmmo >= 100){
			//drawQuad(digits[inHandAmmo / 100], { inHand1X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

			m_sop.setPosition(total1X, inHandAmmoY, 0.0f);
			m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);

			rect = m_tileSet[inHandAmmo / 100];
			shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();
		}

		if (inHandAmmo >= 10){
			//drawQuad(digits[(inHandAmmo / 10) % 10], { inHand2X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

			m_sop.setPosition(inHand2X, inHandAmmoY, 0.0f);
			m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);

			rect = m_tileSet[(inHandAmmo / 10) % 10];
			shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
			shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
			shader->loadInt("u_layer", rect.frame);
			Globals::shapeManager.get("quad_half").drawRaw();
		}
		//drawQuad(digits[inHandAmmo % 10], { inHand3X, inHandAmmoY }, { inHandAmmoSize, inHandAmmoSize });

		m_sop.setPosition(inHand3X, inHandAmmoY, 0.0f);
		m_sop.setScale(inHandAmmoSize, inHandAmmoSize, 0.0f);

		rect = m_tileSet[inHandAmmo % 10];
		shader->loadMatrix("u_transform", ortho * m_sop.getTransformationSOP());
		shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
		shader->loadInt("u_layer", rect.frame);
		Globals::shapeManager.get("quad_half").drawRaw();
	}
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