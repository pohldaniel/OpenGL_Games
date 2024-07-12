#include <engine/input/Keyboard.h>
#include <engine/Fontrenderer.h>
#include <engine/TileSet.h>

#include "MonsterIndex.h"
#include "Globals.h"

MonsterIndex::MonsterIndex() : 
m_visibleItems(6), 
m_viewWidth(0.0f), 
m_viewHeight(0.0f), 
m_currentOffset(0), 
m_currentSelected(0), 
m_beforeSelected(-1),
m_elapsedTime(0.0f),
m_currentFrame(0),
m_frameCount(4){

	m_names.push_back({ "Ivieron", 32u, 9u, false, "plant", 72u });
	m_names.push_back({ "Atrox", 15u, 0u, false, "fire", 0u });
	m_names.push_back({ "Cindrill", 23u, 2u, false, "fire", 16u });
	m_names.push_back({ "Atrox", 11u, 0u, false, "fire", 0u });
	m_names.push_back({ "Sparchu", 13u, 15u, false, "fire", 120u });
	m_names.push_back({ "Gulfin", 17u, 8u, false, "water", 64u });
	m_names.push_back({ "Jacana", 16u, 10u, false, "fire", 80u });
	m_names.push_back({ "Plumette", 9u, 13u, false, "plant", 104u });
	m_names.push_back({ "Cleaf", 7u, 3u, false, "plant", 24u });
	m_names.push_back({ "Charmadillo", 3u, 1u, false, "fire", 8u });

	m_colorMap["plant"] = { 0.39215f, 0.66274f, 0.56470f, 1.0f};
	m_colorMap["fire"] = { 0.97254f, 0.62745f, 0.37647f, 1.0f };
	m_colorMap["water"] = { 0.31372f, 0.69019f, 0.84705f, 1.0f };

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
	m_atlasIcons = TileSetManager::Get().getTileSet("monster_icon").getAtlas();

	TextureAtlasCreator::Get().init(1024u, 1536u);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Atrox.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Charmadillo.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Cindrill.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Cleaf.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Draem.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Finiette.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Finsta.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Friolera.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Gulfin.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Ivieron.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Jacana.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Larvea.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Pluma.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Plumette.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Pouch.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Sparchu.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetGpu();
	//Spritesheet::Safe("monster", TileSetManager::Get().getTileSet("monster").getAtlas());
	m_atlasMonster = TileSetManager::Get().getTileSet("monster").getAtlas();
}

MonsterIndex::~MonsterIndex() {

}

