#include <rapidjson/document.h>
#include <rapidjson/istreamwrapper.h>
#include <engine/input/Keyboard.h>
#include <engine/Fontrenderer.h>
#include <engine/TileSet.h>

#include "MonsterIndex.h"
#include "Globals.h"

std::unordered_map<std::string, MonsterData> MonsterIndex::MonsterData;
std::vector<MonsterEntry> MonsterIndex::Monsters;
std::unordered_map<std::string, AttackData> MonsterIndex::_AttackData;
std::unordered_map<std::string, Vector4f> MonsterIndex::ColorMap;

void MonsterEntry::resetStats() {
	selected = false;
	health = static_cast<float>(level * MonsterIndex::MonsterData[name].maxHealth);
	energy = static_cast<float>(level * MonsterIndex::MonsterData[name].maxEnergy);
}

void MonsterEntry::unselect() {
	selected = false;
}

MonsterIndex::MonsterIndex() : Surface(),
m_visibleItems(6), 
m_viewWidth(0.0f), 
m_viewHeight(0.0f), 
m_currentOffset(0), 
m_currentSelected(0), 
m_beforeSelected(-1),
m_elapsedTime(0.0f),
m_currentFrame(0),
m_frameCount(4),
m_rotate(false){

	std::ifstream file("res/monster.json", std::ios::in);
	if (!file.is_open()) {
		std::cerr << "Could not open file: " << "res/monster.json" << std::endl;
	}

	rapidjson::IStreamWrapper streamWrapper(file);
	rapidjson::Document doc;
	doc.ParseStream(streamWrapper);

	m_stateLabels[0] = "health_stat";
	m_stateLabels[1] = "energy_stat";
	m_stateLabels[2] = "attack";
	m_stateLabels[3] = "defense";
	m_stateLabels[4] = "speed";
	m_stateLabels[5] = "recovery";

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
		_AttackData[attack->name.GetString()].target = attack->value["target"].GetString();
		_AttackData[attack->name.GetString()].amount = attack->value["amount"].GetFloat();
		_AttackData[attack->name.GetString()].cost = attack->value["cost"].GetFloat();
		_AttackData[attack->name.GetString()].element = attack->value["element"].GetString();
		_AttackData[attack->name.GetString()].animation = attack->value["animation"].GetString();
		_AttackData[attack->name.GetString()].graphic = attack->value["graphic"].GetUint();
	}
	file.close();

	Monsters.push_back({ "Friolera", 25u , 3.0f, 200.0f, 0.0f, false });
	Monsters.push_back({ "Atrox", 30u , 3.0f, 15.0f, 0.0f, false });
	//Monsters.push_back({ "Sparchu", 24u , 3.0f, 30.0f, 0.0f, false });
	Monsters.push_back({ "Sparchu", 34u , 3.0f, 30.0f, 0.0f, false });
	Monsters.push_back({ "Pouch", 23u , 3.0f, 30.0f, 0.0f, false });
	Monsters.push_back({ "Gulfin", 17u, 3.0f, 30.0f, false  });
	Monsters.push_back({ "Jacana", 16u, 300.0f, 0.0f, false });
	Monsters.push_back({ "Plumette", 9u , 300.0f, 0.0f, false });
	Monsters.push_back({ "Cleaf", 3u , 300.0f, 0.0f, false });
	Monsters.push_back({ "Charmadillo", 30u , 300.0f, 0.0f, false });

	resetStats();

	ColorMap["plant"] = { 0.39215f, 0.66274f, 0.56470f, 1.0f};
	ColorMap["fire"] = { 0.97254f, 0.62745f, 0.37647f, 1.0f };
	ColorMap["water"] = { 0.31372f, 0.69019f, 0.84705f, 1.0f };
	ColorMap["normal"] = { 1.0f,1.0f, 1.0f, 1.0f };

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
	TileSetManager::Get().getTileSet("monster_icon").setLinear();
	for (int i = 0; i < 16; i++) {
		TileSetManager::Get().getTileSet("monster_icon").shrinkTextureRectX(i, 10.0f / 1024.0f);
	}

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
	TileSetManager::Get().getTileSet("monster").loadTileCpu("res/tmx/graphics/ui/cross.png", false, true, false);
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
	//Surface::draw();

	std::shared_ptr<WidgetMH> currentWidget;
	{
		currentWidget = std::static_pointer_cast<WidgetMH>(getChildren().front());
		currentWidget->draw();
	}
	{
		//right surface
		std::list<std::shared_ptr<NodeUI>>::reverse_iterator  it;
		std::shared_ptr<WidgetMH> subWidget;
		for (it = currentWidget->getChildren().rbegin(); it != currentWidget->getChildren().rend(); ++it) {
			subWidget = std::static_pointer_cast<WidgetMH>(*it);
			subWidget->draw();
		}
		currentWidget = subWidget;
	}
	{	
		//top-rigth content	
		for (auto& children : currentWidget->getChildren()) {
			currentWidget = std::static_pointer_cast<WidgetMH>(children);
			currentWidget->draw();
		}	
	}

	{
		//left surface
		currentWidget = std::static_pointer_cast<WidgetMH>(getChildren().back());
		currentWidget->draw();
		std::list<std::shared_ptr<NodeUI>>::reverse_iterator  it;
		std::shared_ptr<WidgetMH> subWidget;
		for (it = currentWidget->getChildren().rbegin(); it != currentWidget->getChildren().rend(); ++it) {
			subWidget = std::static_pointer_cast<WidgetMH>(*it);
			subWidget->draw();

			for (auto& children : subWidget->getChildren()) {
				std::static_pointer_cast<WidgetMH>(children)->draw();
			}		
		}
	}
}

