#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>
#include <UI/Surface.h>
#include "Monster.h"
#include "Globals.h"
#include "MonsterIndex.h"

std::random_device Monster::RandomDevice;
std::uniform_real_distribution<float> Monster::Distribution(-1.0f, 1.0f);

Monster::Monster(Cell& cell, MonsterEntry& monsterEntry, const Vector2f& pos) : SpriteEntity(cell), monsterEntry(monsterEntry),
m_animationSpeed(6.0f + Distribution(RandomDevice)),
m_maxExperience(150.0f * static_cast<float>(monsterEntry.level)),
m_maxHealth(static_cast<float>(monsterEntry.level * MonsterIndex::MonsterData[monsterEntry.name].maxHealth)),
m_maxEnergy(static_cast<float>(monsterEntry.level * MonsterIndex::MonsterData[monsterEntry.name].maxEnergy)),
m_initiative(0.0f),
m_speed(static_cast<float>(monsterEntry.level) * MonsterIndex::MonsterData[monsterEntry.name].speed),
m_pause(false),
m_highlight(false),
m_coverWithMask(false),
m_attackOffset(0u),
m_canAttack(true),
m_defending(false),
m_delayedKill(false),
m_highlightTimer(this, CALL_BACK_1),
m_delayedKillTimer(this, CALL_BACK_2),
m_showMissingTimer(this, CALL_BACK_3),
m_killed(false)
{
	m_direction.set(0.0f, 0.0f);
	canAttack();
	setPosition(pos[0] / 1280.0f, pos[1] / 720.0f);
	setScale(1280.0f, 720.0f);
	initUI();
	setScale(1.0f, 1.0f);
}

