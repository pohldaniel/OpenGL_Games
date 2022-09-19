#include "Editor.h"

Editor::Editor(StateMachine& machine) : State(machine, CurrentState::EDITOR) {
	newZone = new Zone();
	newZone->loadZone("res/_lua/zone1");

	initTextures();
}

Editor::~Editor() {}

void Editor::fixedUpdate() {

}

void Editor::update() {
	ViewPort::get().update(m_dt);
}

void Editor::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);


	newZone->drawZoneBatched();

	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Editor::initTextures() {

	if (m_interfacetexture.size() > 0) {
		return;
	}

	TextureAtlasCreator::get().init(m_textureAtlas, 1024, 1024);

	TextureManager::Loadimage("res/background_editor.tga", 0, m_interfacetexture);
	TextureManager::Loadimage("res/current_tile_backdrop.tga", 1, m_interfacetexture);
	TextureManager::Loadimage("res/tile.tga", 2, m_interfacetexture);
	TextureManager::Loadimage("res/edit_backdrop.tga", 3, m_interfacetexture);
	TextureManager::Loadimage("res/tile_solid.tga", 4, m_interfacetexture);
	TextureManager::Loadimage("res/circle.tga", 5, m_interfacetexture);

	TextureAtlasCreator::get().shutdown();
}