#include "CharacterInfo.h"
#include "Constants.h"

CharacterInfo  CharacterInfo::s_instance;

CharacterInfo & CharacterInfo::Get() {
	return s_instance;
}

CharacterInfo::CharacterInfo() : Widget(25, 100, 434, 492){

}

void CharacterInfo::setPlayer(Player* player) {
	m_player = player;
}

void CharacterInfo::init() {
	m_infoFont = &Globals::fontManager.get("verdana_12");

	TextureAtlasCreator::get().init(1024, 1024);

	TextureManager::Loadimage("res/interface/CharacterInfoScreen/background.tga", 0, m_textures);
	TextureManager::Loadimage("res/white2x2pixel.tga", 1, m_textures, true);
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

}

void CharacterInfo::drawExpBar() {

}

void CharacterInfo::drawTabs() {

}