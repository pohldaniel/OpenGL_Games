#include "Editor.h"

Editor::Editor(StateMachine& machine) : State(machine, CurrentState::EDITOR) {
	newZone = new Zone();
	newZone->loadZone("res/_lua/zone1");
	m_editorFocus = ViewPort::get().getBottomLeft();
	m_tilepos”ffset = 0;
	m_tilepos = 0;
	m_currentTilepos = 0;

	initTextures();
	Fontrenderer::get().setCharacterSet(Globals::fontManager.get("verdana_9"));
}

Editor::~Editor() {}

void Editor::fixedUpdate() {

}

void Editor::update() {
	ViewPort::get().update(m_dt);

	Mouse &mouse = Mouse::instance();

	if (mouse.wheelPos() < 0.0f) {
		decTilepos();
	}

	if (mouse.wheelPos() > 0.0f) {		
		incTilepos();
	}
}

void Editor::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);

	newZone->drawZoneBatched();
	
	Batchrenderer::get().bindTexture(m_textureAtlas, true);
	Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("batch"));
	
	TextureManager::DrawTextureBatched(m_interfacetexture[0], m_editorFocus[0], m_editorFocus[1] + ViewPort::get().getHeight() - 100, ViewPort::get().getWidth(), 100.0f, false);	
	TextureManager::DrawTextureBatched(m_interfacetexture[0], m_editorFocus[0], m_editorFocus[1], ViewPort::get().getWidth(), 100.0f, false);
	TextureManager::DrawTextureBatched(m_interfacetexture[1], m_editorFocus[0] + ViewPort::get().getWidth() * 0.5f - 5.0f, m_editorFocus[1] + ViewPort::get().getHeight() - 65, 50.0f, 50.0f, false);
	Batchrenderer::get().drawBuffer(false);
	Batchrenderer::get().unbindTexture(true);

	Batchrenderer::get().bindTexture(newZone->m_textureAtlas, true);

	TileSet* tileSet = EditorInterface::getTileSet();
	std::vector<Tile*> curTiles = tileSet->getAllTilesOfType(TileClassificationType::FLOOR);
	for (m_tilepos = 0; m_tilepos<curTiles.size(); ++m_tilepos) {
		Tile *curTile = curTiles[m_tilepos];
		TextureManager::DrawTextureBatched(curTile->texture, m_editorFocus[0] + ViewPort::get().getWidth() * 0.5f + (m_tilepos * 50) + (m_tilepos”ffset * 50), m_editorFocus[1] + ViewPort::get().getHeight() - 60, 40.0f, 40.0f, false);
	}

	Batchrenderer::get().drawBuffer(false);
	Batchrenderer::get().unbindTexture(true);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&ViewPort::get().getCamera().getOrthographicMatrix()[0][0]);

	glBegin(GL_LINES);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] - 20 + m_editorFocus[0], ViewPort::get().getCursorPos()[1] + m_editorFocus[1], 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] + 20 + m_editorFocus[0], ViewPort::get().getCursorPos()[1] + m_editorFocus[1], 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] + m_editorFocus[0], ViewPort::get().getCursorPos()[1] + 20 + m_editorFocus[1], 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] + m_editorFocus[0], ViewPort::get().getCursorPos()[1] - 20 + m_editorFocus[1], 0.0f);
	glEnd();
	glLoadIdentity();

	int fontHeight = Fontrenderer::get().getFontHeight();
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 90 - fontHeight, "[ Scoll Up/Down ]  Select previous/next object");
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 80 - fontHeight, "[ F1 ]  Next set of objects");
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 70 - fontHeight, "[ DEL ]  Delete object at mouse position");
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 60 - fontHeight, "[ ENTER ]  Place object at mouse position");
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 50 - fontHeight, "[ S ]  Saves the changes into zone1-files");
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 40 - fontHeight, "[ O ]  Load a different zone (not yet implemented)");
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 30 - fontHeight, "[ L ]  Exit the editor");
	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + 20 - fontHeight, "//Press the left mouse button near the sides to scroll around ;-)");

	Fontrenderer::get().drawText(m_editorFocus[0] + 10, m_editorFocus[1] + ViewPort::get().getHeight() - 20, "x: " + Fontrenderer::get().FloatToString(ViewPort::get().getCursorPos()[0], 0) + ", y: " + Fontrenderer::get().FloatToString(ViewPort::get().getCursorPos()[1], 0));

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
	TextureManager::Loadimage("res/edit_backdrop.png", 3, m_interfacetexture);
	TextureManager::Loadimage("res/tile_solid.tga", 4, m_interfacetexture);
	TextureManager::Loadimage("res/circle.tga", 5, m_interfacetexture);

	TextureAtlasCreator::get().shutdown();
}

void Editor::incTilepos(){
	TileSet* curTileSet = EditorInterface::getTileSet();

	if (m_currentTilepos + 1 < curTileSet->getAllTilesOfType(TileClassificationType::FLOOR).size()) {
		m_currentTilepos++;
		m_tilepos”ffset--;
	}
}

void Editor::decTilepos(){
	if (m_currentTilepos > 0){
		m_currentTilepos--;
		m_tilepos”ffset++;
	}
}
