#include "Interface.h"
#include "Npc.h"
#include "Zone.h"
#include "Player.h"
#include "TextWindow.h"
#include "Shop.h"
#include "Quest.h"
#include "InventoryCanvas.h"
#include "Utils.h"

Interface Interface::s_instance;

Interface::Interface() : m_spellbook(Spellbook::Get()), m_characterInfo(CharacterInfo::Get()) {

}

Interface::~Interface() {
}

Interface& Interface::Get() {
	return s_instance;
}

void Interface::setPlayer(Player* player) {
	m_player = player;	
}

void Interface::init(std::vector<TextureRect> textures) {
	m_textures = textures;

	m_displayOffsetSmall = 24;
	m_displayOffsetBig = 34;

	m_shortcutFont = &Globals::fontManager.get("verdana_10");
	m_cooldownFont = &Globals::fontManager.get("verdana_11");
	m_interfaceFont = &Globals::fontManager.get("verdana_12");

	m_button.push_back({ 22, 14, 46, 46, "1", Keyboard::Key::KEY_1 });
	m_button.push_back({ 82, 14, 46, 46, "2", Keyboard::Key::KEY_2 });
	m_button.push_back({ 142, 14, 46, 46, "3", Keyboard::Key::KEY_3 });
	m_button.push_back({ 202, 14, 46, 46, "4", Keyboard::Key::KEY_4 });
	m_button.push_back({ 262, 14, 46, 46, "5", Keyboard::Key::KEY_5 });
	m_button.push_back({ 322, 14, 46, 46, "6", Keyboard::Key::KEY_6 });
	m_button.push_back({ 382, 14, 46, 46, "7", Keyboard::Key::KEY_7 });
	m_button.push_back({ 442, 14, 46, 46, "8", Keyboard::Key::KEY_8 });
	m_button.push_back({ 502, 14, 46, 46, "9", Keyboard::Key::KEY_9 });
	m_button.push_back({ 562, 14, 46, 46, "0", Keyboard::Key::KEY_0 });

	m_actionBarPosX = ViewPort::Get().getWidth() - 630;
	m_actionBarPosY = 0;
	m_preparingAoESpell = false;

	connectWidget(m_spellbook);
	connectWidget(m_characterInfo);
	connectWidget(m_characterInfo);
	connectWidget(InventoryCanvas::Get());
	connectWidget(QuestCanvas::Get());

	ShopCanvas::Get().setOnClose([&]() {
		m_player->stopShopping();
		//m_player->setTicketForItemTooltip();
		m_player->m_reloadItemTooltip = true;
		for (size_t curFrame = 0; curFrame < m_widgets.size(); curFrame++) {
			if (dynamic_cast<Widget*>(&ShopCanvas::Get()) == m_widgets[curFrame]) {
				ShopCanvas::Get().setVisible(false);
				m_widgets.erase(m_widgets.begin() + curFrame);
				m_widgets.size() == 0 ? m_activeWidget = nullptr : m_activeWidget = m_widgets.back();
				return;
			}
		}
	});

	ShopCanvas::Get().setOnActivate([&]() {
		m_player->startShopping();
		//m_player->setTicketForItemTooltip();
		m_player->m_reloadItemTooltip = true;
		if (ShopCanvas::Get().isVisible()) {
			for (size_t curFrame = 0; curFrame < m_widgets.size(); curFrame++) {
				if (dynamic_cast<Widget*>(&ShopCanvas::Get()) == m_widgets[curFrame]) {
					m_widgets.erase(m_widgets.begin() + curFrame);
					m_widgets.push_back(&ShopCanvas::Get());
					m_activeWidget = m_widgets.back();
					return;
				}
			}
		}
		else {
			// else add it to the frame vector and make it visible.
			m_widgets.push_back(&ShopCanvas::Get());
			ShopCanvas::Get().setVisible(true);
			m_activeWidget = m_widgets.back();
		}
	});
}

void Interface::inscribeActionsToActionsbar() {
	// setting initial actions in the action bar
	const std::vector<SpellActionBase*> inscribedSpells = m_player->getSpellbook();

	for (short curEntry = 0; curEntry <= 9 && curEntry < inscribedSpells.size(); ++curEntry) {
		bindActionToButtonNr(curEntry, inscribedSpells[curEntry]);
	}

}

void Interface::resize(int deltaW, int deltaH) {
	m_actionBarPosX = ViewPort::Get().getWidth() - 630;
	m_spellbook.resize(deltaW, deltaH);
	m_characterInfo.resize(deltaW, deltaH);
	InventoryCanvas::Get().resize(deltaW, deltaH); 
	ShopCanvas::Get().resize(deltaW, deltaH);
}

