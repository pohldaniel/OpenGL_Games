#include <engine/Fontrenderer.h>
#include <engine/TileSet.h>

#include "MonsterIndex.h"
#include "Globals.h"

MonsterIndex::MonsterIndex() : m_visibleItems(6), m_viewWidth(0.0f), m_viewHeight(0.0f){
	m_names.push_back({ "Ivieron", 9u });
	m_names.push_back({ "Atrox", 0u });
	m_names.push_back({ "Cindrill", 2u });
	m_names.push_back({ "Atrox", 0u });
	m_names.push_back({ "Sparchu", 15u });
	m_names.push_back({ "Gulfin", 8u });
	m_names.push_back({ "Jacana", 10u });

	TextureAtlasCreator::Get().init(1024u, 64u);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Atrox.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Charmadillo.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Cindrill.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Cleaf.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Draem.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Finiette.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Finsta.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Friolera.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Gulfin.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Ivieron.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Jacana.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Larvea.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Pluma.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Plumette.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Pouch.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/icons/Sparchu.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/empty.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileSetGpu();
	//Spritesheet::Safe("monster_icon", TileSetManager::Get().getTileSet("monster_icon").getAtlas());
	m_spritesheet = TileSetManager::Get().getTileSet("monster_icon").getAtlas();
}

MonsterIndex::~MonsterIndex() {

}

void MonsterIndex::draw() {

	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("monster_icon").getTextureRects();

	Globals::spritesheetManager.getAssetPointer("null")->bind();
	m_surface.setPosition(m_viewWidth * 0.2f, m_viewHeight * 0.1f, 0.0f);
	m_surface.setScale(m_viewWidth * 0.6f, m_viewHeight * 0.8f, 1.0f);
	m_surface.draw(Vector4f(0.0f, 0.0f, 1.0f, 1.0f));

	Spritesheet::Bind(m_spritesheet);
	float itemHeigt = (0.8f * m_viewHeight) / static_cast<float>(m_visibleItems);
	float top = 0.9f * m_viewHeight - itemHeigt;
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	for (int i = 0; i < std::min(static_cast<int>(m_names.size()), 6); i++) {
		m_surface.setPosition(m_viewWidth * 0.2f, top - i * itemHeigt, 0.0f);
		m_surface.setScale(m_viewWidth * 0.2f, itemHeigt, 1.0f);
		m_surface.draw(rects[16], Vector4f(0.0f, 1.0f / (i + 1), 0.0f, 1.0f));
		
		m_surface.setPosition(m_viewWidth * 0.2f + 20.0f, top - i * itemHeigt + (0.5f * itemHeigt - rects[m_names[i].second].height * 0.5f), 0.0f);
		m_surface.setScale(rects[m_names[i].second].width, rects[m_names[i].second].height, 1.0f);
		m_surface.draw(rects[m_names[i].second], Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), m_viewWidth * 0.2f + 90.0f, top - i * itemHeigt + (0.5f * itemHeigt - lineHeight * 0.5f), m_names[i].first, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	}
	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().drawBuffer();
}

void MonsterIndex::setViewWidth(float viewWidth) {
	m_viewWidth = viewWidth;
}

void MonsterIndex::setViewHeight(float viewHeight) {
	m_viewHeight = viewHeight;
}