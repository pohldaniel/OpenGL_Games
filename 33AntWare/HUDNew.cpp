#include "HUDNew.h"

#include "Globals.h"
#include "TileSet.h"

HUDNew::HUDNew() {

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
		"res/textures/hurt.png"}));


	TileSetManager::Get().getTileSet("hud").loadTileSetGpu();
	m_tileSet = TileSetManager::Get().getTileSet("hud").getTextureRects();	
	m_hudAtlas = TileSetManager::Get().getTileSet("hud").getAtlas();
	//Spritesheet::Safe("hud", m_hudAtlas);

	m_ammo.setScale(30.0f, 30.0f, 1.0f);
	m_ammo.setPosition(100.0f, 100.0f, 0.0f);

	auto shader = Globals::shaderManager.getAssetPointer("hud_new");
	shader->use();
	shader->loadInt("u_texture", 1);
	shader->unuse();

	Spritesheet::Bind(m_hudAtlas, 1u);
}

void HUDNew::draw(const Camera& camera) {

	

	auto shader = Globals::shaderManager.getAssetPointer("hud_new");
	shader->use();
	
	const TextureRect& rect = m_tileSet[10];
	shader->loadMatrix("u_transform", camera.getOrthographicMatrix() * m_ammo.getTransformationSOP());
	shader->loadVector("u_texRect", Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY + rect.textureHeight));
	shader->loadInt("u_layer", rect.frame);

	Globals::shapeManager.get("quad_aligned").drawRaw();
	shader->unuse();
}