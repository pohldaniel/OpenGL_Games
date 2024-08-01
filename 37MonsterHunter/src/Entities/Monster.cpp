#include <engine/Batchrenderer.h>
#include <engine/Fontrenderer.h>
#include "Monster.h"
#include "Globals.h"
#include "MonsterIndex.h"

std::random_device Monster::RandomDevice;
std::uniform_real_distribution<float> Monster::Distribution(-1.0f, 1.0f);

Monster::Monster(Cell& cell, std::string name, unsigned int level, float experience, float health, float energy) : SpriteEntity(cell),
m_animationSpeed(6.0f + Distribution(RandomDevice)),
m_name(name),
m_level(level),
m_experience(experience),
m_maxExperience(150.0f * static_cast<float>(level)),
m_health(health),
m_maxHealth(static_cast<float>(level * MonsterIndex::MonsterData[name].maxHealth)),
m_energy(energy),
m_maxEnergy(static_cast<float>(level * MonsterIndex::MonsterData[name].maxEnergy)),
m_initiative(0.0f),
m_speed(static_cast<float>(level) * MonsterIndex::MonsterData[name].speed),
m_pause(false),
m_highlight(false),
m_coverWithMask(false),
m_attackOffset(0u),
m_canAttack(true),
m_isDefending(false),
m_delayedKill(false),
m_highlightTimer(this, CALL_BACK_1),
m_delayedKillTimer(this, CALL_BACK_2),
m_showMissingTimer(this, CALL_BACK_3),
m_killed(false),
m_showMissing(false)
{
	m_direction.set(0.0f, 0.0f);
	canAttack();
}

