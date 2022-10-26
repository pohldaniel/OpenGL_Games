#include "Interface.h"
#include "Npc.h"
#include "Zone.h"
#include "Player.h"


/*void sSpellSlot::initFont() {
	font = &Globals::fontManager.get("verdana_11");
}*/

Interface Interface::s_instance;

Interface::Interface() : floatingSpellSlot(sSpellSlot(0, 0, 0, 0)) {

}

Interface& Interface::Get() {
	return s_instance;
}

void Interface::SetPlayer(Player* _player) {
	player = _player;
}

void Interface::init() {

	loadTextures();
	player = &Player::Get();

	shortcutFont = &Globals::fontManager.get("verdana_10");
	cooldownFont = &Globals::fontManager.get("verdana_11");
	interfaceFont = &Globals::fontManager.get("verdana_12");

	button.push_back(sButton(22, 14, 46, 46, "1", Keyboard::Key::KEY_1));
	button.push_back(sButton(82, 14, 46, 46, "2", Keyboard::Key::KEY_2));
	button.push_back(sButton(142, 14, 46, 46, "3", Keyboard::Key::KEY_3));
	button.push_back(sButton(202, 14, 46, 46, "4", Keyboard::Key::KEY_4));
	button.push_back(sButton(262, 14, 46, 46, "5", Keyboard::Key::KEY_5));
	button.push_back(sButton(322, 14, 46, 46, "6", Keyboard::Key::KEY_6));
	button.push_back(sButton(382, 14, 46, 46, "7", Keyboard::Key::KEY_7));
	button.push_back(sButton(442, 14, 46, 46, "8", Keyboard::Key::KEY_8));
	button.push_back(sButton(502, 14, 46, 46, "9", Keyboard::Key::KEY_9));
	button.push_back(sButton(562, 14, 46, 46, "0", Keyboard::Key::KEY_0));

	actionBarPosX = ViewPort::get().getWidth() - 630;
	actionBarPosY = 0;
	preparingAoESpell = false;
}

void Interface::resize() {
	actionBarPosX = ViewPort::get().getWidth() - 630;
}

void Interface::loadTextures() {


	TextureAtlasCreator::get().init(1024, 1024);

	TextureManager::Loadimage("res/lifebar.tga", 0, m_interfacetexture);
	TextureManager::Loadimage("res/interface/tooltip/NPCTarget_background.tga", 1, m_interfacetexture);
	TextureManager::Loadimage("res/interface/tooltip/NPCTarget_left.tga", 2, m_interfacetexture);
	TextureManager::Loadimage("res/interface/tooltip/NPCTarget_right.tga", 3, m_interfacetexture);
	TextureManager::Loadimage("res/white2x2pixel.tga", 4, m_interfacetexture);
	TextureManager::Loadimage("res/interface/BuffWindow/frame.tga", 5, m_interfacetexture);

	TextureManager::Loadimage("res/interface/BuffWindow/background.tga", 6, m_interfacetexture);
	TextureManager::Loadimage("res/fear.tga", 7, m_interfacetexture);
	TextureManager::Loadimage("res/stun.tga", 8, m_interfacetexture);
	TextureManager::Loadimage("res/confused.tga", 9, m_interfacetexture);
	TextureManager::Loadimage("res/mesmerized.tga", 10, m_interfacetexture);

	TextureManager::Loadimage("res/interface/Portrait/base.tga", 11, m_interfacetexture);
	TextureManager::Loadimage("res/interface/Portrait/bar.tga", 12, m_interfacetexture);
	TextureManager::Loadimage("res/interface/Portrait/bar_small.tga", 13, m_interfacetexture);
	TextureManager::Loadimage("res/interface/Portrait/Warrior.tga", 14, m_interfacetexture);
	TextureManager::Loadimage("res/cursors/pointer2.png", 15, m_interfacetexture);
	TextureManager::Loadimage("res/interface/BuffWindow/frame.tga", 16, m_interfacetexture);
	TextureManager::Loadimage("res/interface/BuffWindow/background.tga", 17, m_interfacetexture);
	TextureManager::Loadimage("res/interface/blended_bg.tga", 18, m_interfacetexture);

	TextureManager::Loadimage("res/border.tga", 19, m_interfacetexture);
	TextureManager::Loadimage("res/cursors/circle1_enabled.tga", 20, m_interfacetexture);
	TextureManager::Loadimage("res/cursors/circle1_disabled.tga", 21, m_interfacetexture);

	TextureManager::Loadimage("res/interface/spellbook/base.tga", 22, m_interfacetexture);
	TextureManager::Loadimage("res/interface/spellbook/placeholder.tga", 23, m_interfacetexture);
	TextureManager::Loadimage("res/interface/spellbook/arrow_right.tga", 24, m_interfacetexture);
	TextureManager::Loadimage("res/interface/spellbook/arrow_left.tga", 25, m_interfacetexture);

	m_textureAtlas = TextureAtlasCreator::get().getAtlas();

	//Spritesheet::Safe("interface", m_textureAtlas);

	glBindTexture(GL_TEXTURE_2D_ARRAY, TextureManager::GetTextureAtlas("symbols"));
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

	/*glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureAtlas);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);*/
}

