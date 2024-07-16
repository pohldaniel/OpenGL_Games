#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
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

	std::ifstream file("res/monster.json", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << "res/monster.json" << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	for (rapidjson::Value::ConstMemberIterator monster = doc.MemberBegin(); monster != doc.MemberEnd(); ++monster) {
		m_monsterData[monster->name.GetString()].element = monster->value["element"].GetString();
		m_monsterData[monster->name.GetString()].maxHealth = monster->value["max_health"].GetUint();
		m_monsterData[monster->name.GetString()].maxEnergy = monster->value["max_energy"].GetUint();
		m_monsterData[monster->name.GetString()].attack = monster->value["attack"].GetFloat();
		m_monsterData[monster->name.GetString()].defense = monster->value["defense"].GetFloat();
		m_monsterData[monster->name.GetString()].recovery = monster->value["recovery"].GetFloat();
		m_monsterData[monster->name.GetString()].speed = monster->value["speed"].GetFloat();
		m_monsterData[monster->name.GetString()].graphic = monster->value["graphic"].GetUint();

		for (rapidjson::Value::ConstMemberIterator ability = monster->value["abilities"].MemberBegin(); ability != monster->value["abilities"].MemberEnd(); ++ability) {
			m_monsterData[monster->name.GetString()].abilities[ability->name.GetString()] = ability->value.GetUint();
		}

		if (monster->value.HasMember("evolve")) {
			for (rapidjson::Value::ConstMemberIterator evolve = monster->value["evolve"].MemberBegin(); evolve != monster->value["evolve"].MemberEnd(); ++evolve) {
				m_monsterData[monster->name.GetString()].evolve = { evolve->name.GetString() , evolve->value.GetUint() };
			}
		}
	}

	m_monster.push_back({ "Ivieron", 32u, false });
	m_monster.push_back({ "Atrox", 15u, false });
	m_monster.push_back({ "Cindrill", 23u, false });
	m_monster.push_back({ "Atrox", 30u, false });
	m_monster.push_back({ "Sparchu", 24u, false });
	m_monster.push_back({ "Gulfin", 17u, false });
	m_monster.push_back({ "Jacana", 16u, false });
	m_monster.push_back({ "Plumette", 9u, false });
	m_monster.push_back({ "Cleaf", 3u, false });
	m_monster.push_back({ "Charmadillo", 3u, false });

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
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/bar.png", false, true, false);
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

	//m_surfaceBar.setShader(Globals::shaderManager.getAssetPointer("list"));

	m_stats.push_back("health");
	m_stats.push_back("energy");
	m_stats.push_back("attack");
	m_stats.push_back("defense");
	m_stats.push_back("speed");
	m_stats.push_back("recovery");
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
	for (int i = 0; i < std::min(static_cast<int>(m_monster.size()), m_visibleItems); i++) {

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
	for (int i = 0; i < std::min(static_cast<int>(m_monster.size()), m_visibleItems); i++) {
		
		if (i != m_visibleItems - 1) {
			m_surface.setPosition(m_viewWidth * 0.2f, top - (i + 1) * itemHeigt, 0.0f);
			m_surface.setScale(m_viewWidth * 0.2f, 2.0f, 1.0f);
			m_surface.draw(rects[16], Vector4f(0.29411f, 0.28235f, 0.30196f, 1.0f));
		}

		const Monster& currentMonster = m_monster[i + m_currentOffset];
		const TextureRect& rect = rects[m_monsterData[currentMonster.name].graphic];
		m_surface.setPosition(m_viewWidth * 0.2f + 45.0f - rect.width * 0.5f, top - i * itemHeigt - 0.5f * (itemHeigt + rect.height), 0.0f);
		m_surface.setScale(rect.width, rect.height, 1.0f);
		m_surface.draw(rect, currentMonster.selected ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_surface.setPosition(m_viewWidth * 0.4f - 4.0f, bottom, 0.0f);
	m_surface.setScale(4.0f, itemHeigt * static_cast<float>(m_visibleItems), 1.0f);
	m_surface.draw(rects[16], Vector4f(0.0f, 0.0f, 0.0f, 0.39216f));

	Globals::fontManager.get("dialog").bind();
	for (int i = 0; i < std::min(static_cast<int>(m_monster.size()), m_visibleItems); i++) {
		const Monster& currentMonster = m_monster[i + m_currentOffset];
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), m_viewWidth * 0.2f + 90.0f, top - i * itemHeigt - 0.5f * (itemHeigt + lineHeight), currentMonster.name, currentMonster.selected ? Vector4f(1.0f, 0.84313f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
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


	const Monster& currentMonster = m_monster[m_currentOffset + m_currentSelected];
	shader->loadVector("u_dimensions", Vector2f(m_viewWidth * 0.4f, m_viewHeight * 0.3f));
	shader->loadFloat("u_radius", 12.0f);
	shader->loadUnsignedInt("u_edge", Edge::TOP_RIGHT);
	m_surface.setPosition(m_viewWidth * 0.4f, bottom + 0.5f * m_viewHeight, 0.0f);
	m_surface.setScale(m_viewWidth * 0.4f, m_viewHeight * 0.3f, 1.0f);
	m_surface.draw(m_colorMap[m_monsterData[currentMonster.name].element]);

	Spritesheet::Bind(m_atlasMonster);
	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[m_monsterData[currentMonster.name].graphic * 8 + m_currentFrame];
	m_surface.resetShader();
	m_surface.setPosition(0.4f * m_viewWidth  + 0.2f * m_viewWidth - 0.5f * rect.width, bottom + 0.5f * m_viewHeight + 0.15f * m_viewHeight - 0.5f * rect.height, 0.0f);
	m_surface.setScale(rect.width, rect.height, 1.0f);
	m_surface.draw(rect);

	Globals::fontManager.get("bold").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("bold"), 0.4f * m_viewWidth + 10.0f, top - 10.0f - lineHeightBold, currentMonster.name, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.05f);
	Fontrenderer::Get().drawBuffer();

	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.4f * m_viewWidth + 10.0f, bottom + 0.5f * m_viewHeight + 10.0f, "Lvl: " + std::to_string(currentMonster.level), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.8f * m_viewWidth - 0.045f * Globals::fontManager.get("dialog").getWidth(m_monsterData[currentMonster.name].element) - 10.0f, bottom + 0.5f * m_viewHeight + 10.0f, m_monsterData[currentMonster.name].element, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	Fontrenderer::Get().drawBuffer();

	drawBar({ 0.4f * m_viewWidth + 10.0f , bottom + 0.5f * m_viewHeight + 6.0f, 100.0f, 4.0f }, 700.0f, currentMonster.level * 150.0f, Vector4f(0.16862f, 0.16078f, 0.17255f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.0f);
	drawBar({ 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f , bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - 7.5f, m_viewWidth * 0.4f * 0.45f, 30.0f }, 150.0f, static_cast<float>(currentMonster.level * m_monsterData[currentMonster.name].maxHealth), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f), 2.0f);
	drawBar({ 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.75f - m_viewWidth * 0.4f * 0.45f * 0.5f , bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - 7.5f, m_viewWidth * 0.4f * 0.45f, 30.0f }, 150.0f, static_cast<float>(currentMonster.level * m_monsterData[currentMonster.name].maxEnergy), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f), 2.0f);
	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f + 10.0f, bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - lineHeight * 0.5f + 7.5f, "HP: 150/" + std::to_string(currentMonster.level * m_monsterData[currentMonster.name].maxHealth), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.75f - m_viewWidth * 0.4f * 0.45f * 0.5f + 10.0f, bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - lineHeight * 0.5f + 7.5f, "EP: 150/" + std::to_string(currentMonster.level * m_monsterData[currentMonster.name].maxEnergy), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	Fontrenderer::Get().drawBuffer();

	float left = 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f;
	float width = m_viewWidth * 0.4f * 0.45f;
	float height =  0.5f * m_viewHeight - m_viewWidth * 0.03f - 7.5f;
	float statHeight = (height - 60.0f) / static_cast <float>(m_stats.size());


	Spritesheet::Bind(m_atlasIcons);
	m_surface.setPosition(left, bottom + 30.0f, 0.0f);
	m_surface.setScale(width, height - 60.0f, 1.0f);
	//m_surface.draw(rects[16], Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), left, bottom + 30.0f + height - 60.0f, "Stats", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);
	
	for (size_t i = 0; i < m_stats.size(); i++) {
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), left, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight, m_stats[i], Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 0.045f);

	}
	
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
		if (static_cast<int>(m_monster.size()) - m_currentOffset > m_visibleItems && m_currentSelected == m_visibleItems - 1) {
			resetAnimation();
			m_currentOffset++;
		}

		if (m_currentSelected < std::min(static_cast<int>(m_monster.size() - 1), m_visibleItems - 1)) {
			resetAnimation();
			m_currentSelected++;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		m_monster[m_currentOffset + m_currentSelected].selected = !m_monster[m_currentOffset + m_currentSelected].selected;
		if (m_beforeSelected != m_currentOffset + m_currentSelected) {
			if (m_beforeSelected != -1) {
				m_monster[m_currentOffset + m_currentSelected].selected = false;
				m_monster[m_beforeSelected].selected = false;
				std::iter_swap(m_monster.begin() + m_beforeSelected, m_monster.begin() + m_currentOffset + m_currentSelected);
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

void MonsterIndex::drawBar(const Rect& rect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color, float radius) {
	
	
	float ratio = rect.width / maxValue;
	float progress = std::max(0.0f, std::min(rect.width, value * ratio));

	if (radius != 0.0f) {
		m_surfaceBar.setShader(Globals::shaderManager.getAssetPointer("list"));
		auto shader = m_surfaceBar.getShader();
		shader->use();
		shader->loadFloat("u_radius", radius);
		shader->loadUnsignedInt("u_edge", Edge::ALL);

		shader->loadVector("u_dimensions", Vector2f(rect.width, rect.height));
		m_surfaceBar.setPosition(rect.posX, rect.posY, 0.0f);
		m_surfaceBar.setScale(rect.width, rect.height, 1.0f);
		m_surfaceBar.draw(bgColor);

		shader->loadVector("u_dimensions", Vector2f(progress, rect.height));
		m_surfaceBar.setPosition(rect.posX, rect.posY, 0.0f);
		m_surfaceBar.setScale(progress, rect.height, 1.0f);
		m_surfaceBar.draw(color);

	}else {
		Spritesheet::Bind(m_atlasIcons);
		m_surfaceBar.resetShader();
		
		m_surfaceBar.setPosition(rect.posX, rect.posY, 0.0f);
		m_surfaceBar.setScale(rect.width, rect.height, 1.0f);
		m_surfaceBar.draw(TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[17], bgColor);

		m_surfaceBar.setPosition(rect.posX, rect.posY, 0.0f);
		m_surfaceBar.setScale(progress, rect.height, 1.0f);
		m_surfaceBar.draw(TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[17], color);
	}
	
}