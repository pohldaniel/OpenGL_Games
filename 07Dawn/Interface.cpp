#include "Interface.h"
#include "Npc.h"
#include "Zone.h"
#include "Player.h"

Interface::Interface() { 
	LoadTextures();
	player = &Player::Get();
}

void Interface::SetPlayer(Character *player_) {
	player = player_;
}

void Interface::initFonts(){
	//NPCTextFont = FontCache::getFontFromCache("data/verdana.ttf", 12);
	//levelFont = FontCache::getFontFromCache("data/verdana.ttf", 12);
	
}

void Interface::LoadTextures() {


	TextureAtlasCreator::get().init(512, 512);

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


	m_textureAtlas = TextureAtlasCreator::get().getAtlas();

	//Spritesheet::Safe("interface", m_textureAtlas);
}

void Interface::DrawInterface() {

	Batchrenderer::get().bindTexture(m_textureAtlas, true);
	TextureManager::DrawTextureBatched(m_interfacetexture[14], 4, static_cast<float>(ViewPort::get().getHeight()) - 68, false, false);
	TextureManager::DrawTextureBatched(m_interfacetexture[11], 0, 50 + static_cast<float>(ViewPort::get().getHeight()) - m_interfacetexture[11].height, false, false);

	float lifeBarPercentage = 0.5f;
	float manaBarPercentage = 0.5f;
	float fatigueBarPercentage = 0.1f;
	//uint64_t neededXP = (player->getExpNeededForLevel(player->getLevel() + 1)) - player->getExpNeededForLevel(player->getLevel());
	//uint64_t currentXP = player->getExperience() - player->getExpNeededForLevel(player->getLevel());
	float experienceBarPercentage = 0.3f;

	// health bar
	TextureManager::DrawTextureBatched(m_interfacetexture[12], 76, static_cast<float>(ViewPort::get().getHeight()) - 35, lifeBarPercentage * 123, m_interfacetexture[12].height, Vector4f(0.815f, 0.16f, 0.16f, 1.0f), false, false);
	// mana bar
	if (true) {
		TextureManager::DrawTextureBatched(m_interfacetexture[12], 76, static_cast<float>(ViewPort::get().getHeight()) - 53, manaBarPercentage * 123, m_interfacetexture[12].height, Vector4f(0.16f, 0.576f, 0.815f, 1.0f), false, false);

	/// fatigue bar
	}else {
		Vector4f color;
		if (fatigueBarPercentage <= 0.33){
			color = Vector4f(0.109f, 0.917f, 0.047f, 1.0f);
		}else if (fatigueBarPercentage >= 0.34 && fatigueBarPercentage <= 0.66){
			color = Vector4f(0.917f, 0.847f, 0.047f, 1.0f);

		}else{
			color = Vector4f(0.917f, 0.047f, 0.047f, 1.0f);

		}
		TextureManager::DrawTextureBatched(m_interfacetexture[12], 76, static_cast<float>(ViewPort::get().getHeight()) - 53, fatigueBarPercentage * 123, m_interfacetexture[12].height, color, false, false);
	}

	TextureManager::DrawTextureBatched(m_interfacetexture[13], 76, static_cast<float>(ViewPort::get().getHeight()) - 67, experienceBarPercentage * 123, m_interfacetexture[13].height, false, false);


	// actual castbar
	TextureManager::DrawTextureBatched(m_interfacetexture[0], static_cast<float>(ViewPort::get().getWidth() * 0.5f) - 50, 100, 100 , 20, Vector4f(0.5f, 0.5f, 0.0f, 1.0f), false, false);	
	TextureManager::DrawTextureBatched(m_interfacetexture[0], static_cast<float>(ViewPort::get().getWidth() * 0.5f) - 50, 100, 100 * 0.3f, 20, Vector4f(0.8f, 0.8f, 0.0f, 1.0f), false, false);

	drawCharacterStates();
	Batchrenderer::get().drawBuffer(false);


}

void Interface::DrawCursor(bool hideInGameCursor) {
	if (hideInGameCursor) {
		Batchrenderer::get().bindTexture(m_textureAtlas, true);
		TextureManager::DrawTextureBatched(m_interfacetexture[15], static_cast<float>(ViewPort::get().getCursorPos()[0]), static_cast<float>(ViewPort::get().getCursorPos()[1]) - 19, false, false);
		Batchrenderer::get().drawBuffer(false);
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
		/*if (allCharacters[curCharacter]->isFeared() == true) {
			DrawingHelpers::mapTextureToRect(interfacetextures.getTexture(7),
				allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - interfacetextures.getTexture(7).width / 2, interfacetextures.getTexture(7).width,
				allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, interfacetextures.getTexture(7).height);
		}
		/// draws stun symbol
		if (allCharacters[curCharacter]->isStunned() == true) {
			DrawingHelpers::mapTextureToRect(interfacetextures.getTexture(8),
				allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - interfacetextures.getTexture(8).width / 2, interfacetextures.getTexture(8).width,
				allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, interfacetextures.getTexture(8).height);
		}
		/// draws confused symbol
		if (allCharacters[curCharacter]->isConfused() == true) {
			DrawingHelpers::mapTextureToRect(interfacetextures.getTexture(9),
				allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - interfacetextures.getTexture(9).width / 2, interfacetextures.getTexture(9).width,
				allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, interfacetextures.getTexture(9).height);
		}
		/// draws mesmerized symbol
		if (allCharacters[curCharacter]->isMesmerized() == true) {
			DrawingHelpers::mapTextureToRect(interfacetextures.getTexture(10),
				allCharacters[curCharacter]->getXPos() + allCharacters[curCharacter]->getWidth() / 2 - interfacetextures.getTexture(10).width / 2, interfacetextures.getTexture(10).width,
				allCharacters[curCharacter]->getYPos() + allCharacters[curCharacter]->getHeight() / 2, interfacetextures.getTexture(10).height);
		}*/
	}
}