void Interface::DrawInterface() {

	TextureManager::BindTexture(m_textureAtlas, true);
	
	TextureManager::DrawTexture(m_interfacetexture[14], 4, ViewPort::get().getHeight() - 68, false, false);
	TextureManager::DrawTextureBatched(m_interfacetexture[11], 0, 50 + ViewPort::get().getHeight() - m_interfacetexture[11].height, false, false);

	float lifeBarPercentage = 0.5f;
	float manaBarPercentage = 0.5f;
	float fatigueBarPercentage = 0.1f;
	//uint64_t neededXP = (player->getExpNeededForLevel(player->getLevel() + 1)) - player->getExpNeededForLevel(player->getLevel());
	//uint64_t currentXP = player->getExperience() - player->getExpNeededForLevel(player->getLevel());
	float experienceBarPercentage = 0.3f;

	// health bar
	TextureManager::DrawTextureBatched(m_interfacetexture[12], 76, ViewPort::get().getHeight() - 35, lifeBarPercentage * 123.0f, static_cast<float>(m_interfacetexture[12].height), Vector4f(0.815f, 0.16f, 0.16f, 1.0f), false, false);
	// mana bar
	if(player->getArchType() == Enums::CharacterArchType::Caster) {
		TextureManager::DrawTextureBatched(m_interfacetexture[12], 76, ViewPort::get().getHeight() - 53, manaBarPercentage * 123.0f, static_cast<float>(m_interfacetexture[12].height), Vector4f(0.16f, 0.576f, 0.815f, 1.0f), false, false);
	
	}
	
	// fatigue bar
	if (player->getArchType() == Enums::CharacterArchType::Fighter){
		Vector4f color;
		if (fatigueBarPercentage <= 0.33) {
			color = Vector4f(0.109f, 0.917f, 0.047f, 1.0f);

		} else if (fatigueBarPercentage >= 0.34 && fatigueBarPercentage <= 0.66) {
			color = Vector4f(0.917f, 0.847f, 0.047f, 1.0f);

		} else {
			color = Vector4f(0.917f, 0.047f, 0.047f, 1.0f);

		}
		TextureManager::DrawTextureBatched(m_interfacetexture[12], 76, ViewPort::get().getHeight() - 53, fatigueBarPercentage * 123.0f, static_cast<float>(m_interfacetexture[12].height), color, false, false);
	}

	// fatigue bar
	TextureManager::DrawTextureBatched(m_interfacetexture[13], 76, ViewPort::get().getHeight() - 67, experienceBarPercentage * 123.0f, static_cast<float>(m_interfacetexture[13].height), false, false);

	if (player->getIsPreparing()){
		// actual castbar
		TextureManager::DrawTextureBatched(m_interfacetexture[0], ViewPort::get().getWidth() / 2 - 50, 100, 100.0f, 20.0f, Vector4f(0.5f, 0.5f, 0.0f, 1.0f), false, false);
		TextureManager::DrawTextureBatched(m_interfacetexture[0], ViewPort::get().getWidth() / 2 - 50, 100, 100.0f * player->getPreparationPercentage(), 20.0f, Vector4f(0.8f, 0.8f, 0.0f, 1.0f), false, false);
	}

	//log window
	TextureManager::DrawTextureBatched(m_interfacetexture[18], 0, 0, 390.0f, 150.0f, false, false);
	TextureManager::DrawBuffer(false);
	
	/// draw our level beside the experience bar
	Fontrenderer::Get().drawText(*interfaceFont, 60 - interfaceFont->getWidth(Fontrenderer::Get().FloatToString(static_cast<float>(player->getLevel()), 0)) / 2, ViewPort::get().getHeight() -70, Fontrenderer::Get().FloatToString(static_cast<float>(player->getLevel()), 0), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);

	//buff effect (it seems meaningless to render this symbol as an additinal frame)
	TextureManager::BindTexture(m_textureAtlas, true, 0);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("symbols"), true, 1);
	TextureManager::BindTexture(shortcutFont->spriteSheet, false, 2);

	TextureManager::SetShader(Globals::shaderManager.getAssetPointer("batch_font"));
	Fontrenderer::Get().setRenderer(&Batchrenderer::Get());

	activeSpells = player->getActiveSpells();
	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		// only draw spells that has a duration.
		if (activeSpells[curSpell].first->getDuration() > 0) {
			// here we draw the border and background for the spells we have affecting us.
			// healing and buffs will be drawn with a green border and debuffs or hostile spells with a red border..

			Vector4f borderColor = activeSpells[curSpell].first->isSpellHostile() == true ? Vector4f(0.7f, 0.0f, 0.0f, 1.0f) : Vector4f(0.0f, 0.7f, 0.0f, 1.0f);
			TextureManager::DrawTextureBatched(m_interfacetexture[16], ViewPort::get().getWidth() - 204, ViewPort::get().getHeight() - 50 - 40 * curSpell, borderColor, false, false);
			TextureManager::DrawTextureBatched(m_interfacetexture[17], ViewPort::get().getWidth() - 204 + 36, ViewPort::get().getHeight() - 50 - 40 * curSpell, 168.0f, 36.0f, false, false);
			activeSpells[curSpell].first->drawSymbol(ViewPort::get().getWidth() - 204 + 2, ViewPort::get().getHeight() - 50 - 40 * curSpell + 2, 32.0f, 32.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), 1u);

			Fontrenderer::Get().addText(*shortcutFont, ViewPort::get().getWidth() - 204 + 40, ViewPort::get().getHeight() - 50 + 18 - 40 * curSpell, activeSpells[curSpell].first->getName(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false, 2u);
			Fontrenderer::Get().addText(*shortcutFont, ViewPort::get().getWidth() - 204 + 40, ViewPort::get().getHeight() - 50 + 8 - 40 * curSpell, ConvertTime(activeSpells[curSpell].second, activeSpells[curSpell].first->getDuration()), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false, 2u);
		}
	}
	Fontrenderer::Get().resetRenderer();

	TextureManager::DrawBuffer(false);
	TextureManager::SetShader(Globals::shaderManager.getAssetPointer("batch"));
	TextureManager::UnbindTexture(true, 2);
	TextureManager::UnbindTexture(false, 1);
	TextureManager::UnbindTexture(false, 0);

	//action bar
	//Vector4f barColor = isMouseOver(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY()) ? Vector4f(0.0f, 0.0f, 0.0f, 0.8f) : Vector4f(0.0f, 0.0f, 0.0f, 1.0f);
	TextureManager::DrawTextureBatched(m_interfacetexture[18], ViewPort::get().getWidth() - 630, 0, 630.0f, 80.0f, false, false);
	for (unsigned int buttonId = 0; buttonId < 10; buttonId++) {
		Vector4f borderColor = ( button[buttonId].action != NULL && player->getCurrentSpellActionName() == button[buttonId].action->getName()) ? Vector4f(0.8f, 0.8f, 0.8f, 1.0f) : Vector4f(0.4f, 0.4f, 0.4f, 1.0f);
		TextureManager::DrawTextureBatched(m_interfacetexture[19], ViewPort::get().getWidth() - 610 + buttonId * 60, 12, 50.0f, 50.0f, borderColor, false, false);
	}

	// draw the cursor if it's supposed to be drawn
	if (isPreparingAoESpell() == true) {
		TextureManager::DrawTextureBatched(m_interfacetexture[20], ViewPort::get().getCursorPosRelX() - cursorRadius, ViewPort::get().getCursorPosRelY() - cursorRadius, cursorRadius * 2, cursorRadius * 2, false, false);
	}
	drawCombatText();
	drawCharacterStates();
	TextureManager::DrawBuffer(false);

	drawTargetedNPCText();

	TextureManager::DrawBuffer(true);

	TextureManager::BindTexture(TextureManager::GetTextureAtlas("symbols"), true, 1);
	TextureManager::BindTexture(cooldownFont->spriteSheet, false, 2);
	TextureManager::SetShader(Globals::shaderManager.getAssetPointer("batch_font"));
	Fontrenderer::Get().setRenderer(&Batchrenderer::Get());

	cooldownSpells = player->getCooldownSpells();
	for (unsigned int buttonId = 0; buttonId < 10; buttonId++) {
		bool drawCooldownText;
		std::string cooldownText;
		bool useableSpell = isSpellUseable(button[buttonId].action);
		
		if (button[buttonId].action != NULL) {

			for (size_t curSpell = 0; curSpell < cooldownSpells.size(); curSpell++) {
				if (cooldownSpells[curSpell].first->getName() == button[buttonId].action->getName()) {
					useableSpell = false;
					drawCooldownText = true;
					cooldownText = ConvertTime(cooldownSpells[curSpell].second, cooldownSpells[curSpell].first->getCooldown());
				}
			}
			
			button[buttonId].action->drawSymbol(ViewPort::get().getWidth() - 608 + buttonId * 60, 14, 46.0f, 46.0f, useableSpell ? Vector4f(1.0f, 1.0f, 1.0f, 1.0f) : Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 1u);

			if (drawCooldownText == true){
				unsigned int xModifier = cooldownFont->getWidth(cooldownText);
				Fontrenderer::Get().addText(*cooldownFont, ViewPort::get().getWidth() - 630 + 20 + buttonId * 60 + 6 + (static_cast<float>(50) - xModifier) / 2, 32, cooldownText, Vector4f(1.0f, 1.0f, 0.0f, 1.0f), false, 2u);

			}
		}
	}
	
	Fontrenderer::Get().resetRenderer();

	TextureManager::DrawBuffer(false);
	TextureManager::SetShader(Globals::shaderManager.getAssetPointer("batch"));
	TextureManager::UnbindTexture(false, 2);
	TextureManager::UnbindTexture(true, 1);
	TextureManager::UnbindTexture(true, 0);

	CSpellActionBase *spellUnderMouse = getSpellAtMouse(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());
	if (spellUnderMouse != NULL) {
		if (tooltip != NULL) {
			if (dynamic_cast<SpellTooltip*>(tooltip)->getParent() != spellUnderMouse) {
				delete tooltip;
				tooltip = new SpellTooltip(spellUnderMouse, player);
			}
		}else {
			tooltip = new SpellTooltip(spellUnderMouse, player);
		}
		tooltip->draw(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());
	}
}

