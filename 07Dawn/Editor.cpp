#include "Editor.h"

Editor::Editor(StateMachine& machine) : State(machine, CurrentState::EDITOR) {
	newZone = new Zone();
	newZone->loadZone("res/_lua/zone1");

	m_originalFocus = ViewPort::get().getBottomLeft();
	m_editorFocus = ViewPort::get().getBottomLeft();
	
	m_tileposOffset = 0;
	m_tilepos = 0;
	m_currentTilepos = 0;
	m_selectedTileSet = TileClassificationType::FLOOR;

	initTextures();
	Fontrenderer::get().setCharacterSet(Globals::fontManager.get("verdana_9"));


	LuaFunctions::executeLuaFile("res/_lua/mobdata.lua");
	loadNPCs();
}

Editor::~Editor() {}

void Editor::fixedUpdate() {

}

void Editor::update() {
	ViewPort::get().update(m_dt);
	m_editorFocus = ViewPort::get().getBottomLeft();

	Mouse &mouse = Mouse::instance();
	Keyboard &keyboard = Keyboard::instance();

	if (mouse.wheelPos() < 0.0f) {
		decTilepos();
	}

	if (mouse.wheelPos() > 0.0f) {		
		incTilepos();
	}

	if (keyboard.keyPressed(Keyboard::KEY_ENTER)) {

		Tile *currentTile = EditorInterface::getTileSet(m_selectedTileSet)->getAllTiles()[m_currentTilepos];
		switch (m_selectedTileSet) {
			case TileClassificationType::FLOOR: {
				newZone->changeTile(newZone->locateTile(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])), currentTile);
				break;
			}case TileClassificationType::ENVIRONMENT: {
				newZone->addEnvironment(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]), currentTile, true /* centered on pos */);
				break;
			}case TileClassificationType::SHADOW: {
				newZone->addShadow(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]), currentTile);
				break;
			}/*case 3: // collisionboxes
				zoneToEdit->AddCollisionbox(editorFocus->getX() + mouseX, editorFocus->getY() + mouseY);
				break;
			case 4: // NPCs
				CNPC *curNPC = DawnInterface::addMobSpawnPoint(editorNPCs[current_tilepos].first, editorFocus->getX() + mouseX - 48, editorFocus->getY() + mouseY - 48, 180, 1);
				curNPC->setAttitude(Attitude::HOSTILE);
				break;
			}*/
		}
		
	}

	if (keyboard.keyPressed(Keyboard::KEY_DELETE)) {

		switch (m_selectedTileSet) {
		case TileClassificationType::FLOOR: // tiles
			newZone->deleteTile(newZone->locateTile(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])));
			break;
		case TileClassificationType::ENVIRONMENT: // environment
			if (newZone->deleteEnvironment(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])) == 0)
				m_objectEditSelected = -1;
			break;
		case TileClassificationType::SHADOW: // shadows
			if (newZone->deleteShadow(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])) == 0)
				m_objectEditSelected = -1;
			break;
		/*case 3: // collisionboxes
			if (newZone-->DeleteCollisionbox(editorFocus->getX() + mouseX, editorFocus->getY() + mouseY) == 0)
				m_objectEditSelected = -1;
			break;
		case 4: // NPCs
			if (newZone-->DeleteNPC(editorFocus->getX() + mouseX, editorFocus->getY() + mouseY) == 0)
			{
				zoneToEdit->cleanupNPCList();
				m_objectEditSelected = -1;
			}
			break;*/
		}

		
	}

	if (keyboard.keyPressed(Keyboard::KEY_F1)) {
		m_currentTilepos = 0;
		m_tileposOffset = 0;
		m_objectEditSelected = -1;

		m_selectedTileSet = static_cast<TileClassificationType::TileClassificationType>((m_selectedTileSet + 1) % TileClassificationType::COUNT);
	}
}
	