void MonsterIndex::update(float dt) {
	m_elapsedTime += 6.0f * dt;
	m_currentFrame = static_cast <int>(std::floor(m_elapsedTime));
	if (m_currentFrame > m_frameCount - 1) {
		m_currentFrame = 0;
		m_elapsedTime -= static_cast <float>(m_frameCount);
	}

	const MonsterEntry& currentMonster = Monsters[m_currentOffset + m_currentSelected];
	findChild<Label>("headline")->setLabel(currentMonster.name);
	findChild<Label>("level")->setLabel("Lvl: " + std::to_string(currentMonster.level));
	Label* label = findChild<Label>("element");
	label->setLabel(MonsterData[currentMonster.name].element);
	label->setOffsetX(-0.045f * Globals::fontManager.get("dialog").getWidth(MonsterData[currentMonster.name].element));

	label = findChild<Label>("health");
	label->setLabel("HP: " + Fontrenderer::FloatToString(currentMonster.health, 0) + "/" + std::to_string(currentMonster.level * MonsterData[currentMonster.name].maxHealth));

	label = findChild<Label>("energy");
	label->setLabel("EP: " + Fontrenderer::FloatToString(currentMonster.energy, 0) + "/" + std::to_string(currentMonster.level * MonsterData[currentMonster.name].maxEnergy));

	findChild<Surface>("top-right")->setColor(ColorMap[MonsterData[currentMonster.name].element]);
	findChild<IconAnimated>("icon")->setCurrentFrame(MonsterData[currentMonster.name].graphic * 16 + m_currentFrame);

	BarUI* bar = findChild<BarUI>(m_stateLabels[0]);
	bar->setValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxHealth));
	bar->setMaxValue(static_cast<float>(static_cast<float>(currentMonster.level * m_maxStats[m_stats[0]])));

	bar = findChild<BarUI>(m_stateLabels[1]);
	bar->setValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxEnergy));
	bar->setMaxValue(static_cast<float>(static_cast<float>(currentMonster.level * m_maxStats[m_stats[1]])));

	bar = findChild<BarUI>(m_stateLabels[2]);
	bar->setValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].attack));
	bar->setMaxValue(static_cast<float>(static_cast<float>(currentMonster.level * m_maxStats[m_stats[2]])));

	bar = findChild<BarUI>(m_stateLabels[3]);
	bar->setValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].defense));
	bar->setMaxValue(static_cast<float>(static_cast<float>(currentMonster.level * m_maxStats[m_stats[3]])));

	bar = findChild<BarUI>(m_stateLabels[4]);
	bar->setValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].speed));
	bar->setMaxValue(static_cast<float>(static_cast<float>(currentMonster.level * m_maxStats[m_stats[4]])));

	bar = findChild<BarUI>(m_stateLabels[5]);
	bar->setValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].recovery));
	bar->setMaxValue(static_cast<float>(static_cast<float>(currentMonster.level * m_maxStats[m_stats[5]])));
	
	bar = findChild<BarUI>("level");
	bar->setValue(currentMonster.experience);
	bar->setMaxValue(currentMonster.level * 150.0f);

	bar = findChild<BarUI>("health");
	bar->setValue(currentMonster.health);
	bar->setMaxValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxHealth));

	bar = findChild<BarUI>("energy");
	bar->setValue(currentMonster.energy);
	bar->setMaxValue(static_cast<float>(currentMonster.level * MonsterData[currentMonster.name].maxEnergy));

	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("monster_icon").getTextureRects();
	Surface *surface = findChild<Surface>("left"), *subSurface;
	IconAnimated *iconAnimated;
	for (int i = 0; i < std::min(static_cast<int>(Monsters.size()), m_visibleItems); i++) {
		Vector4f color = i == m_currentSelected ? Vector4f(0.78431f, 0.78431f, 0.78431f, 1.0f) : Vector4f(0.22745f, 0.21568f, 0.23137f, 1.0f);
		subSurface = surface->findChild<Surface>(i);
		subSurface->setColor(color);

		const MonsterEntry& currentMonster = Monsters[i + m_currentOffset];
		const TextureRect& rect = rects[MonsterData[currentMonster.name].graphic];
		iconAnimated = surface->findChild<IconAnimated>(i);
		iconAnimated->setCurrentFrame(MonsterData[currentMonster.name].graphic);
		iconAnimated->setPosition(0.0f, 0.5f);
		iconAnimated->translateRelative(45.0f - 0.5f * rect.width, -0.5f * rect.height);
		iconAnimated->setScaleAbsolute(rect.width, rect.height);
		iconAnimated->setColor(currentMonster.selected ? Vector4f(1.0f, 0.0f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		iconAnimated->updateWorldTransformation();

		label = surface->findChild<Label>(i);
		label->setTextColor(currentMonster.selected ? Vector4f(1.0f, 0.84313f, 0.0f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		label->setLabel(currentMonster.name);
	}

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_R)) {
		m_rotate = !m_rotate;
	}

	if(m_rotate)
		rotate(10.0f * dt);
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
		eraseAbilities();	
		int selectedBefore = m_currentSelected;
		int offsetBefore = m_currentOffset;

		if (m_currentSelected == 0) {
			if (m_currentOffset == 0) {
				m_currentOffset = std::max(static_cast<int>(Monsters.size()) - m_visibleItems, 0);
				m_currentSelected = static_cast<int>(Monsters.size()) - m_currentOffset - 1;
			}else {
				m_currentOffset--;
			}
		}else {
			m_currentSelected--;
		}

		if (selectedBefore != m_currentSelected || offsetBefore != m_currentOffset) {
			resetAnimation();
		}

		addAbilities();
	}

	if (keyboard.keyPressed(Keyboard::KEY_DOWN)) {
		eraseAbilities();
		int selectedBefore = m_currentSelected;
		int offsetBefore = m_currentOffset;

		if (m_currentSelected == std::min(static_cast<int>(Monsters.size() - 1), m_visibleItems - 1)) {

			if (m_currentOffset == std::max(static_cast<int>(Monsters.size()) - m_visibleItems, 0)) {
				m_currentOffset = 0;
				m_currentSelected = 0;
			}else {
				m_currentOffset++;
			}

		}else {
			m_currentSelected++;
		}

		if (selectedBefore != m_currentSelected || offsetBefore != m_currentOffset) {
			resetAnimation();
		}
		addAbilities();
	}

	if (keyboard.keyPressed(Keyboard::KEY_SPACE)) {
		Monsters[m_currentOffset + m_currentSelected].selected = !Monsters[m_currentOffset + m_currentSelected].selected;
		if (m_beforeSelected != m_currentOffset + m_currentSelected) {
			if (m_beforeSelected != -1) {
				Monsters[m_currentOffset + m_currentSelected].selected = false;
				Monsters[m_beforeSelected].selected = false;
				std::iter_swap(Monsters.begin() + m_beforeSelected, Monsters.begin() + m_currentOffset + m_currentSelected);
				m_beforeSelected = -1;
				eraseAbilities();
				addAbilities();
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


void MonsterIndex::resetStats() {
	std::for_each(Monsters.begin(), Monsters.end(), std::mem_fn(&MonsterEntry::resetStats));
}

void MonsterIndex::unselect() {
	std::for_each(Monsters.begin(), Monsters.end(), std::mem_fn(&MonsterEntry::unselect));
}

void MonsterIndex::reset() {
	eraseAbilities();
	eraseMonsters();
	m_currentSelected = 0;
	m_beforeSelected = -1;
	m_currentFrame = 0;
	m_currentOffset = 0;
	unselect();
}

void MonsterIndex::initUI(float viewWidth, float viewHeight) {
	setPosition(viewWidth * 0.2f, viewHeight * 0.1f);
	setScale(viewWidth * 0.6f, viewHeight * 0.8f);
	setOrigin(viewWidth * 0.3f, viewHeight * 0.4f);
	//setOrientation(90.0f);
	setShader(Globals::shaderManager.getAssetPointer("list"));
	setColor(Vector4f(0.22745f, 0.21568f, 0.23137f, 1.0f));
	setBorderRadius(12.0f);

	Surface* surface = addChild<Surface>();
	surface->setPosition(0.333333f, 0.0f);
	surface->setScale(0.666666f, 1.0f);
	surface->updateWorldTransformation();
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setName("right");
	surface->setBorderRadius(12.0f);
	surface->setEdge(Edge::EDGE_RIGHT);
	surface->setColor(Vector4f(0.16862f, 0.16078f, 0.17254f, 1.0f));

	surface = surface->addChild<Surface>();
	surface->setPosition(0.0f, 0.625f);
	surface->setScale(1.0f, 0.375f);
	surface->updateWorldTransformation();
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setName("top-right");
	surface->setBorderRadius(12.0f);
	surface->setEdge(Edge::TOP_RIGHT);
	surface->setColor(Vector4f(0.0f, 1.0f, 1.0f, 1.0f));
	
	//add Icon
	IconAnimated* iconAnimated = surface->addChild<IconAnimated>(TileSetManager::Get().getTileSet("monster").getTextureRects());
	iconAnimated->setPosition(0.5f, 0.5f);
	iconAnimated->translateRelative(-96.0f, -96.0f);
	iconAnimated->scaleAbsolute(192.0f, 192.0f);
	iconAnimated->updateWorldTransformation();
	iconAnimated->setName("icon");
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("monster").getAtlas());

	float lineHeightBold = static_cast<float>(Globals::fontManager.get("bold").lineHeight) * 0.05f;
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	Label* label = surface->addChild<Label>(Globals::fontManager.get("bold"));
	label->setPosition(0.0f, 1.0f);
	label->setScale(1.0f, 1.0f);
	label->translateRelative(10.0f, -10.0f - lineHeightBold);
	label->updateWorldTransformation();
	label->setName("headline");
	label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	label->setSize(0.05f);
	
	label = surface->addChild<Label>(Globals::fontManager.get("dialog"));
	label->setPosition(0.0f, 0.0f);
	label->setScale(1.0f, 1.0f);
	label->translateRelative(10.0f, 10.0f);
	label->updateWorldTransformation();
	label->setName("level");
	label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	label->setSize(0.045f);

	label = surface->addChild<Label>(Globals::fontManager.get("dialog"));
	label->setPosition(1.0f, 0.0f);
	label->setScale(1.0f, 1.0f);
	label->translateRelative(-10.0f, 10.0f);
	label->updateWorldTransformation();
	label->setName("element");
	label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	label->setSize(0.045f);
	
	BarUI* bar = surface->addChild<BarUI>(TileSetManager::Get().getTileSet("bars"));
	bar->setPosition(0.0f, 0.0f);
	bar->translateRelative(10.0f, 4.0f);
	bar->updateWorldTransformation();
	bar->setName("level");
	bar->setRadius(0.0f);
	bar->setBgColor(Vector4f(0.16862f, 0.16078f, 0.17255f, 1.0f));
	bar->setColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	bar->setWidth(100.0f);
	bar->setHeight(5.0f);

	//float left = 0.4f * m_viewWidth + m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f;
	float height = 0.5f * m_viewHeight - m_viewWidth * 0.03f - 7.5f;
	float statHeight = (height - 60.0f) / static_cast <float>(m_stats.size());
	float left =  m_viewWidth * 0.4f * 0.25f - m_viewWidth * 0.4f * 0.45f * 0.5f;
	surface = findChild<Surface>("right");

	label = surface->addChild<Label>(Globals::fontManager.get("dialog"));
	label->setPosition(0.025f, 0.5f);
	label->translateRelative(10.0f, 28.5f);
	label->updateWorldTransformation();
	label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	label->setSize(0.045f);
	label->setName("health");

	label = surface->addChild<Label>(Globals::fontManager.get("dialog"));
	label->setPosition(0.525f, 0.5f);
	label->translateRelative(10.0f, 28.5f);
	label->updateWorldTransformation();
	label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	label->setSize(0.045f);
	label->setName("energy");

	bar = surface->addChild<BarUI>(TileSetManager::Get().getTileSet("bars"));
	bar->setPosition(0.025f, 0.5f);
	bar->translateRelative(0.0f, 25.0f);
	bar->updateWorldTransformation();
	bar->setName("health");
	bar->setRadius(2.0f);
	bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	bar->setColor(Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f));
	bar->setWidth(m_viewWidth * 0.4f * 0.45f);
	bar->setHeight(30.0f);

	bar = surface->addChild<BarUI>(TileSetManager::Get().getTileSet("bars"));
	bar->setPosition(0.525f, 0.5f);
	bar->translateRelative(0.0f, 25.0f);
	bar->updateWorldTransformation();
	bar->setName("energy");
	bar->setRadius(2.0f);
	bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	bar->setColor(Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f));
	bar->setWidth(m_viewWidth * 0.4f * 0.45f);
	bar->setHeight(30.0f);

	label = surface->addChild<Label>(Globals::fontManager.get("dialog"));
	label->setPosition(0.025f, 0.5f);
	label->translateRelative(0.0f, -lineHeight * 0.5f);
	label->updateWorldTransformation();
	label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	label->setSize(0.045f);
	label->setLabel("Stats");

	label = surface->addChild<Label>(Globals::fontManager.get("dialog"));
	label->setPosition(0.525f, 0.5f);
	label->translateRelative(0.0f, -lineHeight * 0.5f);
	label->updateWorldTransformation();
	label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
	label->setSize(0.045f);
	label->setLabel("Ability");

	float width = m_viewWidth * 0.4f * 0.45f;
	for (size_t i = 0; i < m_stats.size(); i++) {
		const TextureRect& rect = TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[23 + i];
		Icon* icon = surface->addChild<Icon>(rect);
		icon->setPosition(0.025f, 0.5f);
		icon->translateRelative(5.0f, -30.0f -rect.height * 0.5f - i * statHeight);
		icon->scaleAbsolute(rect.width, rect.height);
		icon->updateWorldTransformation();
		icon->setSpriteSheet(TileSetManager::Get().getTileSet("monster_icon").getAtlas());

		label = surface->addChild<Label>(Globals::fontManager.get("dialog"));
		label->setPosition(0.025f, 0.5f);
		label->translateRelative(30.0f, -30.0f - i * statHeight - lineHeight * 0.4f + 2.0f);
		label->updateWorldTransformation();
		label->setTextColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
		label->setSize(0.045f);
		label->setLabel(m_stats[i]);

		bar = surface->addChild<BarUI>(TileSetManager::Get().getTileSet("bars"));
		bar->setPosition(0.025f, 0.5f);	
		bar->translateRelative(30.0f, -37.5f - i * statHeight - lineHeight * 0.4f + 2.0f);
		bar->updateWorldTransformation();
		bar->setName(m_stateLabels[i]);
		bar->setRadius(0.0f);
		bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		bar->setColor(Vector4f(0.95686f, 0.99608f, 0.98039f, 1.0f));
		bar->setWidth(width - 30.0f);
		bar->setHeight(5.0f);
	}

	surface = addChild<Surface>();
	surface->setPosition(0.0f, 0.0f);
	surface->setScale(0.333333f, 1.0f);
	surface->updateWorldTransformation();
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setName("left");
	surface->setBorderRadius(12.0f);
	surface->setEdge(Edge::EDGE_LEFT);
	surface->setColor(Vector4f(0.22745f, 0.21568f, 0.23137f, 1.0f));

	Surface* horinzontalBar = surface->addChild<Surface>();
	horinzontalBar->setPosition(1.0f - 0.0125f, 0.0f);
	horinzontalBar->setScale(0.0125f, 1.0f);
	horinzontalBar->updateWorldTransformation();
	horinzontalBar->setShader(Globals::shaderManager.getAssetPointer("list"));
	horinzontalBar->setBorderRadius(0.0f);
	horinzontalBar->setEdge(Edge::EDGE_NONE);
	horinzontalBar->setColor(Vector4f(0.0f, 0.0f, 0.0f, 0.39216f));
}

void MonsterIndex::addAbilities() {
	Surface* surface = findChild<Surface>("right");
	const MonsterEntry& currentMonster = Monsters[m_currentOffset + m_currentSelected];
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	int index = 0;
	float x, y, statHeight = 42.35f;
	std::string lastAbility;
	TextFieldMH* textField;
	for (auto& ability : MonsterData[currentMonster.name].abilities) {
		if (currentMonster.level < ability.second)
			continue;
		x = index % 2 * (Globals::fontManager.get("dialog").getWidth(lastAbility) * 0.045f + 20.0f);
		y = -(int(index / 2) + 1) * statHeight - 4.0f;

		textField = surface->addChild<TextFieldMH>(Globals::fontManager.get("dialog"));
		textField->setPosition(0.525f, 0.5f);
		textField->translateRelative(x, y);
		textField->translateRelative(0.0f, -lineHeight * 0.5f);
		textField->updateWorldTransformation();
		textField->setTextColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
		textField->setSize(0.045f);
		textField->setOffsetX(5.0f);
		textField->setOffsetY(5.0f);
		textField->setLabel(ability.first);
		textField->setBackgroundColor(ColorMap[_AttackData[ability.first].element]);
		textField->setEdge(Edge::ALL);
		textField->setBorderRadius(4.0f);
		textField->setPaddingX(10.0f);
		textField->setPaddingY(10.0f);
		textField->setShader(Globals::shaderManager.getAssetPointer("list"));
		lastAbility = ability.first;
		index++;
	}
}

void MonsterIndex::eraseAbilities() {
	Surface* surface = findChild<Surface>("right");
	surface->eraseChildren<TextFieldMH>();
}

void MonsterIndex::addMonsters() {
	Surface* surface = findChild<Surface>("left");
	float lineHeight = static_cast<float>(Globals::fontManager.get("dialog").lineHeight) * 0.045f;
	const std::vector<TextureRect>& rects = TileSetManager::Get().getTileSet("monster_icon").getTextureRects();
	Surface *subSurface; IconAnimated* animatedIcon; Label* label; Icon* icon;

	float itemHeigt = 1.0f / static_cast<float>(m_visibleItems);
	for (int i = 0; i < std::min(static_cast<int>(Monsters.size()), m_visibleItems); i++) {
		subSurface = surface->addChild<Surface>();

		if (i == 0) {
			subSurface->setEdge(Edge::TOP_LEFT);
		}else if (i == m_visibleItems - 1) {
			subSurface->setEdge(Edge::BOTTOM_LEFT);
		}else {
			subSurface->setEdge(Edge::EDGE_NONE);
		}

		subSurface->setPosition(0.0f, 1.0f - static_cast<float>(i + 1) * itemHeigt);
		subSurface->setScale(1.0f, 0.166666f);
		subSurface->updateWorldTransformation();
		subSurface->setShader(Globals::shaderManager.getAssetPointer("list"));
		subSurface->setBorderRadius(12.0f);
		subSurface->setColor(Vector4f(0.22745f, 0.21568f, 0.23137f, 1.0f));
		subSurface->setIndex(i);

		const MonsterEntry& currentMonster = Monsters[i + m_currentOffset];
		const TextureRect& rect = rects[MonsterData[currentMonster.name].graphic];
		animatedIcon = subSurface->addChild<IconAnimated>(rects);
		animatedIcon->setPosition(0.0f, 0.5f);
		animatedIcon->translateRelative(45.0f - 0.5f * rect.width, -0.5f * rect.height);
		animatedIcon->scaleAbsolute(rect.width, rect.height);
		animatedIcon->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		animatedIcon->updateWorldTransformation();
		animatedIcon->setSpriteSheet(TileSetManager::Get().getTileSet("monster_icon").getAtlas());
		animatedIcon->setCurrentFrame(MonsterData[currentMonster.name].graphic);
		animatedIcon->setIndex(i);

		if (i != m_visibleItems - 1) {
			icon = subSurface->addChild<Icon>(rects[16]);
			icon->setPosition(0.0f, 0.0f);
			icon->setScale(1.0f, 0.02f);
			icon->updateWorldTransformation();
			icon->setColor(Vector4f(0.29411f, 0.28235f, 0.30196f, 1.0f));
			icon->setSpriteSheet(TileSetManager::Get().getTileSet("monster_icon").getAtlas());
		}

		label = subSurface->addChild<Label>(Globals::fontManager.get("dialog"));
		label->setPosition(0.0f, 0.5f);
		label->translateRelative(90.0f, -0.5f * lineHeight);
		label->updateWorldTransformation();
		label->setTextColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		label->setSize(0.045f);
		label->setLabel(currentMonster.name);
		label->setIndex(i);
	}
}

void MonsterIndex::eraseMonsters(){
	Surface* surface = findChild<Surface>("left");
	surface->eraseAllChildren();
}