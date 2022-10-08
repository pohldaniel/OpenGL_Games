#include "Interface.h"

Interface::Interface() { 
	LoadTextures();
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

}

void Interface::DrawCursor(bool hideInGameCursor) {
	if (hideInGameCursor) {
		Batchrenderer::get().bindTexture(m_textureAtlas, true);
		TextureManager::DrawTextureBatched(m_interfacetexture[15], static_cast<float>(ViewPort::get().getCursorPos()[0]), static_cast<float>(ViewPort::get().getCursorPos()[1]) - 19, false, false);
		Batchrenderer::get().drawBuffer(false);
	}	
}