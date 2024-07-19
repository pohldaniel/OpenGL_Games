#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/input/Keyboard.h>
#include <engine/Fontrenderer.h>
#include <engine/TileSet.h>

#include "MonsterIndex.h"
#include "Globals.h"

std::unordered_map<std::string, MonsterData> MonsterIndex::MonsterData;
std::vector<MonsterEntry> MonsterIndex::Monster;

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
		MonsterData[monster->name.GetString()].element = monster->value["element"].GetString();
		MonsterData[monster->name.GetString()].maxHealth = monster->value["max_health"].GetUint();
		MonsterData[monster->name.GetString()].maxEnergy = monster->value["max_energy"].GetUint();
		MonsterData[monster->name.GetString()].attack = monster->value["attack"].GetFloat();
		MonsterData[monster->name.GetString()].defense = monster->value["defense"].GetFloat();
		MonsterData[monster->name.GetString()].recovery = monster->value["recovery"].GetFloat();
		MonsterData[monster->name.GetString()].speed = monster->value["speed"].GetFloat();
		MonsterData[monster->name.GetString()].graphic = monster->value["graphic"].GetUint();

		m_maxStats["health"] = std::max(m_maxStats["health"], static_cast<float>(MonsterData[monster->name.GetString()].maxHealth));
		m_maxStats["energy"] = std::max(m_maxStats["energy"], static_cast<float>(MonsterData[monster->name.GetString()].maxEnergy));
		m_maxStats["attack"] = std::max(m_maxStats["attack"], MonsterData[monster->name.GetString()].attack);
		m_maxStats["defense"] = std::max(m_maxStats["defense"], MonsterData[monster->name.GetString()].defense);
		m_maxStats["speed"] = std::max(m_maxStats["speed"], MonsterData[monster->name.GetString()].speed);
		m_maxStats["recovery"] = std::max(m_maxStats["recovery"], MonsterData[monster->name.GetString()].recovery);

		for (rapidjson::Value::ConstMemberIterator ability = monster->value["abilities"].MemberBegin(); ability != monster->value["abilities"].MemberEnd(); ++ability) {
			MonsterData[monster->name.GetString()].abilities[ability->name.GetString()] = ability->value.GetUint();
		}

		if (monster->value.HasMember("evolve")) {
			for (rapidjson::Value::ConstMemberIterator evolve = monster->value["evolve"].MemberBegin(); evolve != monster->value["evolve"].MemberEnd(); ++evolve) {
				MonsterData[monster->name.GetString()].evolve = { evolve->name.GetString() , evolve->value.GetUint() };
			}
		}
	}
	file.close();

	file.open("res/attack.json", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << "res/attack.json" << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper2(file);
	doc.ParseStream(streamWrapper2);

	for (rapidjson::Value::ConstMemberIterator attack = doc.MemberBegin(); attack != doc.MemberEnd(); ++attack) {
		m_attackData[attack->name.GetString()].target = attack->value["target"].GetString();
		m_attackData[attack->name.GetString()].amount = attack->value["amount"].GetFloat();
		m_attackData[attack->name.GetString()].cost = attack->value["cost"].GetFloat();
		m_attackData[attack->name.GetString()].element = attack->value["element"].GetString();
		m_attackData[attack->name.GetString()].animation = attack->value["animation"].GetString();
	}
	file.close();

	Monster.push_back({ "Charmadillo", 32u, false });
	Monster.push_back({ "Pluma", 15u, false });
	Monster.push_back({ "Finiette", 23u, false });
	Monster.push_back({ "Atrox", 30u, false });
	Monster.push_back({ "Sparchu", 24u, false });
	Monster.push_back({ "Gulfin", 17u, false });
	Monster.push_back({ "Jacana", 16u, false });
	Monster.push_back({ "Plumette", 9u, false });
	Monster.push_back({ "Cleaf", 3u, false });
	Monster.push_back({ "Charmadillo", 30u, false });

	m_colorMap["plant"] = { 0.39215f, 0.66274f, 0.56470f, 1.0f};
	m_colorMap["fire"] = { 0.97254f, 0.62745f, 0.37647f, 1.0f };
	m_colorMap["water"] = { 0.31372f, 0.69019f, 0.84705f, 1.0f };
	m_colorMap["normal"] = { 1.0f,1.0f, 1.0f, 1.0f };

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
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/bar_200.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/bar_100.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/bar_60.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/bar_10.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/bar_5.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/other/bar_2.png", false, true, false);

	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/ui/health.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/ui/energy.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/ui/attack.png", false, true, false);

	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/ui/defense.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/ui/speed.png", false, true, false);
	TileSetManager::Get().getTileSet("monster_icon").loadTileCpu("res/tmx/graphics/ui/recovery.png", false, true, false);

	TileSetManager::Get().getTileSet("monster_icon").loadTileSetGpu();
	//Spritesheet::Safe("monster_icon", TileSetManager::Get().getTileSet("monster_icon").getAtlas());
	m_atlasIcons = TileSetManager::Get().getTileSet("monster_icon").getAtlas();

	TextureAtlasCreator::Get().init(800u, 1600u);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Atrox.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Atrox_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Charmadillo_sheet.png", false, 200.0f, 200.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Charmadillo_highlight.png", false, 200.0f, 200.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Cindrill.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Cindrill_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Cleaf.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Cleaf_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Draem.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Draem_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Finiette_sheet.png", false, 200.0f, 200.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Finiette_highlight.png", false, 200.0f, 200.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Finsta.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Finsta_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Friolera.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Friolera_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Gulfin.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Gulfin_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Ivieron.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Ivieron_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Jacana.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Jacana_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Larvea.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Larvea_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Pluma_sheet.png", false, 200.0f, 200.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Pluma_highlight.png", false, 200.0f, 200.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Plumette.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Plumette_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Pouch.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Pouch_highlight.png", false, 192.0f, 192.0f, true, false);

	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Sparchu.png", false, 192.0f, 192.0f, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileSetCpu("res/tmx/graphics/monsters/Sparchu_highlight.png", false, 192.0f, 192.0f, true, false);

	//TileSetManager::Get().getTileSet("monster").loadTileCpu("res/tmx/graphics/other/bar_60.png", false, true, false);
	TileSetManager::Get().getTileSet("monster").loadTileCpu("res/tmx/graphics/other/empty.png", false, true, false);
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

	auto shader = Globals::shaderManager.getAssetPointer("list");
	shader->use();
	shader->loadVector("u_dimensions", Vector2f(m_viewWidth * 0.6f, m_viewHeight * 0.8f));
	shader->loadFloat("u_radius", 12.0f);
	shader->loadUnsignedInt("u_edge", Edge::ALL);
	
	m_surface.setShader(shader);

	Globals::spritesheetManager.getAssetPointer("null")->bind();
	m_surface.setPosition(m_viewWidth * 0.2f, m_viewHeight * 0.1f, 0.0f);
	m_surface.setScale(m_viewWidth * 0.6f, m_viewHeight * 0.8f, 1.0f);
	m_surface.draw(Vector4f(0.22745f, 0.21568f, 0.23137f, 1.0f));

	Spritesheet::Bind(m_atlasIcons);
	float itemHeigt = (0.8f * m_viewHeight) / static_cast<float>(m_visibleItems);
	float top = 0.9f * m_viewHeight;
	float bottom = 0.1f * m_viewHeight;
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	float lineHeightBold = static_cast<float>(Globals::fontManager.get("bold").lineHeight) * 0.05f;

	shader->loadVector("u_dimensions", Vector2f(m_viewWidth * 0.2f, itemHeigt));
	for (int i = 0; i < std::min(static_cast<int>(Monster.size()), m_visibleItems); i++) {

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
	for (int i = 0; i < std::min(static_cast<int>(Monster.size()), m_visibleItems); i++) {
		
		if (i != m_visibleItems - 1) {
			m_surface.setPosition(m_viewWidth * 0.2f, top - (i + 1) * itemHeigt, 0.0f);
			m_surface.setScale(m_viewWidth * 0.2f, 2.0f, 1.0f);
			m_surface.draw(rects[16], Vector4f(0.29411f, 0.28235f, 0.30196f, 1.0f));
		}

		const MonsterEntry& currentMonster = Monster[i + m_currentOffset];
		const TextureRect& rect = rects[MonsterData[currentMonster.name].graphic];
		m_surface.setPosition(m_viewWidth * 0.2f + 45.0f - rect.width * 0.5f, top - i * itemHeigt - 0.5f * (itemHeigt + rect.height), 0.0f);
		m_surface.setScale(rect.width, rect.height, 1.0f);
		m_surface.draw(rect, currentMonster.selected ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	}

	m_surface.setPosition(m_viewWidth * 0.4f - 4.0f, bottom, 0.0f);
	m_surface.setScale(4.0f, itemHeigt * static_cast<float>(m_visibleItems), 1.0f);
	m_surface.draw(rects[16], Vector4f(0.0f, 0.0f, 0.0f, 0.39216f));

	Globals::fontManager.get("dialog").bind();
	for (int i = 0; i < std::min(static_cast<int>(Monster.size()), m_visibleItems); i++) {
		const MonsterEntry& currentMonster = Monster[i + m_currentOffset];
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


	const MonsterEntry& currentMonster = Monster[m_currentOffset + m_currentSelected];
	shader->loadVector("u_dimensions", Vector2f(m_viewWidth * 0.4f, m_viewHeight * 0.3f));
	shader->loadFloat("u_radius", 12.0f);
	shader->loadUnsignedInt("u_edge", Edge::TOP_RIGHT);
	m_surface.setPosition(m_viewWidth * 0.4f, bottom + 0.5f * m_viewHeight, 0.0f);
	m_surface.setScale(m_viewWidth * 0.4f, m_viewHeight * 0.3f, 1.0f);
	m_surface.draw(m_colorMap[MonsterData[currentMonster.name].element]);

	Spritesheet::Bind(m_atlasMonster);
	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[MonsterData[currentMonster.name].graphic * 16 + m_currentFrame];
	m_surface.resetShader();
	m_surface.setPosition(0.4f * m_viewWidth  + 0.2f * m_viewWidth - 0.5f * rect.width, bottom + 0.5f * m_viewHeight + 0.15f * m_viewHeight - 0.5f * rect.height, 0.0f);
	m_surface.setScale(rect.width, rect.height, 1.0f);
	m_surface.draw(rect);

	Globals::fontManager.get("bold").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("bold"), 0.4f * m_viewWidth + 10.0f, top - 10.0f - lineHeightBold, currentMonster.name, Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.05f);
	Fontrenderer::Get().drawBuffer();

	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.4f * m_viewWidth + 10.0f, bottom + 0.5f * m_viewHeight + 10.0f, "Lvl: " + std::to_string(currentMonster.level), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.045f);
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.8f * m_viewWidth - 0.045f * Globals::fontManager.get("dialog").getWidth(MonsterData[currentMonster.name].element) - 10.0f, bottom + 0.5f * m_viewHeight + 10.0f, MonsterData[currentMonster.name].element, Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.045f);
	Fontrenderer::Get().drawBuffer();

	const TextureRect& bar100 = TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[17];
	drawBar({ 0.4f * m_viewWidth + 10.0f , bottom + 0.5f * m_viewHeight + 6.0f, 100.0f, 5.0f }, bar100, 700.0f, currentMonster.level * 150.0f, Vector4f(0.16862f, 0.16078f, 0.17255f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	
	drawBar({ 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f , bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - 7.5f, m_viewWidth * 0.4f * 0.45f, 30.0f }, 150.0f, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxHealth), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f), 2.0f);
	drawBar({ 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.75f - m_viewWidth * 0.4f * 0.45f * 0.5f , bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - 7.5f, m_viewWidth * 0.4f * 0.45f, 30.0f }, 150.0f, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxEnergy), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f), 2.0f);
	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f + 10.0f, bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - lineHeight * 0.5f + 7.5f, "HP: 150/" + std::to_string(currentMonster.level * MonsterData[currentMonster.name].maxHealth), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.045f);
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.75f - m_viewWidth * 0.4f * 0.45f * 0.5f + 10.0f, bottom + 0.5f * m_viewHeight - m_viewWidth * 0.03f - lineHeight * 0.5f + 7.5f, "EP: 150/" + std::to_string(currentMonster.level * MonsterData[currentMonster.name].maxEnergy), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.045f);
	Fontrenderer::Get().drawBuffer();

	float left = 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f;
	float width = m_viewWidth * 0.4f * 0.45f;
	float height =  0.5f * m_viewHeight - m_viewWidth * 0.03f - 7.5f;
	float statHeight = (height - 60.0f) / static_cast <float>(m_stats.size());


	Spritesheet::Bind(m_atlasIcons);
	m_surface.setPosition(left, bottom + 30.0f, 0.0f);
	m_surface.setScale(width, height - 60.0f, 1.0f);
	//m_surface.draw(rects[16], Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	for (size_t i = 0; i < m_stats.size(); i++) {
		const TextureRect& rect = TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[23 + i];
		m_surface.setPosition(left + 5.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight + rect.height * 0.5f, 0.0f);
		m_surface.setScale(rect.width, rect.height, 1.0f);
		m_surface.draw(rect);
	}

	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), left, bottom + 30.0f + height - 60.0f - lineHeight * 0.5f, "Stats", Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.045f);
	
	for (size_t i = 0; i < m_stats.size(); i++) {
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), left + 30.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight + lineHeight * 0.5f - 8.0f, m_stats[i], Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.045f);
	}
	Fontrenderer::Get().drawBuffer();

	const TextureRect& bar200 = TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[17];
	for (size_t i = 0; i < m_stats.size(); i++) {
		if(i == 0)
			drawBar({ left + 30.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight - 4.0f, width - 30.0f, 5.0f }, bar200, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxHealth), static_cast<float>(currentMonster.level * m_maxStats[m_stats[i]]), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
		else if(i == 1)
			drawBar({ left + 30.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight - 4.0f, width - 30.0f, 5.0f }, bar200, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxEnergy), static_cast<float>(currentMonster.level * m_maxStats[m_stats[i]]), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
		else if (i == 2)
			drawBar({ left + 30.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight - 4.0f, width - 30.0f, 5.0f }, bar200, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].attack), static_cast<float>(currentMonster.level * m_maxStats[m_stats[i]]), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
		else if (i == 3)
			drawBar({ left + 30.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight - 4.0f, width - 30.0f, 5.0f }, bar200, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].defense), static_cast<float>(currentMonster.level * m_maxStats[m_stats[i]]), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
		else if (i == 4)
			drawBar({ left + 30.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight - 4.0f, width - 30.0f, 5.0f }, bar200, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].speed), static_cast<float>(currentMonster.level * m_maxStats[m_stats[i]]), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
		else if (i == 5)
			drawBar({ left + 30.0f, bottom + 30.0f + height - 60.0f - (i + 1) * statHeight - 4.0f, width - 30.0f, 5.0f }, bar200, static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].recovery), static_cast<float>(currentMonster.level * m_maxStats[m_stats[i]]), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	}

	left = 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.75f - m_viewWidth * 0.4f * 0.45f * 0.5f;

	Globals::fontManager.get("dialog").bind();
	Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), left, bottom + 30.0f + height - 60.0f - lineHeight * 0.5f, "Ability", Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f), 0.045f);

	Spritesheet::Bind(m_atlasIcons);
	int index = 0;
	float x, y;
	std::string lastAbility;

	m_surface.setShader(shader);

	shader->use();
	
	shader->loadFloat("u_radius", 4.0f);
	shader->loadUnsignedInt("u_edge", Edge::ALL);

	for (auto& ability : MonsterData[currentMonster.name].abilities) {
		if (currentMonster.level < ability.second)
			continue;
		x = left + index % 2 * (Globals::fontManager.get("dialog").getWidth(lastAbility) * 0.045f + 20.0f);
		y = bottom + 30.0f + height - 60.0f - (int(index / 2) + 1) * statHeight - 4.0f;

		shader->loadVector("u_dimensions", Vector2f(Globals::fontManager.get("dialog").getWidth(ability.first) * 0.045f + 10.0f, lineHeight + 10.0f));

		const TextureRect& rect = TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[16];
		m_surface.setPosition(x - 5.0f, y - 5.0f, 0.0f);
		m_surface.setScale(Globals::fontManager.get("dialog").getWidth(ability.first) * 0.045f + 10.0f, lineHeight + 10.0f, 1.0f);
		m_surface.draw(m_colorMap[m_attackData[ability.first].element]);

		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), x, y, ability.first, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.045f);
		lastAbility = ability.first;
		index++;
	}
	Globals::fontManager.get("dialog").bind();
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
		if (static_cast<int>(Monster.size()) - m_currentOffset > m_visibleItems && m_currentSelected == m_visibleItems - 1) {
			resetAnimation();
			m_currentOffset++;
		}

		if (m_currentSelected < std::min(static_cast<int>(Monster.size() - 1), m_visibleItems - 1)) {
			resetAnimation();
			m_currentSelected++;
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		Monster[m_currentOffset + m_currentSelected].selected = !Monster[m_currentOffset + m_currentSelected].selected;
		if (m_beforeSelected != m_currentOffset + m_currentSelected) {
			if (m_beforeSelected != -1) {
				Monster[m_currentOffset + m_currentSelected].selected = false;
				Monster[m_beforeSelected].selected = false;
				std::iter_swap(Monster.begin() + m_beforeSelected, Monster.begin() + m_currentOffset + m_currentSelected);
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

void MonsterIndex::drawBar(const Rect& rect, const TextureRect& textureRect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color) {
	float ratio = rect.width / maxValue;
	float progress = std::max(0.0f, std::min(rect.width, value * ratio));

	Spritesheet::Bind(m_atlasIcons);
	m_surfaceBar.resetShader();

	m_surfaceBar.setPosition(rect.posX, rect.posY, 0.0f);
	m_surfaceBar.setScale(rect.width, rect.height, 1.0f);
	m_surfaceBar.draw(textureRect, bgColor);

	m_surfaceBar.setPosition(rect.posX, rect.posY, 0.0f);
	m_surfaceBar.setScale(progress, rect.height, 1.0f);
	m_surfaceBar.draw(textureRect, color);
}