Monster::Monster(Monster const& rhs) : SpriteEntity(rhs.cell) {
	m_bars = rhs.m_bars;
	m_animationSpeed = rhs.m_animationSpeed;
	m_name = rhs.m_name;
	m_level = rhs.m_level;
	m_experience = rhs.m_experience;
	m_maxExperience = rhs.m_maxExperience;
	m_health = rhs.m_health;
	m_maxHealth = rhs.m_maxHealth;
	m_energy = rhs.m_energy;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_isDefending = rhs.m_isDefending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;
	m_showMissing = rhs.m_showMissing;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setRecipient(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setRecipient(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setRecipient(this);
}

Monster::Monster(Monster&& rhs) : SpriteEntity(rhs.cell) {
	m_bars = rhs.m_bars;
	m_animationSpeed = rhs.m_animationSpeed;
	m_name = rhs.m_name;
	m_level = rhs.m_level;
	m_experience = rhs.m_experience;
	m_maxExperience = rhs.m_maxExperience;
	m_health = rhs.m_health;
	m_maxHealth = rhs.m_maxHealth;
	m_energy = rhs.m_energy;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_isDefending = rhs.m_isDefending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;
	m_showMissing = rhs.m_showMissing;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setRecipient(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setRecipient(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setRecipient(this);
}

Monster& Monster::operator=(const Monster& rhs) {
	SpriteEntity::operator=(rhs);

	m_bars = rhs.m_bars;
	m_animationSpeed = rhs.m_animationSpeed;
	m_name = rhs.m_name;
	m_level = rhs.m_level;
	m_experience = rhs.m_experience;
	m_maxExperience = rhs.m_maxExperience;
	m_health = rhs.m_health;
	m_maxHealth = rhs.m_maxHealth;
	m_energy = rhs.m_energy;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_isDefending = rhs.m_isDefending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;
	m_showMissing = rhs.m_showMissing;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setRecipient(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setRecipient(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setRecipient(this);
	return *this;
}

Monster& Monster::operator=(Monster&& rhs) {
	SpriteEntity::operator=(rhs);

	m_bars = rhs.m_bars;
	m_animationSpeed = rhs.m_animationSpeed;
	m_name = rhs.m_name;
	m_level = rhs.m_level;
	m_experience = rhs.m_experience;
	m_maxExperience = rhs.m_maxExperience;
	m_health = rhs.m_health;
	m_maxHealth = rhs.m_maxHealth;
	m_energy = rhs.m_energy;
	m_maxEnergy = rhs.m_maxEnergy;
	m_initiative = rhs.m_initiative;
	m_speed = rhs.m_speed;
	m_pause = rhs.m_pause;
	m_highlight = rhs.m_highlight;
	m_coverWithMask = rhs.m_coverWithMask;
	m_canAttack = rhs.m_canAttack;
	m_isDefending = rhs.m_isDefending;
	m_delayedKill = rhs.m_delayedKill;
	m_killed = rhs.m_killed;
	m_attackOffset = rhs.m_attackOffset;
	m_showMissing = rhs.m_showMissing;

	m_highlightTimer = rhs.m_highlightTimer;
	m_highlightTimer.setRecipient(this);
	m_delayedKillTimer = rhs.m_delayedKillTimer;
	m_delayedKillTimer.setRecipient(this);
	m_showMissingTimer = rhs.m_showMissingTimer;
	m_showMissingTimer.setRecipient(this);
	return *this;
}

Monster::~Monster() {

}

void Monster::drawBack() {

	float padding = 10.0f;
	float width = Globals::fontManager.get("dialog").getWidth(m_name) * 0.045f + 2.0f * padding;
	float height = Globals::fontManager.get("dialog").lineHeight * 0.045f + 2.0f * padding;

	float lvlWidth = 60.0f;
	float lvlHeight = 26.0f;
	float fontWidth = Globals::fontManager.get("dialog").getWidth("Lvl: " + std::to_string(m_level)) * 0.035f - 5.0f;
	float fontHeight = Globals::fontManager.get("dialog").lineHeight * 0.035f - 5.0f;
	float lineHeightSmall = Globals::fontManager.get("dialog").lineHeight * 0.035f;

	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[cell.currentFrame];
	const TextureRect& emptyRect = TileSetManager::Get().getTileSet("monster").getTextureRects()[256];

	if(cell.flipped) {
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - width * 0.5f + 16.0f, cell.posY + 96.0f - height * 0.5f + 40.0f, width, height), Vector4f(emptyRect.textureOffsetX, emptyRect.textureOffsetY, emptyRect.textureWidth, emptyRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), emptyRect.frame);
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX - width * 0.5f + 16.0f, cell.posY + 96.0f - height * 0.5f + 40.0f - lvlHeight, lvlWidth, lvlHeight), Vector4f(emptyRect.textureOffsetX, emptyRect.textureOffsetY, emptyRect.textureWidth, emptyRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), emptyRect.frame);
		drawBar({ cell.posX - width * 0.5f + 16.0f, cell.posY + 96.0f - height * 0.5f + 40.0f - lvlHeight, lvlWidth, 2.0f }, emptyRect, m_experience, m_maxExperience, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX - width * 0.5f + 16.0f + padding, cell.posY + 96.0f - height * 0.5f + 40.0f + padding, m_name, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.045f);
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX - width * 0.5f + 16.0f + 30 - 0.5f * fontWidth, cell.posY + 96.0f - height * 0.5f + 40.0f - lvlHeight + 13.0f - 0.5f * fontHeight, "Lvl " + std::to_string(m_level), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.035f);
	}else {		 
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + rect.width - width * 0.5f - 30.0f, cell.posY + 96.0f - height * 0.5f + 40.0f, width, height), Vector4f(emptyRect.textureOffsetX, emptyRect.textureOffsetY, emptyRect.textureWidth, emptyRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), emptyRect.frame);
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + rect.width + width - width * 0.5f - 30.0f - lvlWidth, cell.posY + 96.0f - height * 0.5f + 40.0f - lvlHeight, lvlWidth, lvlHeight), Vector4f(emptyRect.textureOffsetX, emptyRect.textureOffsetY, emptyRect.textureWidth, emptyRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), emptyRect.frame);
		drawBar({ cell.posX + rect.width + width - width * 0.5f - 30.0f - lvlWidth, cell.posY + 96.0f - height * 0.5f + 40.0f - lvlHeight, lvlWidth, 2.0f }, emptyRect, m_experience, m_maxExperience, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f));

		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX + rect.width - width * 0.5f - 30.0f + padding, cell.posY + 96.0f - height * 0.5f + 40.0f + padding, m_name, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.045f);
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX + rect.width + width - width * 0.5f - 30.0f - lvlWidth + 30 - 0.5f * fontWidth, cell.posY + 96.0f - height * 0.5f + 40.0f - lvlHeight + 13.0f - 0.5f * fontHeight, "Lvl " + std::to_string(m_level), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.035f);
	}
}