void MonsterIndex::draw() {

	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("monster_icon").getTextureRects();

	Globals::spritesheetManager.getAssetPointer("null")->bind();
	m_surface.setPosition(m_viewWidth * 0.2f, m_viewHeight * 0.1f, 0.0f);
	m_surface.setScale(m_viewWidth * 0.6f, m_viewHeight * 0.8f, 1.0f);
	//m_surface.draw(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

	Spritesheet::Bind(m_atlasIcons);
	float itemHeigt = (0.8f * m_viewHeight) / static_cast<float>(m_visibleItems);
	float top = 0.9f * m_viewHeight;
	float bottom = 0.1f * m_viewHeight;
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	float lineHeightBold = static_cast<float>(Globals::fontManager.get("bold").lineHeight) * 0.05f;

	auto shader = Globals::shaderManager.getAssetPointer("list");
	shader->use();
	shader->loadVector("u_dimensions", Vector2f(m_viewWidth * 0.2f, itemHeigt));
	shader->loadFloat("u_radius", 12.0f);

	m_surface.setShader(shader);
	for (int i = 0; i < std::min(static_cast<int>(m_names.size()), m_visibleItems); i++) {

		if (i == 0) {
			shader->loadUnsignedInt("u_edge", Edge::TOP_LEFT);
		}else if (i == m_visibleItems - 1) {
			shader->loadUnsignedInt("u_edge", Edge::BOTTOM_LEFT);
		}else {
			shader->loadUnsignedInt("u_edge", Edge::EDGE_NONE);
		}

		Vector4f color = i == m_currentSelected ? Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f) : Vector4f(0.22745f, 0.21568f, 0.23137f, 1.0f);
		m_surface.setPosition(m_viewWidth * 0.2f, top - (i + 1) * itemHeigt, 0.0f);
		m_surface.setScale(m_viewWidth * 0.2f, itemHeigt, 1.0f);
		m_surface.draw(color);	
	}

	m_surface.resetShader();
	for (int i = 0; i < std::min(static_cast<int>(m_names.size()), m_visibleItems); i++) {
		
		if (i != m_visibleItems - 1) {
			m_surface.setPosition(m_viewWidth * 0.2f, top - (i + 1) * itemHeigt, 0.0f);
			m_surface.setScale(m_viewWidth * 0.2f, 2.0f, 1.0f);
			m_surface.draw(rects[16], Vector4f(0.29411f, 0.28235f, 0.30196f, 1.0f));
		}

		const std::tuple<std::string, unsigned int, unsigned int, bool, std::string, unsigned int>& currentMonster = m_names[i + m_currentOffset];
		const TextureRect& rect = rects[std::get<2>(currentMonster)];
		m_surface.setPosition(m_viewWidth * 0.2f + 45.0f - rect.width * 0.5f, top - i * itemHeigt - 0.5f * (itemHeigt + rect.height), 0.0f);
		m_surface.setScale(rect.width, rect.height, 1.0f);
		m_surface.draw(rect, std::get<3>(currentMonster) ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));	
	}

	m_surface.setPosition(m_viewWidth * 0.4f - 4.0f, bottom, 0.0f);
	m_surface.setScale(4.0f, itemHeigt * static_cast<float>(m_visibleItems), 1.0f);
	m_surface.draw(rects[16], Vector4f(0.0f, 0.0f, 0.0f, 0.39216f));

	Globals::fontManager.get("dialog").bind();
	for (int i = 0; i < std::min(static_cast<int>(m_names.size()), m_visibleItems); i++) {
		const std::tuple<std::string, unsigned int, unsigned int, bool, std::string, unsigned int>& currentMonster = m_names[i + m_currentOffset];
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), m_viewWidth * 0.2f + 90.0f, top - i * itemHeigt - 0.5f * (itemHeigt + lineHeight), std::get<0>(currentMonster), std::get<3>(currentMonster) ? Vector4f(1.0f, 0.84313f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	}
	Fontrenderer::Get().drawBuffer();

	
	m_surface.setShader(shader);
	
	shader->use();
	shader->loadVector("u_dimensions", Vector2f(m_viewWidth * 0.4f, m_viewHeight * 0.8f));
	shader->loadFloat("u_radius", 12.0f);
	shader->loadUnsignedInt("u_edge", Edge::EDGE_RIGHT);
	m_surface.setPosition(m_viewWidth * 0.4f, bottom, 0.0f);
	m_surface.setScale(m_viewWidth * 0.4f, m_viewHeight * 0.8f, 1.0f);
	m_surface.draw(Vector4f(0.16862f, 0.16078f, 0.17254f, 1.0f));


	const std::tuple<std::string, unsigned int, unsigned int, bool, std::string, unsigned int>& currentMonster = m_names[m_currentOffset + m_currentSelected];
	shader->loadVector("u_dimensions", Vector2f(m_viewWidth * 0.4f, m_viewHeight * 0.3f));
	shader->loadFloat("u_radius", 12.0f);
	shader->loadUnsignedInt("u_edge", Edge::TOP_RIGHT);
	m_surface.setPosition(m_viewWidth * 0.4f, bottom + 0.5f * m_viewHeight, 0.0f);
	m_surface.setScale(m_viewWidth * 0.4f, m_viewHeight * 0.3f, 1.0f);
	m_surface.draw(m_colorMap[std::get<4>(currentMonster)]);

	Spritesheet::Bind(m_atlasMonster);
	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[std::get<5>(currentMonster) + m_currentFrame];
	m_surface.resetShader();
	m_surface.setPosition(0.4f * m_viewWidth  + 0.2f * m_viewWidth - 0.5f * rect.width, bottom + 0.5f * m_viewHeight + 0.15f * m_viewHeight - 0.5f * rect.height, 0.0f);
	m_surface.setScale(rect.width, rect.height, 1.0f);
	m_surface.draw(rect);

	Globals::fontManager.get("bold").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("bold"), 0.4f * m_viewWidth + 10.0f, top - 10.0f - lineHeightBold, std::get<0>(currentMonster), std::get<3>(currentMonster) ? Vector4f(1.0f, 0.84313f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.05f);
	Fontrenderer::Get().drawBuffer();

	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.4f * m_viewWidth + 10.0f, bottom + 0.5f * m_viewHeight + 10.0f, "Level " + std::to_string(std::get<1>(currentMonster)), std::get<3>(currentMonster) ? Vector4f(1.0f, 0.84313f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	Fontrenderer::Get().drawBuffer();
}

void MonsterIndex::update(float dt) {
	m_elapsedTime += 6.0f * dt;
	m_currentFrame = static_cast <int>(std::floor(m_elapsedTime));
	if (m_currentFrame > m_frameCount - 1) {
		m_currentFrame = 0;
		m_elapsedTime -= static_cast <float>(m_frameCount);
	}
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
		
		if (m_currentOffset > 0 && m_currentSelected == 0) {
			resetAnimation();
			m_currentOffset--;
		}

		if (m_currentSelected > 0) {
			resetAnimation();
			m_currentSelected--;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)) {
		if (static_cast<int>(m_names.size()) - m_currentOffset > m_visibleItems && m_currentSelected == m_visibleItems - 1) {
			resetAnimation();
			m_currentOffset++;
		}

		if (m_currentSelected < std::min(static_cast<int>(m_names.size() - 1), m_visibleItems - 1)) {
			resetAnimation();
			m_currentSelected++;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		std::get<3>(m_names[m_currentOffset + m_currentSelected]) = !std::get<3>(m_names[m_currentOffset + m_currentSelected]);
		if (m_beforeSelected != m_currentOffset + m_currentSelected) {
			if (m_beforeSelected != -1) {
				std::get<3>(m_names[m_currentOffset + m_currentSelected]) = false;
				std::get<3>(m_names[m_beforeSelected]) = false;
				std::iter_swap(m_names.begin() + m_beforeSelected, m_names.begin() + m_currentOffset + m_currentSelected);
				m_beforeSelected = -1;
			}else {
				m_beforeSelected = m_currentOffset + m_currentSelected;
			}
		}else {
			m_beforeSelected = -1;
		}
	}
}

void MonsterIndex::resetAnimation() {
	m_currentFrame = 0;
	m_elapsedTime = 1.0f;
}