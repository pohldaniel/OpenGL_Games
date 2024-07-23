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
m_coverWithMask(false)
{
	m_direction.set(0.0f, 0.0f);
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
	const TextureRect& emptyRect = TileSetManager::Get().getTileSet("monster").getTextureRects().back();

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

	const TextureRect& rect = TileSetManager::Get().getTileSet("monster").getTextureRects()[cell.currentFrame];
	const TextureRect& hRect = TileSetManager::Get().getTileSet("monster").getTextureRects()[cell.currentFrame +  8u];
	const TextureRect& emptyRect = TileSetManager::Get().getTileSet("monster").getTextureRects().back();
	const TextureRect& barRect = TileSetManager::Get().getTileSet("monster_icon").getTextureRects()[19];
	if (cell.flipped) {
		//Monster sprite
		if (m_highlight) {
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, hRect.width, hRect.height), Vector4f(hRect.textureOffsetX + hRect.textureWidth, hRect.textureOffsetY, -hRect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), hRect.frame);
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
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, hRect.width, hRect.height), Vector4f(hRect.textureOffsetX, hRect.textureOffsetY, hRect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), hRect.frame);
		}

		if (!m_coverWithMask)
			Batchrenderer::Get().addQuadAA(Vector4f(cell.posX, cell.posY, rect.width, rect.height), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);

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

	m_elapsedTime += m_animationSpeed * dt;
	cell.currentFrame = m_startFrame + static_cast<int>(std::floor(m_elapsedTime));
	if (cell.currentFrame - m_startFrame > m_frameCount - 1) {
		cell.currentFrame = m_startFrame;
		m_elapsedTime -= static_cast<float>(m_frameCount);
	}

	if (m_pause)
		return;

	m_initiative += m_speed * dt;
	//if (m_initiative >= 100.0f)
	//	m_initiative = 0.0f;
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
	if (m_highlight) {
		m_coverWithMask = true;
		m_highlightTimer.setOnTimerEnd([&m_coverWithMask = m_coverWithMask]() {
			m_coverWithMask = false;
		});
		m_highlightTimer.start(300, false);
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