void Monster::draw() {

	float padding = 10.0f;
	float width = Globals::fontManager.get("dialog").getWidth(m_name) * 0.045f + 2.0f * padding;
	float height = Globals::fontManager.get("dialog").lineHeight * 0.045f + 2.0f * padding;
	
	//float lvlWidth = Globals::fontManager.get("dialog").getWidth("Lvl: " + std::to_string(m_level)) * 0.035f + 2.0f * padding;
	//float lvlHeight = Globals::fontManager.get("dialog").lineHeight * 0.035f + 2.0f * padding;

	float lvlWidth = 60.0f;
	float lineHeightSmall = Globals::fontManager.get("dialog").lineHeight * 0.035f;

	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[cell.currentFrame + m_attackOffset];
	const TextureRect& highlightRect = TileSetManager::Get().getTileSet("monster").getTextureRects()[cell.currentFrame + m_attackOffset +  8u];
	const TextureRect& emptyRect = TileSetManager::Get().getTileSet("monster").getTextureRects()[256];
	const TextureRect& missingRect = TileSetManager::Get().getTileSet("monster").getTextureRects()[257];
	const TextureRect& barRect = TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[19];

	if (cell.flipped) {
		//Monster sprite
		if (m_highlight) {
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, highlightRect.width, highlightRect.height), Vector4f(highlightRect.textureOffsetX + highlightRect.textureWidth, highlightRect.textureOffsetY, -highlightRect.textureWidth, highlightRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), highlightRect.frame);
		}

		if(!m_coverWithMask)
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, rect.width, rect.height), Vector4f(rect.textureOffsetX + rect.textureWidth, rect.textureOffsetY, -rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
	
		//attributes
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + 0.5f * rect.width - 75.0f, cell.posY - 20.0f, 150.0f, 48.0f), Vector4f(emptyRect.textureOffsetX, emptyRect.textureOffsetY, emptyRect.textureWidth, emptyRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), emptyRect.frame);
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - lineHeightSmall, Fontrenderer::Get().floatToString(m_health, 0) + "/" + Fontrenderer::Get().floatToString(m_maxHealth, 0), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.035f);
		addBar({ cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - lineHeightSmall - 4.0f, lvlWidth, 5.0f }, barRect, m_health, m_maxHealth, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f));
		
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - 2.0f * lineHeightSmall - 4.0f, Fontrenderer::Get().floatToString(m_energy, 0) + "/" + Fontrenderer::Get().floatToString(m_maxEnergy, 0), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.035f);
		addBar({ cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - 2.0f * lineHeightSmall - 8.0f, lvlWidth, 5.0f }, barRect, m_energy, m_maxEnergy, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f));

		drawBar({ cell.posX + 0.5f * rect.width - 75.0f, cell.posY - 20.0f, 150.0f, 2.0f }, emptyRect, m_initiative, 100.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	}else {
		
		//Monster sprite
		if (m_highlight) {
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, highlightRect.width, highlightRect.height), Vector4f(highlightRect.textureOffsetX, highlightRect.textureOffsetY, highlightRect.textureWidth, highlightRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), highlightRect.frame);
		}

		if (!m_coverWithMask)
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);

		if (m_showMissing)
			Batchrenderer::Get().addQuadAA(Vector4f(cell.centerX - 0.5f * missingRect.width, cell.centerY - 0.5f * missingRect.height, missingRect.width, missingRect.height), Vector4f(missingRect.textureOffsetX, missingRect.textureOffsetY, missingRect.textureWidth, missingRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), missingRect.frame);

		//attributes
		Batchrenderer::Get().addQuadAA(Vector4f(cell.posX + 0.5f * rect.width - 75.0f, cell.posY - 20.0f, 150.0f, 48.0f), Vector4f(emptyRect.textureOffsetX, emptyRect.textureOffsetY, emptyRect.textureWidth, emptyRect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), emptyRect.frame);
		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - lineHeightSmall, Fontrenderer::Get().floatToString(m_health, 0) + "/" + Fontrenderer::Get().floatToString(m_maxHealth, 0), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.035f);
		addBar({ cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - lineHeightSmall - 4.0f, lvlWidth, 5.0f }, barRect, m_health, m_maxHealth, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.94117f, 0.19215f, 0.19215f, 1.0f));

		Fontrenderer::Get().addText(Globals::fontManager.get("dialog"), cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - 2.0f * lineHeightSmall - 4.0f, Fontrenderer::Get().floatToString(m_energy, 0) + "/" + Fontrenderer::Get().floatToString(m_maxEnergy, 0), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), 0.035f);
		addBar({ cell.posX + 0.5f * rect.width - 75.0f + 10.0f, cell.posY - 20.0f + 48.0f - 2.0f * lineHeightSmall - 8.0f, lvlWidth, 5.0f }, barRect, m_energy, m_maxEnergy, Vector4f(0.0f, 0.0f, 0.0f, 1.0f), Vector4f(0.4f, 0.84313f, 0.93333f, 1.0f));

		drawBar({ cell.posX + 0.5f * rect.width - 75.0f, cell.posY - 20.0f, 150.0f, 2.0f }, emptyRect, m_initiative, 100.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), Vector4f(0.0f, 0.0f, 0.0f, 1.0f));
	}	
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

	if (m_pause)
		return;

	m_initiative += m_speed * dt;
}

