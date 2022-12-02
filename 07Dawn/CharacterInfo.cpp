#include "CharacterInfo.h"
#include "Statssystem.h"

CharacterInfo  CharacterInfo::s_instance;

CharacterInfo & CharacterInfo::Get() {
	return s_instance;
}

CharacterInfo::CharacterInfo() : Widget(25, 100, 434, 492, 13, 15){

}

void CharacterInfo::setPlayer(Player* player) {
	m_player = player;
}

void CharacterInfo::init() {
	m_infoFont = &Globals::fontManager.get("verdana_12");

	TextureAtlasCreator::get().init("infoscreen", 1024, 1024);

	TextureManager::Loadimage("res/interface/CharacterInfoScreen/background.tga", 0, m_textures);
	TextureManager::Loadimage("res/white2x2pixel.png", 1, m_textures, true);
	TextureManager::Loadimage("res/interface/CharacterInfoScreen/meleeSelected.tga", 2, m_textures);
	TextureManager::Loadimage("res/interface/CharacterInfoScreen/defenseSelected.tga", 3, m_textures);
	TextureManager::Loadimage("res/interface/CharacterInfoScreen/spellsSelected.tga", 4, m_textures);

	m_textureAtlas = TextureAtlasCreator::get().getAtlas();

	currentTab = 0;
	tabs[0].tabimage = m_textures[2];
	tabs[0].height = 32;
	tabs[0].width = 58;
	tabs[0].posX = 61;
	tabs[0].posY = 242;

	tabs[1].tabimage = m_textures[3];
	tabs[1].height = 32;
	tabs[1].width = 72;
	tabs[1].posX = 196;
	tabs[1].posY = 242;

	tabs[2].tabimage = m_textures[4];
	tabs[2].height = 32;
	tabs[2].width = 59;
	tabs[2].posX = 340;
	tabs[2].posY = 242;

	addMoveableFrame(434, 21, 13, 486);
	addCloseButton(22, 22, 422, 483);
}