void Interface::draw() {

	TextureManager::DrawTextureBatched(m_textures[m_player->getPortraitOffset()], 4, ViewPort::Get().getHeight() - 68, false, false);
	TextureManager::DrawTextureBatched(m_textures[11], 0, 50 + ViewPort::Get().getHeight() - m_textures[11].height, false, false);

	float lifeBarPercentage = static_cast<float>(m_player->getCurrentHealth()) / m_player->getModifiedMaxHealth();
	float manaBarPercentage = static_cast<float>(m_player->getCurrentMana()) / m_player->getModifiedMaxMana();
	float fatigueBarPercentage = static_cast<float>(1 - (static_cast<float>(m_player->getCurrentFatigue()) / m_player->getModifiedMaxFatigue()));
	unsigned int neededXP = (m_player->getExpNeededForLevel(m_player->getLevel() + 1)) - m_player->getExpNeededForLevel(m_player->getLevel());
	unsigned int currentXP = m_player->getExperience() - m_player->getExpNeededForLevel(m_player->getLevel());
	float experienceBarPercentage = static_cast<float>(currentXP) / neededXP;

	// health bar
	TextureManager::DrawTextureBatched(m_textures[12], 76, ViewPort::Get().getHeight() - 35, lifeBarPercentage * 123.0f, static_cast<float>(m_textures[12].height), Vector4f(0.815f, 0.16f, 0.16f, 1.0f), false, false);
	// mana bar
	if (m_player->getArchType() == Enums::CharacterArchType::Caster) {
		TextureManager::DrawTextureBatched(m_textures[12], 76, ViewPort::Get().getHeight() - 53, manaBarPercentage * 123.0f, static_cast<float>(m_textures[12].height), Vector4f(0.16f, 0.576f, 0.815f, 1.0f), false, false);
	}

	// fatigue bar
	if (m_player->getArchType() == Enums::CharacterArchType::Fighter) {
		Vector4f color;
		if (fatigueBarPercentage <= 0.33) {
			color = Vector4f(0.109f, 0.917f, 0.047f, 1.0f);

		}else if (fatigueBarPercentage >= 0.34 && fatigueBarPercentage <= 0.66) {
			color = Vector4f(0.917f, 0.847f, 0.047f, 1.0f);

		}else {
			color = Vector4f(0.917f, 0.047f, 0.047f, 1.0f);

		}
		TextureManager::DrawTextureBatched(m_textures[12], 76, ViewPort::Get().getHeight() - 53, fatigueBarPercentage * 123.0f, static_cast<float>(m_textures[12].height), color, false, false);
	}

	// fatigue bar
	TextureManager::DrawTextureBatched(m_textures[13], 76, ViewPort::Get().getHeight() - 67, experienceBarPercentage * 123.0f, static_cast<float>(m_textures[13].height), false, false);

	if (m_player->getIsPreparing()) {
		// actual castbar
		TextureManager::DrawTextureBatched(m_textures[0], ViewPort::Get().getWidth() / 2 - 50, 100, 100.0f, 20.0f, Vector4f(0.5f, 0.5f, 0.0f, 1.0f), false, false);
		TextureManager::DrawTextureBatched(m_textures[0], ViewPort::Get().getWidth() / 2 - 50, 100, 100.0f * m_player->getPreparationPercentage(), 20.0f, Vector4f(0.8f, 0.8f, 0.0f, 1.0f), false, false);
	}

	//log window
	TextureManager::DrawTextureBatched(m_textures[20], 0, 0, 370.0f, 150.0f, false, false);
	for (unsigned int lineRow = 0; lineRow < m_textDatabase.size() && lineRow < 10; lineRow++) {
		Fontrenderer::Get().addText(*m_interfaceFont, 10, 10 + (lineRow * m_interfaceFont->lineHeight), m_textDatabase[lineRow].text, m_textDatabase[lineRow].color, false);
	}

	/// draw our level beside the experience bar
	Fontrenderer::Get().addText(*m_interfaceFont, 60 - m_interfaceFont->getWidth(Fontrenderer::Get().FloatToString(static_cast<float>(m_player->getLevel()), 0)) / 2, ViewPort::Get().getHeight() - 70, Fontrenderer::Get().FloatToString(static_cast<float>(m_player->getLevel()), 0), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

	//buff effect (it seems meaningless to render this symbol as an additinal frame)
	//TextureManager::BindTexture(m_shortcutFont->spriteSheet, false, 1);
	m_activeSpells = m_player->getActiveSpells();
	for (unsigned int curSpell = 0; curSpell < m_activeSpells.size(); curSpell++) {
		// only draw spells that has a duration.
		if (m_activeSpells[curSpell]->getDuration() > 0) {
			// here we draw the border and background for the spells we have affecting us.
			// healing and buffs will be drawn with a green border and debuffs or hostile spells with a red border..
			Vector4f borderColor = m_activeSpells[curSpell]->isSpellHostile() == true ? Vector4f(0.7f, 0.0f, 0.0f, 1.0f) : Vector4f(0.0f, 0.7f, 0.0f, 1.0f);
			TextureManager::DrawTextureBatched(m_textures[18], ViewPort::Get().getWidth() - 204, ViewPort::Get().getHeight() - 50 - 40 * curSpell, borderColor, false, false);
			TextureManager::DrawTextureBatched(m_textures[20], ViewPort::Get().getWidth() - 204 + 36, ViewPort::Get().getHeight() - 50 - 40 * curSpell, 168.0f, 36.0f, false, false);
			m_activeSpells[curSpell]->drawSymbol(ViewPort::Get().getWidth() - 204 + 2, ViewPort::Get().getHeight() - 50 - 40 * curSpell + 2, 32.0f, 32.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f));

			Fontrenderer::Get().addText(*m_shortcutFont, ViewPort::Get().getWidth() - 204 + 40, ViewPort::Get().getHeight() - 50 + 18 - 40 * curSpell, m_activeSpells[curSpell]->getName(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
			Fontrenderer::Get().addText(*m_shortcutFont, ViewPort::Get().getWidth() - 204 + 40, ViewPort::Get().getHeight() - 50 + 8 - 40 * curSpell, Utils::ConvertTime(m_activeSpells[curSpell]->m_timer.getElapsedTimeSinceRestartSec(), m_activeSpells[curSpell]->getDuration()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		}
	}


	//action bar
	//Vector4f barColor = isMouseOver(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY()) ? Vector4f(0.0f, 0.0f, 0.0f, 0.8f) : Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	TextureManager::DrawTextureBatched(m_textures[20], ViewPort::Get().getWidth() - 630, 0, 630.0f, 80.0f, false, false);
	for (unsigned int buttonId = 0; buttonId < 10; buttonId++) {
		Vector4f borderColor = (m_button[buttonId].action != NULL && m_player->getCurrentSpellActionName() == m_button[buttonId].action->getName()) ? Vector4f(0.8f, 0.8f, 0.8f, 1.0f) : Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
		TextureManager::DrawTextureBatched(m_textures[21], ViewPort::Get().getWidth() - 610 + buttonId * 60, 12, 50.0f, 50.0f, borderColor, false, false);
		Fontrenderer::Get().addText(*m_shortcutFont, m_actionBarPosX + +20 + buttonId * 60 - 8, 54, m_button[buttonId].number.c_str(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
	}

	// draw the cursor if it's supposed to be drawn
	if (isPreparingAoESpell() == true) {
		TextureManager::DrawTextureBatched(m_textures[22], ViewPort::Get().getCursorPosRelX() - m_cursorRadius, ViewPort::Get().getCursorPosRelY() - m_cursorRadius, static_cast<float>(m_cursorRadius * 2), static_cast<float>(m_cursorRadius * 2), false, false);
	}

	m_cooldownSpells = m_player->getCooldownSpells();
	for (unsigned int buttonId = 0; buttonId < 10; buttonId++) {
		bool drawCooldownText = false;
		std::string cooldownText;

		if (m_button[buttonId].action != NULL) {
			bool useableSpell = isSpellUseable(m_button[buttonId].action);

			for (unsigned int curSpell = 0; curSpell < m_cooldownSpells.size(); curSpell++) {
				if (m_cooldownSpells[curSpell]->getName() == m_button[buttonId].action->getName()) {
					useableSpell = false;
					drawCooldownText = true;
					cooldownText = Utils::ConvertTime(m_cooldownSpells[curSpell]->m_timer.getElapsedTimeSinceRestartSec(), m_cooldownSpells[curSpell]->getCooldown());
				}
			}
			
			m_button[buttonId].action->drawSymbol(ViewPort::Get().getWidth() - 608 + buttonId * 60, 14, 46.0f, 46.0f, useableSpell ? Vector4f(1.0f, 1.0f, 1.0f, 1.0f) : Vector4f(1.0f, 0.0f, 0.0f, 1.0f), false);

			if (drawCooldownText == true) {
				
				unsigned int xModifier = m_cooldownFont->getWidth(cooldownText);
				Fontrenderer::Get().addText(*m_cooldownFont, ViewPort::Get().getWidth() - 630 + 20 + buttonId * 60 + 6 + (50 - xModifier) / 2, 32, cooldownText, Vector4f(1.0f, 1.0f, 0.0f, 1.0f), false);
			}
		}
	}

	drawCombatText();
	drawCharacterStates();
	drawTargetedNPCText();

	drawSpellTooltip(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());

	SpellActionBase *spellUnderMouse = getSpellAtMouse(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
	if (spellUnderMouse != NULL) {
		if (m_tooltip != NULL) {
			if (dynamic_cast<SpellTooltip*>(m_tooltip)->getParent() != spellUnderMouse) {
				delete m_tooltip;
				m_tooltip = new SpellTooltip(spellUnderMouse, m_player);
			}
		}
		else {
			m_tooltip = new SpellTooltip(spellUnderMouse, m_player);
		}
		m_tooltip->draw(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
	}

	for (size_t curFrame = 0; curFrame < m_widgets.size(); curFrame++) {
		m_widgets[curFrame]->draw();
	}
	
	ShopCanvas::Get().drawItemTooltip(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
	//if (m_widgets.size() > 0) {
	//	if (m_widgets.back() == &m_spellbook && !m_spellbook.hasFloatingSpell()) {
	//		m_spellbook.drawSpellTooltip(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());		
	//	}else if (m_widgets.back() == &m_inventoryScreen) {
	//		m_inventoryScreen.drawItemTooltip(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
	//	}
	//}


	m_spellbook.drawSpellTooltip(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
	
	InventoryCanvas::Get().drawItemTooltip(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
	

	InventoryCanvas::Get().drawFloatingSelection();
	m_spellbook.drawFloatingSpell();
	ShopCanvas::Get().drawFloatingSelection();

	
}

void Interface::drawCursor(bool drawInGameCursor) {
	if (drawInGameCursor) {
		TextureManager::DrawTextureBatched(m_textures[17], ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY() - 19, false, false);
		TextureManager::DrawBuffer();

	}
}


void Interface::drawCharacterStates() {
	/// draws states for the characters
	/// Todo: charmed
	std::vector<Npc*> zoneNPCs = ZoneManager::Get().getCurrentZone()->getNPCs();
	std::vector<Character*> allCharacters;
	allCharacters.push_back(m_player);
	for (size_t curNPC = 0; curNPC < zoneNPCs.size(); curNPC++) {
		allCharacters.push_back(zoneNPCs[curNPC]);
	}

	for (size_t curCharacter = 0; curCharacter < allCharacters.size(); curCharacter++) {
		/// draws fear symbol
		if (allCharacters[curCharacter]->isFeared() == true) {
			TextureManager::DrawTextureBatched(m_textures[7], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_textures[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, true);
		}
		/// draws stun symbol
		if (allCharacters[curCharacter]->isStunned() == true) {
			TextureManager::DrawTextureBatched(m_textures[8], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_textures[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, true);
		}
		/// draws confused symbol
		if (allCharacters[curCharacter]->isConfused() == true) {
			TextureManager::DrawTextureBatched(m_textures[9], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_textures[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, true);

		}
		/// draws mesmerized symbol
		if (allCharacters[curCharacter]->isMesmerized() == true) {
			TextureManager::DrawTextureBatched(m_textures[10], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_textures[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, true);
		}
	}
}

void Interface::addCombatText(int amount, bool critical, unsigned short damageType, int posX, int posY, bool update) {
	std::stringstream converterStream, damageStream;
	converterStream << amount;
	std::string tempString = converterStream.str();
	int damageNumber;
	int rand_x = 0;
	
	rand_x = RNG::randomInt(-32, 32);

	int characterSpace = 16;

	if (critical == true) {
		characterSpace = 24; // more space between digits if critical (need more space since bigger font)
	}

	// adding every digit in the amount of damage / heal to our struct.
	// adding a random value (rand_x) to x_pos, to spread the damage out a bit so it wont look too clogged.
	for (unsigned int streamCounter = 0; streamCounter < tempString.length(); streamCounter++) {
		damageStream.clear();
		damageStream << tempString.at(streamCounter);
		damageStream >> damageNumber;
		m_damageDisplay.push_back(DamageDisplay(damageNumber, critical, damageType, posX + (streamCounter*characterSpace) + rand_x, posY, update));
	}
}

void Interface::drawCombatText() {
	int k = 0;
	// different color for heal and damage. damage = red, heal = green
	float damageType[2][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } };

	TextureRect *fontTextures;
	float reduce_amount;
	for (size_t currentDamageDisplay = 0; currentDamageDisplay < m_damageDisplay.size(); currentDamageDisplay++) {

		// cleaning up text that is already faded out.
		if (m_damageDisplay[currentDamageDisplay].transparency < 0.0f) {
			m_damageDisplay.erase(m_damageDisplay.begin() + currentDamageDisplay);
			if (currentDamageDisplay >= m_damageDisplay.size()) {
				break;
			}
		}

		if (m_damageDisplay[currentDamageDisplay].critical == true) {
			fontTextures = &m_textures[m_damageDisplay[currentDamageDisplay].digitToDisplay + m_displayOffsetBig];
			reduce_amount = 0.04f;
		}else {
			fontTextures = &m_textures[m_damageDisplay[currentDamageDisplay].digitToDisplay + m_displayOffsetSmall];
			reduce_amount = 0.06f;
		}

		// fading and moving text upwards every 50ms
		if ((m_damageDisplay[currentDamageDisplay].timer.getElapsedTimeMilli()) > 50) {
			m_damageDisplay[currentDamageDisplay].transparency -= reduce_amount;
			m_damageDisplay[currentDamageDisplay].posY++;
			m_damageDisplay[currentDamageDisplay].timer.reset();
		}

		if (m_damageDisplay[currentDamageDisplay].update) {
			k = 1;
		}

		m_damageDisplay[currentDamageDisplay].posX += (m_player->getDeltaX()*k);
		m_damageDisplay[currentDamageDisplay].posY += (m_player->getDeltaY()*k);

		//sets the color of the text we're drawing based on what type of damage type we're displaying.
		Vector4f color = Vector4f(damageType[m_damageDisplay[currentDamageDisplay].damageType][0], damageType[m_damageDisplay[currentDamageDisplay].damageType][1], damageType[m_damageDisplay[currentDamageDisplay].damageType][2], m_damageDisplay[currentDamageDisplay].transparency);
		TextureManager::DrawTextureBatched(*fontTextures, m_damageDisplay[currentDamageDisplay].posX, m_damageDisplay[currentDamageDisplay].posY, color, false, !m_damageDisplay[currentDamageDisplay].update);
	}
}

void Interface::drawTargetedNPCText() {
	if (m_player->getTarget() == nullptr) return;

	Npc* npc = dynamic_cast<Npc*>(m_player->getTarget());

	uint8_t width = 40;
	uint8_t stringWidth = m_interfaceFont->getWidth(npc->getName().c_str());

	if (stringWidth > width + 64) {
		width = stringWidth - 56;
	}

	int fontStart = npc->getXPos() + npc->getWidth() / 2 - stringWidth / 2;
	int tooltipStart = npc->getXPos() + npc->getWidth() / 2 - (width + 64) / 2;

	TextureManager::DrawTextureBatched(m_textures[2], tooltipStart, npc->getYPos() + npc->getHeight() - 3, 32.0f, 32.0f, true, true);
	TextureManager::DrawTextureBatched(m_textures[1], tooltipStart + 32, npc->getYPos() + npc->getHeight() - 3, 40.0f, 32.0f, true, true);
	TextureManager::DrawTextureBatched(m_textures[3], tooltipStart + 32 + 40, npc->getYPos() + npc->getHeight() - 3, 32.0f, 32.0f, true, true);


	float life_percentage = static_cast<float>(npc->getCurrentHealth()) / static_cast<float>(npc->getModifiedMaxHealth());
	TextureManager::DrawTextureBatched(m_textures[0], npc->getXPos(), npc->getYPos() + npc->getHeight(), npc->getWidth()*life_percentage, 8.0f, Vector4f(1.0f - life_percentage, life_percentage, 0.0f, 1.0f), true, true);

	if (npc->continuePreparing()) {
		// draw backdrop first
		TextureManager::DrawTextureBatched(m_textures[0], npc->getXPos(), npc->getYPos() + npc->getHeight() - 12, static_cast<float>(npc->getWidth()), 8.0f, Vector4f(0.5f, 0.5f, 0.0f, 1.0f), true, true);
		//then the actual castbar
		TextureManager::DrawTextureBatched(m_textures[0], npc->getXPos(), npc->getYPos() + npc->getHeight() - 12, npc->getWidth()* npc->getPreparationPercentage(), 8.0f, Vector4f(0.8f, 0.8f, 0.0f, 1.0f), true, true);
		Fontrenderer::Get().addText(*m_interfaceFont, npc->getXPos(), npc->getYPos() + npc->getHeight() - 24, npc->getCurrentSpellActionName(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
	}

	Vector4f color;
	switch (npc->getAttitude()) {
		case Enums::Attitude::FRIENDLY:
			color = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
			break;
		case Enums::Attitude::NEUTRAL:
			color = Vector4f(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case Enums::Attitude::HOSTILE:
			color = Vector4f(1.0f, 0.0f, 0.0f, 1.0f);
			break;
	}

	Fontrenderer::Get().addText(*m_interfaceFont, fontStart, npc->getYPos() + npc->getHeight() + 12, npc->getName(), color, true);

	// load the active spells from the NPC
	std::vector<SpellActionBase*> activeSpells = npc->getActiveSpells();

	for (unsigned int curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		// only draw spells that has a duration.
		if (activeSpells[curSpell]->getDuration() > 0) {
			// here we draw the border and background for the spells we have affecting us.
			// healing and buffs will be drawn with a green border and debuffs or hostile spells with a red border..

			Vector4f color = activeSpells[curSpell]->isSpellHostile() == true ? Vector4f(0.7f, 0.0f, 0.0f, 1.0f) : Vector4f(0.0f, 0.7f, 0.0f, 1.0f);
			TextureManager::DrawTextureBatched(m_textures[5], npc->getXPos() + (19 * curSpell) + 2, npc->getYPos() + npc->getHeight() + 30, 18.0f, 18.0f, color, true, true);
			TextureManager::DrawTextureBatched(m_textures[6], npc->getXPos() + (19 * curSpell) + 2, npc->getYPos() + npc->getHeight() + 30, 18.0f, 18.0f, color, true, true);
			activeSpells[curSpell]->drawSymbol(npc->getXPos() + (19 * curSpell) + 3, npc->getYPos() + npc->getHeight() + 31, 16.0f, 16.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
		}
	}
}

void Interface::drawSpellTooltip(int x, int y) {
	if (isMouseOver(x, y)) {
		int buttonId = getMouseOverButtonId(x, y);

		if (buttonId >= 0 && m_button[buttonId].tooltip != NULL && isPreparingAoESpell() == false && !m_spellbook.hasFloatingSpell())
			m_button[buttonId].tooltip->draw(x, y);
	}
}

bool Interface::isSpellUseable(SpellActionBase* action) {

	// do we have enough fatigue to cast?
	if (dynamic_cast<Action*>(action) != NULL) {
		if (action->getSpellCost() > m_player->getCurrentFatigue()) {
			return false;
		}

		// do we have enough mana to cast?
	}else if (dynamic_cast<Spell*>(action) != NULL) {
		if (action->getSpellCost() > m_player->getCurrentMana()) {
			return false;
		}
	}

	if (action != NULL && action->getNeedTarget() && m_player->getTarget() == NULL) {
		return false;
	}

	// do we have a target? if so, are we in range? (doesn't check for selfaffectign spells)
	if (m_player->getTarget() != NULL && action->getEffectType() != Enums::EffectType::SelfAffectingSpell) {

		if (m_player->getTargetAttitude() == Enums::Attitude::FRIENDLY)
			return false;

		double distance = sqrt(pow((m_player->getXPos() + m_player->getWidth() / 2) - (m_player->getTarget()->getXPos() + m_player->getTarget()->getWidth() / 2), 2) + pow((m_player->getYPos() + m_player->getHeight() / 2) - (m_player->getTarget()->getYPos() + m_player->getTarget()->getHeight() / 2), 2));

		if (action->isInRange(distance) == false)
			return false;
	}

	// are we stunned?
	if (m_player->isStunned() == true || m_player->isFeared() == true || m_player->isMesmerized() == true || m_player->isCharmed() == true) {
		return false;
	}

	// does the spell / action require a weapon of any sort?
	if (action->getRequiredWeapons() != 0) {
		if ((action->getRequiredWeapons() & (m_player->getInventory().getWeaponTypeBySlot(Enums::ItemSlot::MAIN_HAND) | m_player->getInventory().getWeaponTypeBySlot(Enums::ItemSlot::OFF_HAND))) == 0) {
			return false;
		}
	}

	return true;
}

void Interface::bindActionToButtonNr(int buttonNr, SpellActionBase *action) {
	bindAction(&m_button[buttonNr], action);
}

void Interface::bindAction(Button* button, SpellActionBase* action) {
	button->action = action;
	button->tooltip = new SpellTooltip(button->action, m_player);

	/** this could be added to game settings, making the player choose to
	display a full tooltip when hoovering spells in the actionbar.**/
	button->tooltip->enableSmallTooltip();
}

void Interface::unbindAction(Button* button) {
	button->action = NULL;
	delete button->tooltip;
	button->tooltip = NULL;
}


void Interface::dragSpell(Button* spellQueue) {
	if (!m_spellbook.hasFloatingSpell() && !InventoryCanvas::Get().hasFloatingSelection()) {
		m_preparingAoESpell = false;
		m_spellbook.setFloatingSpell(spellQueue->action);
		unbindAction(spellQueue);
		spellQueue = nullptr;
	}
}

void Interface::setSpellQueue(Button& button, bool actionReadyToCast) {
	m_spellQueue = &button;
	m_spellQueue->actionReadyToCast = actionReadyToCast;
}

short Interface::getMouseOverButtonId(int x, int y) {
	for (unsigned short buttonIndex = 0; buttonIndex < m_button.size(); buttonIndex++) {
		if (x > m_actionBarPosX + m_button[buttonIndex].posX &&
			x < m_actionBarPosX + m_button[buttonIndex].posX + (m_button[buttonIndex].width) &&
			y > m_button[buttonIndex].posY &&
			y < m_button[buttonIndex].posY + (m_button[buttonIndex].height)) {
			return buttonIndex;
		}
	}

	return -1;
}

bool Interface::isButtonUsed(Button* button) const {
	 return button->action != nullptr;
}

void Interface::processInput() {

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		m_lastMouseDown = std::pair<int, int>(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
		m_buttonId = getMouseOverButtonId(m_lastMouseDown.first, m_lastMouseDown.second);

		if (m_buttonId >= 0) {
			// we clicked a button which has an action and has no floating spell on the mouse (we're launching an action from the actionbar)
			if (m_button[m_buttonId].action != NULL && !m_spellbook.hasFloatingSpell()/*&& !isPreparingAoESpell()*/) {

				if (isSpellUseable(m_button[m_buttonId].action)) {

					// AoE spell with specific position
					if (m_button[m_buttonId].action->getEffectType() == Enums::EffectType::AreaTargetSpell && m_player->getTarget() == NULL) {
						setSpellQueue(m_button[m_buttonId], false);
						m_cursorRadius = m_button[m_buttonId].action->getRadius();
						// "regular" spell
					} else {
						setSpellQueue(m_button[m_buttonId]);
					}
				}

				if (isPreparingAoESpell()) {
					m_spellQueue = NULL;
					m_preparingAoESpell = false;
				}
			}

			// check to see if we're holding a floating spell on the mouse. if we do, we want to place it in the actionbar slot...
			if (m_spellbook.hasFloatingSpell()) {

				if (isButtonUsed(&m_button[m_buttonId]))
					unbindAction(&m_button[m_buttonId]);

				bindAction(&m_button[m_buttonId], m_spellbook.getFloatingSpell()->action);
				m_spellbook.unsetFloatingSpell();

				//if(isSpellUseable(button[buttonId].action))
				//setSpellQueue(button[buttonId], false);
			}
		}
	}

	if (mouse.buttonDown(Mouse::BUTTON_LEFT)) {
		if ((sqrt(pow(m_lastMouseDown.first - ViewPort::Get().getCursorPosRelX(), 2) + pow(m_lastMouseDown.second - ViewPort::Get().getCursorPosRelY(), 2)) > 5) /*&& !sPreparingAoESpell()*/) {
			if (m_buttonId >= 0) {
				dragSpell(&m_button[m_buttonId]);
			}
		}
	}

	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {

		//remove buff effect
		SpellActionBase *spellUnderMouse = getSpellAtMouse(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
		if (spellUnderMouse != NULL) {
			if (spellUnderMouse->isSpellHostile() == false) {
				m_player->removeActiveSpell(spellUnderMouse);
			}
		}

		if (isPreparingAoESpell()) {
			m_spellQueue = NULL;
			m_preparingAoESpell = false;
		}
	}

	if (isPreparingAoESpell()) {
		makeReadyToCast(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
	}

	if (m_spellQueue != NULL && (mouse.buttonUp(Mouse::BUTTON_LEFT) && !isPreparingAoESpell() || mouse.buttonPressed(Mouse::BUTTON_LEFT) && isPreparingAoESpell())) {
		executeSpellQueue();
	}

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_B)) {
		m_spellbook.isVisible() ? m_spellbook.close() : m_spellbook.activate();
	}

	if (keyboard.keyPressed(Keyboard::KEY_C)) {
		m_characterInfo.isVisible() ? m_characterInfo.close() : m_characterInfo.activate();
	}


	if (keyboard.keyPressed(Keyboard::KEY_I)) {
		InventoryCanvas::Get().isVisible() ? InventoryCanvas::Get().close() : InventoryCanvas::Get().activate();
	}

	if (keyboard.keyPressed(Keyboard::KEY_U)) {
		ShopCanvas::Get().isVisible() ? ShopCanvas::Get().close() : ShopCanvas::Get().activate();
	}

	if (keyboard.keyPressed(Keyboard::KEY_Q)) {
		QuestCanvas::Get().isVisible() ? QuestCanvas::Get().close() : QuestCanvas::Get().activate();
	}

	if (m_activeWidget) m_activeWidget->processInput();

}

void Interface::processInputRightDrag() {
	Mouse &mouse = Mouse::instance();
	bool widgetInteraction = false;
	bool hasSelection = InventoryCanvas::Get().hasFloatingSelection();
	bool hasSpell = m_spellbook.hasFloatingSpell();

	if (mouse.buttonPressed(Mouse::BUTTON_LEFT) || mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it) {
			if ((*it)->isMouseOnFrame(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY())) {
				(*it)->activate();
				widgetInteraction = true;
				break;
			}
		}

	}
	
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		m_buttonId = getMouseOverButtonId(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
		
		if (m_buttonId >= 0 && !widgetInteraction) {
			// we clicked a button which has an action and has no floating spell on the mouse (we're launching an action from the actionbar)
			if (m_button[m_buttonId].action != NULL && !m_spellbook.hasFloatingSpell()/*&& !isPreparingAoESpell()*/) {

				if (isSpellUseable(m_button[m_buttonId].action)) {
					// AoE spell with specific position
					if (m_button[m_buttonId].action->getEffectType() == Enums::EffectType::AreaTargetSpell && m_player->getTarget() == NULL) {
						setSpellQueue(m_button[m_buttonId], false);
						m_cursorRadius = m_button[m_buttonId].action->getRadius();

					// "regular" spell
					}else {
						setSpellQueue(m_button[m_buttonId]);
					}
				}

				if (isPreparingAoESpell()) {
					m_spellQueue = NULL;
					m_preparingAoESpell = false;
				}
			}

			if (m_spellbook.hasFloatingSpell()) {

				if (isButtonUsed(&m_button[m_buttonId]))
					unbindAction(&m_button[m_buttonId]);

				bindAction(&m_button[m_buttonId], m_spellbook.getFloatingSpell()->action);
				m_spellbook.unsetFloatingSpell();

				//if (isSpellUseable(button[buttonId].action) && button[buttonId].action->getEffectType() != Enums::EffectType::AreaTargetSpell) {				
				//setSpellQueue(button[buttonId], false);
				//}
			}
		}

		if (m_spellbook.hasFloatingSpell()) {
			m_spellbook.unsetFloatingSpell();
		}
	}

	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		m_lastMouseDown = std::pair<int, int>(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
		m_buttonId = getMouseOverButtonId(m_lastMouseDown.first, m_lastMouseDown.second);

		//remove buff effect
		SpellActionBase *spellUnderMouse = getSpellAtMouse(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
		if (spellUnderMouse != NULL) {
			if (spellUnderMouse->isSpellHostile() == false) {
				m_player->removeActiveSpell(spellUnderMouse);
			}
		}

		if (isPreparingAoESpell()) {
			m_spellQueue = NULL;
			m_preparingAoESpell = false;
		}
	}

	if (mouse.buttonDown(Mouse::BUTTON_RIGHT)) {
		bool canDrag = true;
		for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it) {
			canDrag &= !(*it)->isMouseOnFrame(ViewPort::Get().getCursorPosRelX(), ViewPort::Get().getCursorPosRelY());
		}
		if (canDrag && (sqrt(pow(m_lastMouseDown.first - ViewPort::Get().getCursorPosRelX(), 2) + pow(m_lastMouseDown.second - ViewPort::Get().getCursorPosRelY(), 2)) > 5) /*&& !sPreparingAoESpell()*/) {
			if (m_buttonId >= 0) {
				
				dragSpell(&m_button[m_buttonId]);
			}
		}
	}

	Keyboard &keyboard = Keyboard::instance();
	if (keyboard.keyPressed(Keyboard::KEY_B)) {
		m_spellbook.isVisible() ? m_spellbook.close() : m_spellbook.activate();
	}

	if (keyboard.keyPressed(Keyboard::KEY_C)) {
		m_characterInfo.isVisible() ? m_characterInfo.close() : m_characterInfo.activate();
	}


	if (keyboard.keyPressed(Keyboard::KEY_I)) {
		InventoryCanvas::Get().isVisible() ? InventoryCanvas::Get().close() : InventoryCanvas::Get().activate();
	}

	if (keyboard.keyPressed(Keyboard::KEY_U)) {
		ShopCanvas::Get().isVisible() ? ShopCanvas::Get().close() : ShopCanvas::Get().activate();
		
	}

	if (keyboard.keyPressed(Keyboard::KEY_Q)) {
		QuestCanvas::Get().isVisible() ? QuestCanvas::Get().close() : QuestCanvas::Get().activate();
	}

	if (m_activeWidget) m_activeWidget->processInput();

	/*if (hasSpell && InventoryCanvas::Get().hasFloatingSelection()) {
		m_spellbook.unsetFloatingSpell();
	}

	if (hasSelection && m_spellbook.hasFloatingSpell()) {
		//m_inventoryScreen.unsetFloatingSelection();
		m_spellbook.unsetFloatingSpell();
	}*/

	if (widgetInteraction) {
		m_spellQueue = NULL;
		m_preparingAoESpell = false;
	}

	if (isPreparingAoESpell()) {
		makeReadyToCast(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
	}

	if (m_spellQueue != NULL && mouse.buttonPressed(Mouse::BUTTON_LEFT) ) {
		executeSpellQueue();
	}

}

void Interface::executeSpellQueue() {
	Enums::EffectType effectType = m_spellQueue->action->getEffectType();

 	if (m_spellQueue->action != NULL && m_spellQueue->actionReadyToCast == true) {
		
		SpellActionBase *curAction = NULL;
		if (effectType == Enums::EffectType::SingleTargetSpell && m_player->getTarget() != NULL) {
			curAction = m_spellQueue->action->cast(m_player, m_player->getTarget(), true);

		}else if (effectType == Enums::EffectType::SelfAffectingSpell) {
			curAction = m_spellQueue->action->cast(m_player, m_player, false);

		}else if (effectType == Enums::EffectType::AreaTargetSpell) {

			// AoE spell cast on target with target selected previous to casting
			if (!m_preparingAoESpell) {
				curAction = m_spellQueue->action->cast(m_player, m_player->getTarget(), false);

			// AoE spell cast on specific position
			}else if (m_spellQueue->areaOfEffectOnSpecificLocation == true) {
				curAction = m_spellQueue->action->cast(m_player, m_spellQueue->actionSpecificXPos, m_spellQueue->actionSpecificYPos);
				m_preparingAoESpell = false;
			}
		}
		
		if (curAction != NULL) {
			m_player->castSpell(dynamic_cast<SpellActionBase*>(curAction));
		}

		m_spellQueue = NULL;

	}else if (m_spellQueue->action) {
		if (effectType == Enums::EffectType::AreaTargetSpell)
			m_preparingAoESpell = true;
	}
}

bool Interface::isPreparingAoESpell() const {
	return m_preparingAoESpell;
}

void Interface::makeReadyToCast(int x, int y) {
	m_spellQueue->actionReadyToCast = true;
	m_spellQueue->areaOfEffectOnSpecificLocation = true;
	m_spellQueue->actionSpecificXPos = x;
	m_spellQueue->actionSpecificYPos = y;
}

bool Interface::isMouseOver(int x, int y) {
	if (x > m_actionBarPosX && x < m_actionBarPosX + 630 && y > m_actionBarPosY && y < m_actionBarPosY + 80) {
		return true;
	}

	return false;
}

SpellActionBase* Interface::getSpellAtMouse(int mouseX, int mouseY) {
	for (unsigned int curSpell = 0; curSpell < m_activeSpells.size(); curSpell++) {
		// only draw spells that has a duration.
		if (m_activeSpells[curSpell]->getDuration() > 0) {
			int spellPosYStart = (ViewPort::Get().getHeight() - 50) - 40 * curSpell;
			int spellPosXStart = ViewPort::Get().getWidth() - 204;
			if (mouseY > spellPosYStart
				&& mouseY < spellPosYStart + 36
				&& mouseX > spellPosXStart
				&& mouseX < spellPosXStart + 204) {
				return m_activeSpells[curSpell];
			}
		}
	}
	return NULL;
}

void Interface::addTextToLog(std::string text, Vector4f color) {
	std::vector<std::string> formattedLines;
	TextWindow::FormatMultilineText(text, formattedLines, 370, &Globals::fontManager.get("verdana_12"));
	for (size_t curLine = 0; curLine < formattedLines.size(); curLine++) {
		m_textDatabase.insert(m_textDatabase.begin(), { formattedLines[curLine], color });
	}
}

void Interface::clearLogWindow() {
	m_textDatabase.clear();
}

void Interface::connectWidget(Widget& widget, bool activate) {

	widget.setOnActivate([&]() {

		if (widget.isVisible()) {
			for (size_t curFrame = 0; curFrame < m_widgets.size(); curFrame++) {
				if (&widget == m_widgets[curFrame]) {
					m_widgets.erase(m_widgets.begin() + curFrame);
					m_widgets.push_back(&widget);
					widget.m_keepInputHandling ? m_activeWidget = nullptr : m_activeWidget = m_widgets.back();
					return;
				}
			}
		}else {

			m_widgets.push_back(&widget);
			widget.setVisible(true);
			widget.m_keepInputHandling ? m_activeWidget = nullptr : m_activeWidget = m_widgets.back();
		}
	});
	if(activate) widget.activate();

	widget.setOnClose([&]() {
		for (size_t curFrame = 0; curFrame < m_widgets.size(); curFrame++) {
			if (&widget == m_widgets[curFrame]) {
				widget.setVisible(false);
				m_widgets.erase(m_widgets.begin() + curFrame);
				m_widgets.size() == 0 ? m_activeWidget = nullptr : m_activeWidget = m_widgets.back();				
				return;
			}
		}
	});
}

void Interface::closeAll() {
	for (auto it = m_widgets.rbegin(); it != m_widgets.rend(); ++it) {
		if (!(*it)->m_keepInputHandling) {
			(*it)->close();
		}
	}
}

Widget* Interface::getActiveWidget() {
	return m_activeWidget;
}

std::vector<Widget*>& Interface::getWidgets() {
	return m_widgets;
}

void Interface::clear() {
	for (size_t curButtonNr = 0; curButtonNr < m_button.size(); ++curButtonNr) {
		if (isButtonUsed(&m_button[curButtonNr]))
			unbindAction(&m_button[curButtonNr]);
	}
}

std::string Interface::getActionBarLuaSaveText() {
	std::ostringstream oss;
	oss << "-- action bar" << std::endl;

	for (size_t curButtonNr = 0; curButtonNr< m_button.size(); ++curButtonNr) {
		if (isButtonUsed(&m_button[curButtonNr])) {
			oss << "DawnInterface.restoreActionBar( " << curButtonNr << ", "
				<< "spellDatabase[ \"" << m_button[curButtonNr].action->getID() << "\" ] );" << std::endl;
		}
	}

	return oss.str();
}