void Editor::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);


	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);

	newZone->drawZoneBatched();
	
	Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("batch"));

	Batchrenderer::get().bindTexture(m_textureAtlas, true);
	
	
	TextureManager::DrawTextureBatched(m_interfacetexture[0], m_originalFocus[0], m_originalFocus[1] + ViewPort::get().getHeight() - 100, ViewPort::get().getWidth(), 100.0f, false);
	TextureManager::DrawTextureBatched(m_interfacetexture[0], m_originalFocus[0], m_originalFocus[1], ViewPort::get().getWidth(), 100.0f, false);
	TextureManager::DrawTextureBatched(m_interfacetexture[1], m_originalFocus[0] + ViewPort::get().getWidth() * 0.5f - 5.0f, m_originalFocus[1] + ViewPort::get().getHeight() - 65, 50.0f, 50.0f, false);
	Batchrenderer::get().drawBuffer(false);
	

	if (m_selectedTileSet == TileClassificationType::COLLISION) {
		
		for (unsigned int x = 0; x < newZone->collisionMap.size(); x++) {
			TextureManager::DrawTextureBatched(m_interfacetexture[4], newZone->collisionMap[x].x, newZone->collisionMap[x].y, newZone->collisionMap[x].w, newZone->collisionMap[x].h, true, Vector4f(0.7f, 0.1f, 0.6f, 0.8f));
			
		}
		Batchrenderer::get().drawBuffer(true);
	}else if (m_selectedTileSet == TileClassificationType::NPC) {
		Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("batch"));
		Batchrenderer::get().bindTexture(TextureManager::GetTextureAtlas("Skeleton_Archer"), true);

		for (size_t curNPC = 0; curNPC < editorNPCs.size(); curNPC++) {
			TextureRect& rect = editorNPCs[curNPC].second->getTileSet(ActivityType::ActivityType::Walking)->getAllTiles()[4]->texture;
			TextureManager::DrawTextureBatched(rect, m_originalFocus[0] + ViewPort::get().getWidth() * 0.5f + (curNPC * 50) + (m_tileposOffset * 50) - 48 + 20, m_originalFocus[1] + ViewPort::get().getHeight() - 40 - 48, rect.width, rect.height, false);
		}
		Batchrenderer::get().drawBuffer(true);
	}else {

		Batchrenderer::get().bindTexture(TextureManager::GetTextureAtlas(newZone->getName()), true);
		std::vector<Tile*> curTiles = EditorInterface::getTileSet(m_selectedTileSet)->getAllTiles();


		for (m_tilepos = 0; m_tilepos < curTiles.size(); ++m_tilepos) {
			Tile *curTile = curTiles[m_tilepos];
			TextureManager::DrawTextureBatched(curTile->texture, m_originalFocus[0] + ViewPort::get().getWidth() * 0.5f + (m_tilepos * 50) + (m_tileposOffset * 50), m_originalFocus[1] + ViewPort::get().getHeight() - 60, 40.0f, 40.0f, false);
		}

		Batchrenderer::get().drawBuffer(false);
		
	}
	Batchrenderer::get().unbindTexture(true);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(&ViewPort::get().getCamera().getOrthographicMatrix()[0][0]);

	glBegin(GL_LINES);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] - 20 + m_originalFocus[0], ViewPort::get().getCursorPos()[1] + m_originalFocus[1], 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] + 20 + m_originalFocus[0], ViewPort::get().getCursorPos()[1] + m_originalFocus[1], 0.0f);
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] + m_originalFocus[0], ViewPort::get().getCursorPos()[1] + 20 + m_originalFocus[1], 0.0f);
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(ViewPort::get().getCursorPos()[0] + m_originalFocus[0], ViewPort::get().getCursorPos()[1] - 20 + m_originalFocus[1], 0.0f);
	glEnd();
	glLoadIdentity();

	int fontHeight = Fontrenderer::get().getFontHeight();
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 90 - fontHeight, "[ Scoll Up/Down ]  Select previous/next object");
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 80 - fontHeight, "[ F1 ]  Next set of objects");
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 70 - fontHeight, "[ DEL ]  Delete object at mouse position");
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 60 - fontHeight, "[ ENTER ]  Place object at mouse position");
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 50 - fontHeight, "[ S ]  Saves the changes into zone1-files");
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 40 - fontHeight, "[ O ]  Load a different zone (not yet implemented)");
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 30 - fontHeight, "[ L ]  Exit the editor");
	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + 20 - fontHeight, "//Press the left mouse button near the sides to scroll around ;-)");

	Fontrenderer::get().drawText(m_originalFocus[0] + 10, m_originalFocus[1] + ViewPort::get().getHeight() - 20, "x: " + Fontrenderer::get().FloatToString(ViewPort::get().getCursorPos()[0], 0) + ", y: " + Fontrenderer::get().FloatToString(ViewPort::get().getCursorPos()[1], 0));

	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void Editor::initTextures() {

	if (m_interfacetexture.size() > 0) {
		return;
	}

	TextureAtlasCreator::get().init(1024, 1024);

	TextureManager::Loadimage("res/background_editor.tga", 0, m_interfacetexture);
	TextureManager::Loadimage("res/current_tile_backdrop.tga", 1, m_interfacetexture);
	TextureManager::Loadimage("res/tile.tga", 2, m_interfacetexture);
	TextureManager::Loadimage("res/edit_backdrop.png", 3, m_interfacetexture);
	TextureManager::Loadimage("res/tile_solid.tga", 4, m_interfacetexture);
	TextureManager::Loadimage("res/circle.tga", 5, m_interfacetexture);

	m_textureAtlas = TextureAtlasCreator::get().getAtlas();
}

void Editor::loadNPCs(){
	editorNPCs.clear();
	for (std::map< std::string, CCharacter* >::iterator curNPC = Globals::allMobTypes.begin(); curNPC != Globals::allMobTypes.end(); curNPC++) {
		editorNPCs.push_back(std::pair< std::string, CCharacter* >((*curNPC).first, (*curNPC).second));
	}
}

void Editor::incTilepos(){

	if (m_selectedTileSet == TileClassificationType::NPC) {
		if (m_currentTilepos + 1 < editorNPCs.size()){
			m_currentTilepos++;
			m_tileposOffset--;
		}
	}else {

		TileSet* curTileSet = EditorInterface::getTileSet(m_selectedTileSet);

		if (m_currentTilepos + 1 < curTileSet->numberOfTiles()) {
			m_currentTilepos++;
			m_tileposOffset--;
		}
	}
}

void Editor::decTilepos(){
	if (m_currentTilepos > 0){
		m_currentTilepos--;
		m_tileposOffset++;
	}
}