void CharacterInfo::draw() {
	if (!m_visible) return;
	TextureManager::BindTexture(m_textureAtlas, true, 0);
	TextureManager::DrawTextureBatched(m_textures[0], m_posX, m_posY, false, false);

	unsigned short descriptionTextStart = 60;
	unsigned short valueFieldStart = 280;
	unsigned short topBorderDistance = 30;
	unsigned short lineDistance = 15;


	unsigned short curLine = 0;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), m_player->getName(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Exp", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	
	curLine++;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Level " + std::to_string(m_player->getLevel()) + " " + m_player->getClassName().c_str(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	
	curLine += 2;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Health", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getCurrentHealth()) + " / " + std::to_string(m_player->getModifiedMaxHealth()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

	curLine++;
	if (m_player->getArchType() == Enums::CharacterArchType::Caster) {
		Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Mana", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getCurrentMana()) + " / " + std::to_string(m_player->getModifiedMaxMana()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	
	}else if (m_player->getArchType() == Enums::CharacterArchType::Fighter) {
		Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Max fatigue", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedMaxFatigue()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	}
	curLine += 2;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Dexterity", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedDexterity()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

	curLine++;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Intellect", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedIntellect()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

	curLine++;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Strength", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedStrength()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

	curLine++;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Vitality", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedVitality()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

	curLine++;
	Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Wisdom", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedWisdom()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	
	drawExpBar();
	drawTabs();
	
	TextureManager::DrawBuffer(false);
}

void CharacterInfo::drawExpBar() {
	unsigned long neededXP = (m_player->getExpNeededForLevel(m_player->getLevel() + 1)) - m_player->getExpNeededForLevel(m_player->getLevel());
	unsigned long currentXP = m_player->getExperience() - m_player->getExpNeededForLevel(m_player->getLevel());

	float expBarWidth = static_cast<float>(currentXP) / neededXP * 100.0f;

	TextureManager::DrawTextureBatched(m_textures[1], m_posX + 280, m_posY + 433, 100.0f, 10.0f, Vector4f(0.7f, 0.73f, 0.29f, 1.0f), false, false);
	TextureManager::DrawTextureBatched(m_textures[1], m_posX + 280, m_posY + 433, expBarWidth, 10.0f, Vector4f(0.9f, 0.93f, 0.29f, 1.0f), false, false);
}

void CharacterInfo::drawTabs() {
	TextureManager::DrawTextureBatched(tabs[currentTab].tabimage, m_posX + tabs[currentTab].posX, m_posY + tabs[currentTab].posY, false, false);

	const StatsSystem *statsSystem = StatsSystem::getStatsSystem();

	size_t descriptionTextStart = 80;
	size_t valueFieldStart = 300;
	size_t topBorderDistance = 265;
	size_t lineDistance = 15;

	Fontrenderer::Get().bindTexture(*m_infoFont);
	int curLine = 0;
	switch (currentTab) {
		case 0: // draws melee tab
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Damage", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(static_cast<unsigned int>(m_player->getModifiedMinDamage() * statsSystem->complexGetDamageModifier(m_player->getLevel(), m_player->getModifiedDamageModifierPoints(), m_player->getLevel()))) + "-" + std::to_string(static_cast<unsigned int>(m_player->getModifiedMaxDamage() * statsSystem->complexGetDamageModifier(m_player->getLevel(), m_player->getModifiedDamageModifierPoints(), m_player->getLevel()))), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		
			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Chance to hit", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetHitChance(m_player->getLevel(), m_player->getModifiedHitModifierPoints(), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Critical strike", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetMeleeCriticalStrikeChance(m_player->getLevel(), m_player->getModifiedMeleeCriticalModifierPoints(), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

		break;
		case 1:
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Armor", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedArmor()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		
			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Damage Reduction", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetDamageReductionModifier(m_player->getLevel(), m_player->getModifiedArmor(), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Chance to block", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetBlockChance(m_player->getLevel(), m_player->getModifiedBlockModifierPoints(), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Dodge", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetEvadeChance(m_player->getLevel(), m_player->getModifiedEvadeModifierPoints(), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Parry", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetParryChance(m_player->getLevel(), m_player->getModifiedParryModifierPoints(), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine++;
			for (size_t curElement = 0; curElement< static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
				Enums::ElementType curElementType = static_cast<Enums::ElementType>(curElement);
				Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::string("Resist ").append(Character::ElementToString(curElementType)), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
				Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetResistElementChance(m_player->getLevel(), m_player->getModifiedResistElementModifierPoints(curElementType), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

				curLine++;
			}

		break;
		case 2:
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Spell critical strike", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetSpellCriticalStrikeChance(m_player->getLevel(), m_player->getModifiedSpellCriticalModifierPoints(), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Health regen", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedHealthRegen()) + " per sec", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine++;
			Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), "Mana regen", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::to_string(m_player->getModifiedManaRegen()) + " per sec", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

			curLine += 2;
			for (size_t curElement = 0; curElement< static_cast<size_t>(Enums::ElementType::CountET); ++curElement) {
				Enums::ElementType curElementType = static_cast<Enums::ElementType>(curElement);
				Fontrenderer::Get().addText(*m_infoFont, m_posX + descriptionTextStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), std::string(Character::ElementToString(curElementType).append(" spell effect")), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
				Fontrenderer::Get().addText(*m_infoFont, m_posX + valueFieldStart, m_posY + m_height - (topBorderDistance + curLine*lineDistance + m_infoFont->lineHeight), Fontrenderer::Get().FloatToString(statsSystem->complexGetSpellEffectElementModifier(m_player->getLevel(), m_player->getModifiedSpellEffectElementModifierPoints(curElementType), m_player->getLevel()) * 100.0f, 2) + "%", Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
				curLine++;
			}

		break;
	}
}

void CharacterInfo::processInput() {
	if (!m_visible) return;
	Widget::processInput();

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		for (size_t tabIndex = 0; tabIndex <= 2; tabIndex++) {
			if (ViewPort::get().getCursorPosRelX() > tabs[tabIndex].posX + m_posX
				&& ViewPort::get().getCursorPosRelY() > tabs[tabIndex].posY + m_posY
				&& ViewPort::get().getCursorPosRelX() < tabs[tabIndex].posX + m_posX + tabs[tabIndex].width
				&& ViewPort::get().getCursorPosRelY() < tabs[tabIndex].posY + m_posY + tabs[tabIndex].height) {
				currentTab = tabIndex;
			}
		}
	}
}