Monster::Monster(Monster const& rhs) : Empty(rhs), SpriteEntity(rhs.cell), monsterEntry(rhs.monsterEntry) {
	m_animationSpeed = rhs.m_animationSpeed;
	m_maxExperience = rhs.m_maxExperience;	
	m_maxHealth = rhs.m_maxHealth;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_defending = rhs.m_defending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setReceiver(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setReceiver(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setReceiver(this);
}

Monster::Monster(Monster&& rhs) : Empty(rhs), SpriteEntity(rhs.cell), monsterEntry(rhs.monsterEntry) {
	m_animationSpeed = rhs.m_animationSpeed;
	m_maxExperience = rhs.m_maxExperience;
	m_maxHealth = rhs.m_maxHealth;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_defending = rhs.m_defending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setReceiver(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setReceiver(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setReceiver(this);
}

Monster& Monster::operator=(const Monster& rhs) {
	SpriteEntity::operator=(rhs);
	cell = rhs.cell;
	monsterEntry = rhs.monsterEntry;

	m_animationSpeed = rhs.m_animationSpeed;
	m_maxExperience = rhs.m_maxExperience;
	m_maxHealth = rhs.m_maxHealth;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_defending = rhs.m_defending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setReceiver(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setReceiver(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setReceiver(this);
	return *this;
}

Monster& Monster::operator=(Monster&& rhs) {
	SpriteEntity::operator=(rhs);
	cell = rhs.cell;
	monsterEntry = rhs.monsterEntry;

	m_animationSpeed = rhs.m_animationSpeed;
	m_maxExperience = rhs.m_maxExperience;
	m_maxHealth = rhs.m_maxHealth;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_defending = rhs.m_defending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setReceiver(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setReceiver(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setReceiver(this);
	return *this;
}

Monster::~Monster() {

}

void Monster::drawDefault() {
	drawTree();
}

void Monster::update(float dt) {

	m_highlightTimer.update(dt);
	m_delayedKillTimer.update(dt);
	m_showMissingTimer.update(dt);

	m_elapsedTime += m_animationSpeed * dt;
	cell.currentFrame = m_startFrame + static_cast<int>(std::floor(m_elapsedTime));
	if (cell.currentFrame - m_startFrame > m_frameCount - 1) {
		cell.currentFrame = m_startFrame;
		m_elapsedTime -= static_cast<float>(m_frameCount);
		if (m_attackOffset > 0)
			m_attackOffset = 0u;
	}

	ui::IconAnimated* iconAnimated = findChild<ui::IconAnimated>("image");
	iconAnimated->setCurrentFrame(cell.currentFrame + m_attackOffset);
	iconAnimated->setCurrentFrame(m_coverWithMask ? -1 : cell.currentFrame + m_attackOffset);

	iconAnimated = findChild<ui::IconAnimated>("highlight");
	iconAnimated->setCurrentFrame(m_highlight ? cell.currentFrame + m_attackOffset + 8u : -1);

	if (m_pause)
		return;

	m_initiative += m_speed * dt;

	findChild<ui::Bar>("initiative")->setValue(m_initiative);
}

float Monster::getInitiative() {
	return m_initiative;
}

void Monster::setInitiative(float initiative) {
	m_initiative = initiative;
	findChild<ui::Bar>("initiative")->setValue(m_initiative);
}

void Monster::pause() {
	m_pause = true;
}

void Monster::unPause() {
	m_pause = false;
}

void Monster::setHighlight(bool highlight) {
	m_highlight = highlight;
	m_coverWithMask = false;
}

void Monster::setHighlight(bool highlight, unsigned int milli) {
	m_highlight = highlight;
	if (m_highlight) {
		m_coverWithMask = true;
		m_highlightTimer.start(milli, false);
	}
}

const std::string& Monster::getName() const {
	return monsterEntry.get().name;
}

const unsigned int Monster::getLevel() const {
	return monsterEntry.get().level;
}

const float Monster::getEnergy() const {
	return monsterEntry.get().energy;
}

const float Monster::getHealth() const {
	return monsterEntry.get().health;
}

void Monster::reduceEnergy(const AttackData& attack) {
	monsterEntry.get().energy -= attack.cost;

	findChild<ui::Bar>("energy")->setValue(monsterEntry.get().energy);
	findChild<ui::Label>("energy")->setLabel(Fontrenderer::Get().floatToString(monsterEntry.get().energy, 0) + "/" + Fontrenderer::Get().floatToString(m_maxEnergy, 0));
}

void Monster::playAttackAnimation() {
	m_attackOffset = 4u;
	m_elapsedTime = 0.0f;
}

void Monster::canAttack() {
	CanAttack(m_canAttack, MonsterIndex::_AttackData, *this);
}

const bool Monster::getCanAttack() const {
	return m_canAttack;
}

void Monster::setCanAttack(bool canAttack) {
	m_canAttack = canAttack;
}

void Monster::applyAttack(float amount, const AttackData& attackData) {
	
	if (attackData.element == "fire" && MonsterIndex::MonsterData[monsterEntry.get().name].element == "plant" ||
		attackData.element == "water" && MonsterIndex::MonsterData[monsterEntry.get().name].element == "fire" ||
		attackData.element == "plant" && MonsterIndex::MonsterData[monsterEntry.get().name].element == "water") {
		amount *= 2.0f;
	}

	if (attackData.element == "fire" && MonsterIndex::MonsterData[monsterEntry.get().name].element == "water" ||
		attackData.element == "water" && MonsterIndex::MonsterData[monsterEntry.get().name].element == "plant" ||
		attackData.element == "plant" && MonsterIndex::MonsterData[monsterEntry.get().name].element == "fire") {
		amount *= 0.5f;
	}
	float targetDefense = 1.0f - MonsterIndex::MonsterData[monsterEntry.get().name].defense * static_cast<float>(monsterEntry.get().level) * 0.0005f;
	if (m_defending) {
		targetDefense -= 0.2f;
	}

	targetDefense = Math::Clamp(targetDefense, 0.0f, 1.0f);
	monsterEntry.get().health -= amount * targetDefense;
	monsterEntry.get().health = std::min(std::max(0.0f, monsterEntry.get().health), m_maxHealth);

	findChild<ui::Bar>("health")->setValue(monsterEntry.get().health);
	findChild<ui::Bar>("health")->setMaxValue(m_maxHealth);
	findChild<ui::Label>("health")->setLabel(Fontrenderer::Get().floatToString(monsterEntry.get().health, 0) + "/" + Fontrenderer::Get().floatToString(m_maxHealth, 0));
}

const float Monster::getBaseDamage(const std::string attackName) const {
	return GetBaseDamage(monsterEntry.get().name, attackName, monsterEntry.get().level);
}

float Monster::GetBaseDamage(const std::string monsterName, const std::string attackName, unsigned int level) {
	return MonsterIndex::MonsterData[monsterName].attack * MonsterIndex::_AttackData[attackName].amount * static_cast<float>(level);
}

void Monster::CanAttack(bool& canAttack, const std::unordered_map<std::string, AttackData>& attackData, const Monster& monster) {
	canAttack = !std::count_if(attackData.begin(), attackData.end(), [&monster = monster](const std::pair<std::string, AttackData>& attack) { return attack.second.cost <= monster.getEnergy(); }) == 0;
}

const bool Monster::getDelayedKill() const {
	return m_delayedKill;
}

void Monster::startDelayedKill() {	
	if (!m_killed) {
		m_delayedKillTimer.start(600u, false, false);
		m_killed = true;
	}
}

const bool Monster::getPause() const {
	return m_pause;
}

void Monster::OnCallBack(CallBack callback) {
	if (callback == CALL_BACK_1) 
		m_coverWithMask = false;
	else if (callback == CALL_BACK_2)
		m_delayedKill = true;
	else if (callback == CALL_BACK_3)
		findChild<ui::Icon>("missing")->eraseSelf();
} 

void Monster::updateExperience(float amount) {
	if (m_maxExperience - monsterEntry.get().experience > amount) {
		monsterEntry.get().experience += amount;
	}else {
		monsterEntry.get().level++;
		monsterEntry.get().experience = amount - (m_maxExperience - monsterEntry.get().experience);
		m_maxExperience = 150.0f * static_cast<float>(monsterEntry.get().level);
		while(monsterEntry.get().experience > m_maxExperience) {
			monsterEntry.get().experience -= m_maxExperience;
			monsterEntry.get().level++;
			m_maxExperience = 150.0f * static_cast<float>(monsterEntry.get().level);
		}
	}

	findChild<ui::Bar>("experience")->setValue(monsterEntry.get().experience);
	findChild<ui::Bar>("experience")->setMaxValue(m_maxExperience);
	findChild<ui::Label>("level")->setLabel("Lvl " + std::to_string(monsterEntry.get().level));
}

const bool Monster::getKilled() const {
	return m_killed;
}

void Monster::setKilled(bool killed) {
	m_killed = killed;
}

const float Monster::getMaxHealth() const {
	return m_maxHealth;
}

void Monster::showMissing() {
	m_showMissingTimer.start(1000u, false);

	ui::Icon* icon = findChild<ui::IconAnimated>("image")->addChild<ui::Icon>(TileSetManager::Get().getTileSet("monster").getTextureRects()[257]);
	icon->setSpriteSheet(TileSetManager::Get().getTileSet("monster").getAtlas());
	icon->setPosition(0.5f, 0.5f);
	icon->setAligned(true);
	icon->setName("missing");
}

const float Monster::getExperience() const {
	return monsterEntry.get().experience;
}

void Monster::setGraphic(int graphic) {
	m_startFrame = graphic;
}

MonsterEntry& Monster::getMonsterEntry() {
	return monsterEntry;
}

void Monster::calculateStates(MonsterEntry& monsterEntry) {
	m_maxExperience = 150.0f * static_cast<float>(monsterEntry.level);
	m_maxHealth = static_cast<float>(monsterEntry.level) *  static_cast<float>(MonsterIndex::MonsterData[monsterEntry.name].maxHealth);
	m_maxEnergy = static_cast<float>(monsterEntry.level) *  static_cast<float>(MonsterIndex::MonsterData[monsterEntry.name].maxEnergy);
	
	findChild<ui::Bar>("health")->setValue(monsterEntry.health);
	findChild<ui::Bar>("health")->setMaxValue(m_maxHealth);

	findChild<ui::Bar>("energy")->setValue(monsterEntry.energy);
	findChild<ui::Bar>("energy")->setMaxValue(m_maxEnergy);

	findChild<ui::Label>("health")->setLabel(Fontrenderer::Get().floatToString(monsterEntry.health, 0) + "/" + Fontrenderer::Get().floatToString(m_maxHealth, 0));
	findChild<ui::Label>("energy")->setLabel(Fontrenderer::Get().floatToString(monsterEntry.energy, 0) + "/" + Fontrenderer::Get().floatToString(m_maxEnergy, 0));

	findChild<ui::Bar>("experience")->setValue(monsterEntry.experience);
	findChild<ui::Bar>("experience")->setMaxValue(m_maxExperience);

	findChild<ui::Label>("name")->setLabel(monsterEntry.name);
	findChild<ui::Label>("level")->setLabel("Lvl " + std::to_string(monsterEntry.level));

	float padding = 10.0f;
	float width = Globals::fontManager.get("dialog").getWidth(monsterEntry.name) * 0.045f + 2.0f * padding;
	float height = Globals::fontManager.get("dialog").lineHeight * 0.045f + 2.0f * padding;

	findChild<ui::Surface>("background")->setScaleAbsolute(width, height);
}

void Monster::setMonsterEntry(MonsterEntry& _monsterEntry) {
	monsterEntry = _monsterEntry;
	calculateStates(monsterEntry);	
}

void Monster::setDelayedKill(bool delayedKill) {
	m_delayedKill = delayedKill;
}

void Monster::setDefending(bool defending) {
	m_defending = defending;
}

void Monster::initUI() {

	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[cell.currentFrame];
	const TextureRect& highlightRect = TileSetManager::Get().getTileSet("monster").getTextureRects()[cell.currentFrame + m_attackOffset + 8u];
	const TextureRect& missingRect = TileSetManager::Get().getTileSet("monster").getTextureRects()[257];
	const Vector2f& pos = getWorldPosition();
	float lineHeightSmall = Globals::fontManager.get("dialog").lineHeight * 0.035f;
	float padding = 10.0f;
	float width = Globals::fontManager.get("dialog").getWidth(monsterEntry.get().name) * 0.045f + 2.0f * padding;
	float height = Globals::fontManager.get("dialog").lineHeight * 0.045f + 2.0f * padding;
	float lvlWidth = 60.0f;
	float lvlHeight = 26.0f;
	float fontWidth = Globals::fontManager.get("dialog").getWidth("Lvl: " + std::to_string(monsterEntry.get().level)) * 0.035f - 5.0f;
	float fontHeight = Globals::fontManager.get("dialog").lineHeight * 0.035f - 5.0f;

	ui::Surface* surface = addChild<ui::Surface>();
	surface->translateRelative(-width * 0.5f + 16.0f, 96.0f - height * 0.5f + 40.0f);
	surface->scaleAbsolute(width, height);
	surface->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setBorderRadius(0.0f);
	surface->setEdge(ui::Edge::EDGE_NONE);
	surface->setName("background");

	ui::Label* label = surface->addChild<ui::Label>(Globals::fontManager.get("dialog"));
	label->translateRelative(padding, padding);
	label->setTextColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	label->setSize(0.045f);
	label->setLabel(monsterEntry.get().name);
	label->setName("name");

	surface = addChild<ui::Surface>();
	surface->translateRelative(-width * 0.5f + 16.0f, + 96.0f - height * 0.5f + 40.0f - lvlHeight);
	surface->scaleAbsolute(lvlWidth, lvlHeight);
	surface->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setBorderRadius(0.0f);
	surface->setEdge(ui::Edge::EDGE_NONE);

	label = surface->addChild<ui::Label>(Globals::fontManager.get("dialog"));
	label->translateRelative(30 - 0.5f * fontWidth, 13.0f - 0.5f * fontHeight);
	label->setTextColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	label->setSize(0.035f);
	label->setLabel("Lvl " + std::to_string(monsterEntry.get().level));
	label->setName("level");

	ui::Bar* bar = addChild<ui::Bar>(TileSetManager::Get().getTileSet("bars"));
	bar->translateRelative(-width * 0.5f + 16.0f, + 96.0f - height * 0.5f + 40.0f - lvlHeight);
	bar->setRadius(0.0f);
	bar->setBgColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	bar->setColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	bar->setWidth(lvlWidth);
	bar->setHeight(2.0f);
	bar->setMaxValue(m_maxExperience);
	bar->setValue(monsterEntry.get().experience);
	bar->setName("experience");

	ui::IconAnimated* iconAnimated = addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("monster").getTextureRects());
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->scaleAbsolute(highlightRect.width, highlightRect.height);
	iconAnimated->setName("highlight");
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("monster").getAtlas());
	iconAnimated->setCurrentFrame(-1);
	iconAnimated->setFlipped(cell.flipped);

	iconAnimated = addChild<ui::IconAnimated>(TileSetManager::Get().getTileSet("monster").getTextureRects());
	iconAnimated->setPosition(0.0f, 0.0f);
	iconAnimated->scaleAbsolute(rect.width, rect.height);
	iconAnimated->setName("image");
	iconAnimated->setSpriteSheet(TileSetManager::Get().getTileSet("monster").getAtlas());
	iconAnimated->setCurrentFrame(cell.currentFrame);
	iconAnimated->setFlipped(cell.flipped);

	surface = addChild<ui::Surface>();
	surface->translateRelative(0.5f * rect.width - 75.0f, -20.0);
	surface->scaleAbsolute(150.0f, 48.0f);
	surface->setColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	surface->setShader(Globals::shaderManager.getAssetPointer("list"));
	surface->setBorderRadius(0.0f);
	surface->setEdge(ui::Edge::EDGE_NONE);

	bar = addChild<ui::Bar>(TileSetManager::Get().getTileSet("bars"));
	bar->translateRelative(0.5f * rect.width - 75.0f, -20.0);
	bar->setName("initiative");
	bar->setRadius(0.0f);
	bar->setBgColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	bar->setColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	bar->setWidth(150.0f);
	bar->setHeight(2.0f);
	bar->setMaxValue(100.0f);
	
	bar = addChild<ui::Bar>(TileSetManager::Get().getTileSet("bars"));
	bar->translateRelative(0.5f * rect.width - 75.0f + 9.0f, -20.0f + 48.0f - 2.0f * lineHeightSmall + 11.0f);
	bar->setName("health");
	bar->setRadius(0.0f);
	bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	bar->setColor(Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f));
	bar->setWidth(130.0f);
	bar->setHeight(5.0f);
	bar->setValue(monsterEntry.get().health);
	bar->setMaxValue(m_maxHealth);

	bar = addChild<ui::Bar>(TileSetManager::Get().getTileSet("bars"));
	bar->translateRelative(0.5f * rect.width - 75.0f + 9.0f, -20.0f + 48.0f - 2.0f * lineHeightSmall - 10.0f);
	bar->setName("energy");
	bar->setRadius(0.0f);
	bar->setBgColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	bar->setColor(Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f));
	bar->setWidth(130.0f);
	bar->setHeight(5.0f);
	bar->setValue(monsterEntry.get().energy);
	bar->setMaxValue(m_maxEnergy);

	label  = addChild<ui::Label>(Globals::fontManager.get("dialog"));
	label->translateRelative(0.5f * rect.width - 75.0f + 10.0f, -20.0f + 48.0f - lineHeightSmall);
	label->setTextColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	label->setSize(0.035f);
	label->setLabel(Fontrenderer::Get().floatToString(monsterEntry.get().health, 0) + "/" + Fontrenderer::Get().floatToString(m_maxHealth, 0));
	label->setName("health");

	label = addChild<ui::Label>(Globals::fontManager.get("dialog"));
	label->translateRelative(0.5f * rect.width - 75.0f + 10.0f, -20.0f + 48.0f - 2.0f * lineHeightSmall - 4.0f);
	label->setTextColor(Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	label->setSize(0.035f);
	label->setLabel(Fontrenderer::Get().floatToString(monsterEntry.get().energy, 0) + "/" + Fontrenderer::Get().floatToString(m_maxEnergy, 0));
	label->setName("energy");
}