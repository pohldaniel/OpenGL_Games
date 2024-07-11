#include <engine/input/Keyboard.h>
#include <engine/Fontrenderer.h>
#include <engine/TileSet.h>

#include "MonsterIndex.h"
#include "Globals.h"

MonsterIndex::MonsterIndex() : m_visibleItems(6), m_viewWidth(0.0f), m_viewHeight(0.0f), m_currentOffset(0), m_currentSelected(0){
	m_names.push_back({ "Ivieron", 9u, {1.0f, 1.0f, 1.0f, 1.0f}});
	m_names.push_back({ "Atrox", 0u, {1.0f, 1.0f, 1.0f, 1.0f} });
	m_names.push_back({ "Cindrill", 2u, {1.0f, 1.0f, 1.0f, 1.0f}});
	m_names.push_back({ "Atrox", 0u, {1.0f, 1.0f, 1.0f, 1.0f} });
	m_names.push_back({ "Sparchu", 15u, {1.0f, 1.0f, 1.0f, 1.0f}});
	m_names.push_back({ "Gulfin", 8u, {1.0f, 1.0f, 1.0f, 1.0f}});
	m_names.push_back({ "Jacana", 10u, {1.0f, 1.0f, 1.0f, 1.0f}});
	m_names.push_back({ "Plumette", 13u, {1.0f, 1.0f, 1.0f, 1.0f}});
	m_names.push_back({ "Cleaf", 3u, {1.0f, 1.0f, 1.0f, 1.0f}});

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
	m_surface.draw(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

	Spritesheet::Bind(m_spritesheet);
	float itemHeigt = (0.8f * m_viewHeight) / static_cast<float>(m_visibleItems);
	float top = 0.9f * m_viewHeight - itemHeigt;
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	for (int i = 0; i < std::min(static_cast<int>(m_names.size()), 6); i++) {
		Vector4f color = i == m_currentSelected ? Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f) : Vector4f(0.22745f, 0.21568f, 0.23137f, 1.0f);
		m_surface.setPosition(m_viewWidth * 0.2f, top - i * itemHeigt, 0.0f);
		m_surface.setScale(m_viewWidth * 0.2f, itemHeigt, 1.0f);
		m_surface.draw(rects[16], color);
		
		const std::tuple<std::string, unsigned int, Vector4f>& currentMonster = m_names[i + m_currentOffset];
		const TextureRect& rect = rects[std::get<1>(currentMonster)];
		m_surface.setPosition(m_viewWidth * 0.2f + 45.0f - rect.width * 0.5f, top - i * itemHeigt + (0.5f * itemHeigt - rect.height * 0.5f), 0.0f);
		m_surface.setScale(rect.width, rect.height, 1.0f);
		m_surface.draw(rect, std::get<2>(currentMonster));
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), m_viewWidth * 0.2f + 90.0f, top - i * itemHeigt + (0.5f * itemHeigt - lineHeight * 0.5f), std::get<0>(currentMonster), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
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

void MonsterIndex::processInput() {
	

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_UP)) {
		if(m_currentOffset > 0 && m_currentSelected == 0)
			m_currentOffset--;

		if (m_currentSelected > 0)
			m_currentSelected--;
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)) {
		
		if (static_cast<int>(m_names.size()) - m_currentOffset > 6 && m_currentSelected == 5)
			m_currentOffset++;

		if (m_currentSelected < std::min(static_cast<int>(m_names.size() - 1), 5))
			m_currentSelected++;

	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		Vector4f& color = std::get<2>(m_names[m_currentOffset + m_currentSelected]);
		if (color == Vector4f(1.0f, 0.0f, 0.0f, 1.0f)) {
			color = Vector4f(1.0f, 1.0f, 1.0f, 1.0f);
		}else {
			color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
		}
	}
}