void Interface::DrawCursor(bool drawInGameCursor) {
	if (drawInGameCursor) {
		TextureManager::BindTexture(m_textureAtlas, true);
		TextureManager::DrawTextureBatched(m_interfacetexture[15], ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY() - 19, false, false);
		TextureManager::DrawBuffer(false);
	}
}

void Interface::DrawFloatingSpell() {

	if (floatingSpell != NULL){
		TextureManager::BindTexture(m_textureAtlas, true);
		TextureManager::DrawTexture(m_interfacetexture[23], ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY() +20, 50.0f, 50.0f, false, true);

		TextureManager::BindTexture(TextureManager::GetTextureAtlas("symbols"), true,0);

		
		// draw the spell icon
		floatingSpell->action->drawSymbol(ViewPort::get().getCursorPosX() + 2, ViewPort::get().getCursorPosY() + 22, 46.0f, 46.0f, Vector4f(1.0f, 0.0f, 0.0f, 1.0f), 0u);
		TextureManager::DrawBuffer(true);


		// draw the spell name
		//floatingSpell->font->drawText(world_x + mouseX + 25 - floatingSpell->font->calcStringWidth(floatingSpell->action->getName()) / 2,
			//world_y + mouseY + 20 - floatingSpell->font->getHeight() - 5,
			//floatingSpell->action->getName());
	}
}