void Monster::addBar(const Rect& rect, const TextureRect& textureRect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color) {
	m_bars.push_back({ rect , textureRect , value , maxValue , bgColor , color });
}

void Monster::drawBars() {

	float ratio;
	float progress;

	for (const auto& bar : m_bars) {
		Batchrenderer::Get().addQuadAA(Vector4f(bar.rect.posX, bar.rect.posY, bar.rect.width, bar.rect.height), Vector4f(bar.textureRect.textureOffsetX, bar.textureRect.textureOffsetY, bar.textureRect.textureWidth, bar.textureRect.textureHeight), bar.bgColor, bar.textureRect.frame);


		ratio = bar.rect.width / bar.maxValue;
		progress = std::max(0.0f, std::min(bar.rect.width, bar.value * ratio));
		if (progress < 1.0f)
			continue;

		const TextureRect& barRect = progress < 3.0f ? TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[22] :
								     progress < 6.0f ? TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[21] :  
									 progress < 10.0f ? TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[20] :
									 TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[19];

		Batchrenderer::Get().addQuadAA(Vector4f(bar.rect.posX, bar.rect.posY, progress, bar.rect.height), Vector4f(barRect.textureOffsetX, barRect.textureOffsetY, barRect.textureWidth, barRect.textureHeight), bar.color, barRect.frame);
	}

	m_bars.clear();
	m_bars.shrink_to_fit();
}

void Monster::drawBar(const Rect& rect, const TextureRect& textureRect, float value, float maxValue, const Vector4f& bgColor, const Vector4f& color) {	
	float ratio = rect.width / maxValue;
	float progress = std::max(0.0f, std::min(rect.width, value * ratio));
	Batchrenderer::Get().addQuadAA(Vector4f(rect.posX, rect.posY, rect.width, rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), bgColor, textureRect.frame);
	Batchrenderer::Get().addQuadAA(Vector4f(rect.posX, rect.posY, progress, rect.height), Vector4f(textureRect.textureOffsetX, textureRect.textureOffsetY, textureRect.textureWidth, textureRect.textureHeight), color, textureRect.frame);
}

float Monster::getInitiative() {
	return m_initiative;
}

void Monster::setInitiative(float initiative) {
	m_initiative = initiative;
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
	return m_name;
}

const unsigned int Monster::getLevel() const {
	return m_level;
}

const float Monster::getEnergy() const {
	return m_energy;
}

const float Monster::getHealth() const {
	return m_health;
}

void Monster::reduceEnergy(const AttackData& attack) {
	m_energy -= attack.cost;
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
	
	if (attackData.element == "fire" && MonsterIndex::MonsterData[m_name].element == "plant" ||
		attackData.element == "water" && MonsterIndex::MonsterData[m_name].element == "fire" ||
		attackData.element == "plant" && MonsterIndex::MonsterData[m_name].element == "water") {
		amount *= 2.0f;
	}

	if (attackData.element == "fire" && MonsterIndex::MonsterData[m_name].element == "water" ||
		attackData.element == "water" && MonsterIndex::MonsterData[m_name].element == "plant" ||
		attackData.element == "plant" && MonsterIndex::MonsterData[m_name].element == "fire") {
		amount *= 0.5f;
	}
	float targetDefense = 1.0f - MonsterIndex::MonsterData[m_name].defense * static_cast<float>(m_level) * 0.0005f;
	if (m_isDefending) {
		targetDefense -= 0.2f;
	}

	targetDefense = Math::Clamp(targetDefense, 0.0f, 1.0f);
	m_health -= amount * targetDefense;
	m_health = std::min(std::max(0.0f, m_health), m_maxHealth);
}

const float Monster::getBaseDamage(const std::string attackName) const {
	return GetBaseDamage(m_name, attackName, m_level);
}

void Monster::setIsDefending(bool isDefending) {
	m_isDefending = isDefending;
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
	m_delayedKillTimer.start(600u, false, true);	
	m_killed = true;
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
		m_showMissing = false;
	
} 

void Monster::updateExperience(float amount) {
	if (m_maxExperience - m_experience > amount) {
		m_experience += amount;
	}else {
		m_level++;
		m_experience = amount - (m_maxExperience - m_experience);
		m_maxExperience = 150.0f * static_cast<float>(m_level);
		
		while(m_experience > m_maxExperience) {
			m_experience -= m_maxExperience;
			m_level++;
			m_maxExperience = 150.0f * static_cast<float>(m_level);
		}
	}
}

const bool Monster::getKilled() const {
	return m_killed;
}

const float Monster::getMaxHealth() const {
	return m_maxHealth;
}

void Monster::showMissing() {
	m_showMissing = true;
	m_showMissingTimer.start(1000u, false);
}