void Interface::drawCharacterStates() {
	/// draws states for the characters
	/// Todo: charmed
	std::vector<Npc*> zoneNPCs = ZoneManager::Get().getCurrentZone()->getNPCs();
	std::vector<Character*> allCharacters;
	allCharacters.push_back(player);
	for (size_t curNPC = 0; curNPC < zoneNPCs.size(); curNPC++) {
		allCharacters.push_back(zoneNPCs[curNPC]);
	}

	for (size_t curCharacter = 0; curCharacter < allCharacters.size(); curCharacter++) {
		/// draws fear symbol
		if (allCharacters[curCharacter]->isFeared() == true) {
			TextureManager::DrawTextureBatched(m_interfacetexture[7], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_interfacetexture[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, false);
		}
		/// draws stun symbol
		if (allCharacters[curCharacter]->isStunned() == true) {
			TextureManager::DrawTextureBatched(m_interfacetexture[8], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_interfacetexture[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, false);
		}
		/// draws confused symbol
		if (allCharacters[curCharacter]->isConfused() == true) {
			TextureManager::DrawTextureBatched(m_interfacetexture[9], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_interfacetexture[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, false);

		}
		/// draws mesmerized symbol
		if (allCharacters[curCharacter]->isMesmerized() == true) {
			TextureManager::DrawTextureBatched(m_interfacetexture[10], allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - m_interfacetexture[7].width / 2, allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, false, false);
		}
	}
}

void Interface::addCombatText(int amount, bool critical, unsigned char damageType, int x_pos, int y_pos, bool update) {
	std::stringstream converterStream, damageStream;
	converterStream << amount;
	std::string tempString = converterStream.str();
	int damageNumber;
	int rand_x = 0;

	rand_x = RNG::randomInt(-32, 32);

	int characterSpace = 16;

	if (critical == true){
		characterSpace = 24; // more space between digits if critical (need more space since bigger font)
	}

	// adding every digit in the amount of damage / heal to our struct.
	// adding a random value (rand_x) to x_pos, to spread the damage out a bit so it wont look too clogged.
	for (size_t streamCounter = 0; streamCounter < tempString.length(); streamCounter++){
		damageStream.clear();
		damageStream << tempString.at(streamCounter);
		damageStream >> damageNumber;
		damageDisplay.push_back(sDamageDisplay(damageNumber, critical, damageType, x_pos + (streamCounter*characterSpace) + rand_x, y_pos, update));
	}
}

void Interface::drawCombatText() {
	int k = 0;
	// different color for heal and damage. damage = red, heal = green
	GLfloat damageType[2][3] = { { 1.0f, 0.0f, 0.0f },{ 0.0f, 1.0f, 0.0f } };

	TextureRect *fontTextures;
	float reduce_amount;

	for (size_t currentDamageDisplay = 0; currentDamageDisplay < damageDisplay.size(); currentDamageDisplay++) {

		// cleaning up text that is already faded out.
		if (damageDisplay[currentDamageDisplay].transparency < 0.0f) {
			damageDisplay.erase(damageDisplay.begin() + currentDamageDisplay);
			if (currentDamageDisplay >= damageDisplay.size()) {
				break;
			}
		}

		if (damageDisplay[currentDamageDisplay].critical == true) {
			fontTextures = &damageDisplayTexturesBig;
			reduce_amount = 0.04;
		}else {
			fontTextures = &damageDisplayTexturesSmall;
			reduce_amount = 0.06;
		}

		// fading and moving text upwards every 50ms
		damageDisplay[currentDamageDisplay].thisFrame = Globals::clock.getElapsedTimeMilli();
		if ((damageDisplay[currentDamageDisplay].thisFrame - damageDisplay[currentDamageDisplay].lastFrame) > 50) {
			damageDisplay[currentDamageDisplay].transparency -= reduce_amount;
			damageDisplay[currentDamageDisplay].y_pos++;
			damageDisplay[currentDamageDisplay].lastFrame = damageDisplay[currentDamageDisplay].thisFrame;
		}

		if (damageDisplay[currentDamageDisplay].update) {
			k = 1;
		}

		damageDisplay[currentDamageDisplay].x_pos += (player->getDeltaX()*k);
		damageDisplay[currentDamageDisplay].y_pos += (player->getDeltaY()*k);

		//sets the color of the text we're drawing based on what type of damage type we're displaying.
		Vector4f color = Vector4f(damageType[damageDisplay[currentDamageDisplay].damageType][0], damageType[damageDisplay[currentDamageDisplay].damageType][1], damageType[damageDisplay[currentDamageDisplay].damageType][2], damageDisplay[currentDamageDisplay].transparency);
		TextureManager::DrawTextureBatched(*fontTextures, damageDisplay[currentDamageDisplay].x_pos, damageDisplay[currentDamageDisplay].y_pos, color, false, false);
	}
}

void Interface::drawTargetedNPCText() {
	if (player->getTarget() == nullptr) return;

	Npc* npc = dynamic_cast<Npc*>(player->getTarget());

	uint8_t width = 40;
	uint8_t stringWidth = interfaceFont->getWidth(npc->getName().c_str());

	if (stringWidth > width + 64){
		width = stringWidth - 56;
	}

	int fontStart = npc->x_pos + npc->getWidth() / 2 - stringWidth / 2;
	int tooltipStart = npc->x_pos + npc->getWidth() / 2 - (width + 64) / 2;

	TextureManager::DrawTextureBatched(m_interfacetexture[2], tooltipStart, npc->y_pos + npc->getHeight() - 3, 32.0f, 32.0f, true, true);
	TextureManager::DrawTextureBatched(m_interfacetexture[1], tooltipStart +32, npc->y_pos + npc->getHeight() - 3, 40.0f, 32.0f, true, true);
	TextureManager::DrawTextureBatched(m_interfacetexture[3], tooltipStart + 32 + 40, npc->y_pos + npc->getHeight() - 3, 32.0f, 32.0f, true, true);
	
	
	float life_percentage = static_cast<float>(npc->getCurrentHealth()) / static_cast<float>(npc->getModifiedMaxHealth());
	TextureManager::DrawTextureBatched(m_interfacetexture[0], npc->getXPos(), npc->getYPos() + npc->getHeight(), npc->getWidth()*life_percentage, 8.0f, Vector4f(1.0f - life_percentage, life_percentage, 0.0f, 1.0f), true, true);
	
	if (true) {
		// draw backdrop first
		TextureManager::DrawTextureBatched(m_interfacetexture[0], npc->getXPos(), npc->getYPos() + npc->getHeight() - 12, static_cast<float>(npc->getWidth()), 8.0f, Vector4f(0.5f, 0.5f, 0.0f, 1.0f), true, true);
		//then the actual castbar
		TextureManager::DrawTextureBatched(m_interfacetexture[0], npc->getXPos(), npc->getYPos() + npc->getHeight() - 12, npc->getWidth()* npc->getPreparationPercentage(), 8.0f, Vector4f(0.8f, 0.8f, 0.0f, 1.0f), true, true);
		Fontrenderer::Get().addText(*interfaceFont, npc->getXPos(), npc->y_pos + npc->getHeight() - 24, npc->getCurrentSpellActionName(), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
	}

	TextureManager::DrawBuffer(true);

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

	Fontrenderer::Get().bindTexture(*interfaceFont);
	Fontrenderer::Get().addText(*interfaceFont, fontStart, npc->y_pos + npc->getHeight() + 12, npc->getName(), color, true);
	Fontrenderer::Get().drawBuffer(true);
	
	// load the active spells from the NPC
	std::vector<std::pair<CSpellActionBase*, uint32_t> > activeSpells = npc->getActiveSpells();

	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++) {
		// only draw spells that has a duration.
		if (activeSpells[curSpell].first->getDuration() > 0) {
			// here we draw the border and background for the spells we have affecting us.
			// healing and buffs will be drawn with a green border and debuffs or hostile spells with a red border..

			Vector4f color = activeSpells[curSpell].first->isSpellHostile() == true ? Vector4f(0.7f, 0.0f, 0.0f, 1.0f) : Vector4f(0.0f, 0.7f, 0.0f, 1.0f);
			TextureManager::DrawTextureBatched(m_interfacetexture[5], npc->getXPos() + (19 * curSpell) + 2, npc->getYPos() + npc->getHeight() + 30, 18.0f, 18.0f, color, true, true);
			TextureManager::DrawTextureBatched(m_interfacetexture[6], npc->getXPos() + (19 * curSpell) + 2, npc->getYPos() + npc->getHeight() + 30, 18.0f, 18.0f, color, true, true);
			activeSpells[curSpell].first->drawSymbol(npc->getXPos() + (19 * curSpell) + 3, npc->getYPos() + npc->getHeight() + 31, 16.0f, 16.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
		}
	}
}

bool Interface::isSpellUseable(CSpellActionBase* action) {
	// do we have enough fatigue to cast?
	if (dynamic_cast<CAction*>(action) != NULL) {
		if (action->getSpellCost() > player->getCurrentFatigue()) {
			return false;
		}

	// do we have enough mana to cast?
	} else if (dynamic_cast<CSpell*>(action) != NULL) {
		if (action->getSpellCost() > player->getCurrentMana()) {
			return false;
		}
	}

	if (action != NULL && action->getNeedTarget() && player->getTarget() == NULL) {
		return false;
	}

	// do we have a target? if so, are we in range? (doesn't check for selfaffectign spells)
	if (player->getTarget() != NULL && action->getEffectType() != Enums::EffectType::SelfAffectingSpell) {

		if (player->getTargetAttitude() == Enums::Attitude::FRIENDLY)
			return false;

		uint16_t distance = sqrt(pow((player->getXPos() + player->getWidth() / 2) - (player->getTarget()->getXPos() + player->getTarget()->getWidth() / 2), 2) + pow((player->getYPos() + player->getHeight() / 2) - (player->getTarget()->getYPos() + player->getTarget()->getHeight() / 2), 2));

		if (action->isInRange(distance) == false)
			return false;
	}

	// are we stunned?
	if (player->isStunned() == true || player->isFeared() == true || player->isMesmerized() == true || player->isCharmed() == true) {
		return false;
	}

	// does the spell / action require a weapon of any sort?
	/*if (action->getRequiredWeapons() != 0) {
		if ((action->getRequiredWeapons() & (player->getInventory()->getWeaponTypeBySlot(ItemSlot::MAIN_HAND) | player->getInventory()->getWeaponTypeBySlot(ItemSlot::OFF_HAND))) == 0) {
			return false;
		}
	}*/

	return true;
}

void Interface::bindActionToButtonNr(int buttonNr, CSpellActionBase *action) {
	bindAction(&button[buttonNr], action);
}

void Interface::bindAction(sButton *button, CSpellActionBase* action) {
	button->action = action;
	button->tooltip = new SpellTooltip(button->action, player);

	/** this could be added to game settings, making the player choose to
	display a full tooltip when hoovering spells in the actionbar.**/
	button->tooltip->enableSmallTooltip();
}

void Interface::unbindAction(sButton *button) {
	button->action = NULL;
	delete button->tooltip;
	button->tooltip = NULL;
}

sSpellSlot* Interface::getFloatingSpell() const {
	return floatingSpell;
}

void Interface::setFloatingSpell(CSpellActionBase* newFloatingSpell) {
	floatingSpellSlot.action = newFloatingSpell;
	floatingSpell = &floatingSpellSlot;
}

void Interface::unsetFloatingSpell() {
	floatingSpell = NULL;
}

bool Interface::hasFloatingSpell() const {
	return floatingSpell != NULL;
}

void Interface::dragSpell(sButton *_spellQueue) {
	if (!hasFloatingSpell()) {
		preparingAoESpell = false;
		setFloatingSpell(_spellQueue->action);
		unbindAction(_spellQueue);
		spellQueue = NULL;
	}
}

void Interface::setSpellQueue(sButton &button, bool actionReadyToCast) {
	spellQueue = &button;
	spellQueue->actionReadyToCast = actionReadyToCast;
}

int8_t Interface::getMouseOverButtonId(int x, int y) {
	for (size_t buttonIndex = 0; buttonIndex < button.size(); buttonIndex++) {
		if (x > actionBarPosX + button[buttonIndex].posX &&
			x < actionBarPosX + button[buttonIndex].posX + (button[buttonIndex].width) &&
			y > button[buttonIndex].posY &&
			y < button[buttonIndex].posY + (button[buttonIndex].height)) {
			return buttonIndex;
		}
	}

	return -1;
}

bool Interface::isButtonUsed(sButton *button) const {
	return button->action != NULL;
}

void Interface::processInput() {

	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		m_lastMouseDown = std::pair<int, int>(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());
		buttonId = getMouseOverButtonId(m_lastMouseDown.first, m_lastMouseDown.second);

		if (buttonId >= 0) {
			// we clicked a button which has an action and has no floating spell on the mouse (we're launching an action from the actionbar)
			if (button[buttonId].action != NULL && !hasFloatingSpell()/*&& !isPreparingAoESpell()*/) {
					
				if (isSpellUseable(button[buttonId].action)) {

					// AoE spell with specific position
					if (button[buttonId].action->getEffectType() == Enums::EffectType::AreaTargetSpell && player->getTarget() == NULL) {
						setSpellQueue(button[buttonId], false);
						cursorRadius = button[buttonId].action->getRadius();
					// "regular" spell
					}else {
						setSpellQueue(button[buttonId]);
					}
				}

				if (isPreparingAoESpell()) {
					spellQueue = NULL;
					preparingAoESpell = false;
				}	
			}
			
			// check to see if we're holding a floating spell on the mouse. if we do, we want to place it in the actionbar slot...
			if (hasFloatingSpell()) {
				
				if (isButtonUsed(&button[buttonId]))
					unbindAction(&button[buttonId]);
				
				bindAction(&button[buttonId], getFloatingSpell()->action);
				unsetFloatingSpell();

				//if(isSpellUseable(button[buttonId].action))
					//setSpellQueue(button[buttonId], false);
			}
		}
	}

	if (mouse.buttonDown(Mouse::BUTTON_LEFT)) {
		if ((sqrt(pow(m_lastMouseDown.first - ViewPort::get().getCursorPosRelX(), 2) + pow(m_lastMouseDown.second - ViewPort::get().getCursorPosRelY(), 2)) > 5) /*&& !sPreparingAoESpell()*/) {
			if (buttonId >= 0) {
				dragSpell(&button[buttonId]);
			}
		}
	}

	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		CSpellActionBase *spellUnderMouse = getSpellAtMouse(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());
		if (spellUnderMouse != NULL) {
			if (spellUnderMouse->isSpellHostile() == false) {
				player->removeActiveSpell(spellUnderMouse);
			}
		}
	}

	if (isPreparingAoESpell()) {
		makeReadyToCast(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY());
	}

	if (spellQueue != NULL && (mouse.buttonUp(Mouse::BUTTON_LEFT) && !isPreparingAoESpell() || mouse.buttonPressed(Mouse::BUTTON_LEFT) && isPreparingAoESpell())) {	
		executeSpellQueue();
	}
}

void Interface::processInputRightDrag() {
	Mouse &mouse = Mouse::instance();
	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		buttonId = getMouseOverButtonId(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());

		if (buttonId >= 0) {
			// we clicked a button which has an action and has no floating spell on the mouse (we're launching an action from the actionbar)
			if (button[buttonId].action != NULL && !hasFloatingSpell()/*&& !isPreparingAoESpell()*/) {

				if (isSpellUseable(button[buttonId].action)) {
					// AoE spell with specific position
					if (button[buttonId].action->getEffectType() == Enums::EffectType::AreaTargetSpell && player->getTarget() == NULL) {
						setSpellQueue(button[buttonId], false);
						cursorRadius = button[buttonId].action->getRadius();

					// "regular" spell
					}else {
						setSpellQueue(button[buttonId]);
					}
				}

				if (isPreparingAoESpell()) {
					spellQueue = NULL;
					preparingAoESpell = false;
				}
			}

			if (hasFloatingSpell()) {

				if (isButtonUsed(&button[buttonId]))
					unbindAction(&button[buttonId]);

				bindAction(&button[buttonId], getFloatingSpell()->action);
				unsetFloatingSpell();

				//if (isSpellUseable(button[buttonId].action) && button[buttonId].action->getEffectType() != Enums::EffectType::AreaTargetSpell) {				
					//setSpellQueue(button[buttonId], false);
				//}
			}
		}
	}

	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		m_lastMouseDown = std::pair<int, int>(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());
		buttonId = getMouseOverButtonId(m_lastMouseDown.first, m_lastMouseDown.second);

		CSpellActionBase *spellUnderMouse = getSpellAtMouse(ViewPort::get().getCursorPosRelX(), ViewPort::get().getCursorPosRelY());
		if (spellUnderMouse != NULL) {
			if (spellUnderMouse->isSpellHostile() == false) {
				player->removeActiveSpell(spellUnderMouse);
			}
		}
	}

	if (mouse.buttonDown(Mouse::BUTTON_RIGHT)) {
		
			if ((sqrt(pow(m_lastMouseDown.first - ViewPort::get().getCursorPosRelX(), 2) + pow(m_lastMouseDown.second - ViewPort::get().getCursorPosRelY(), 2)) > 5) /*&& !sPreparingAoESpell()*/) {			
				if (buttonId >= 0) {
				dragSpell(&button[buttonId]);
			}
		}
	}

	if (isPreparingAoESpell()) {
		makeReadyToCast(ViewPort::get().getCursorPosX(), ViewPort::get().getCursorPosY());
	}

	if (spellQueue != NULL && mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		executeSpellQueue();
	}
}

CSpellActionBase* Interface::getCurrentAction() {
	return spellQueue != nullptr ? spellQueue->action : nullptr;
}

void Interface::executeSpellQueue() {
		Enums::EffectType effectType = spellQueue->action->getEffectType();

		if (spellQueue->action != NULL && spellQueue->actionReadyToCast == true) {
			CSpellActionBase *curAction = NULL;
			if (effectType == Enums::EffectType::SingleTargetSpell && player->getTarget() != NULL) {
				curAction = spellQueue->action->cast(player, player->getTarget(), true);
			}else if (effectType == Enums::EffectType::SelfAffectingSpell) {
				curAction = spellQueue->action->cast(player, player, false);
			}else if (effectType == Enums::EffectType::AreaTargetSpell) {
				
				// AoE spell cast on target with target selected previous to casting
				if (!preparingAoESpell) {
					curAction = spellQueue->action->cast(player, player->getTarget(), false);

				// AoE spell cast on specific position
				} else if (spellQueue->areaOfEffectOnSpecificLocation == true) {
					curAction = spellQueue->action->cast(player, spellQueue->actionSpecificXPos, spellQueue->actionSpecificYPos);
					preparingAoESpell = false;
				}
			}

			if (curAction != NULL) {			
				player->castSpell(dynamic_cast<CSpellActionBase*>(curAction));
				player->m_waitForAnimation = true;
			}

			spellQueue = NULL;
		}else if (spellQueue->action) {			
			if (effectType == Enums::EffectType::AreaTargetSpell)
				preparingAoESpell = true;
		}	
}

bool Interface::isPreparingAoESpell() const {
	return preparingAoESpell;
}

void Interface::makeReadyToCast(int x, int y) {
	spellQueue->actionReadyToCast = true;
	spellQueue->areaOfEffectOnSpecificLocation = true;
	spellQueue->actionSpecificXPos = x;
	spellQueue->actionSpecificYPos = y;
}

bool Interface::isMouseOver(int x, int y) {
	if (x > actionBarPosX && x < actionBarPosX + 630 && y > actionBarPosY && y <actionBarPosY + 80){
		return true;
	}

	return false;
}

CSpellActionBase* Interface::getSpellAtMouse(int mouseX, int mouseY) {
	for (size_t curSpell = 0; curSpell < activeSpells.size(); curSpell++){
		// only draw spells that has a duration.
		if (activeSpells[curSpell].first->getDuration() > 0) {
			int spellPosYStart = (ViewPort::get().getHeight() - 50) - 40 * curSpell;
			int spellPosXStart = ViewPort::get().getWidth() - 204;
			if (mouseY > spellPosYStart
				&& mouseY < spellPosYStart + 36
				&& mouseX > spellPosXStart
				&& mouseX < spellPosXStart + 204) {
				return activeSpells[curSpell].first;
			}
		}
	}
	return NULL;
}