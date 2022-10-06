#include "Editor.h"

Editor::Editor(StateMachine& machine) : State(machine, CurrentState::EDITOR) {
	LuaFunctions::executeLuaFile("res/_lua/mobdata.lua");

	newZone = &ZoneManager::Get().getZone("res/_lua/zone1");
	newZone->loadZone();
	
	m_originalFocus = ViewPort::get().getBottomLeft();
	m_editorFocus = ViewPort::get().getBottomLeft();
	
	m_tileposOffset = 0;
	m_tilepos = 0;
	m_currentTilepos = 0;
	m_selectedTileSet = Enums::TileClassificationType::FLOOR;
	adjacencyModeEnabled = false;

	initTextures();
	loadNPCs();	

	
	LuaFunctions::executeLuaFile("res/_lua/gameinit.lua");
	LuaFunctions::executeLuaFile("res/_lua/tileAdjacency.lua");
}

Editor::~Editor() {}

void Editor::fixedUpdate() {

}

void Editor::update() {
	newZone->update(m_dt);

	ViewPort::get().update(m_dt);
	m_editorFocus = ViewPort::get().getBottomLeft();

	Mouse &mouse = Mouse::instance();
	Keyboard &keyboard = Keyboard::instance();

	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		m_currentFocus = m_editorFocus;
		int previousSelectedObjectId = m_selectedObjectId;

		switch (m_selectedTileSet) {
			case Enums::TileClassificationType::ENVIRONMENT:
				m_selectedObjectId = newZone->locateEnvironment(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]));
				break;
			case Enums::TileClassificationType::SHADOW: // shadows
				m_selectedObjectId = newZone->locateShadow(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]));
				break;
			case Enums::TileClassificationType::COLLISION:
				m_selectedObjectId = newZone->locateCollisionbox(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]));
				break;
			case Enums::TileClassificationType::NPC: // NPCs
				m_selectedObjectId = newZone->locateNPC(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]));
			default:
				curAdjacentTiles.clear();
				break;
		}

		if (previousSelectedObjectId != m_selectedObjectId){
			// something new was selected. Update adjacency-list if necessary
			if (adjacencyModeEnabled){
				updateAdjacencyList();
			}
		}
	}

	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		if (adjacencyModeEnabled) {
			placeAdjacentTile();
		}
	}

	if (mouse.wheelPos() < 0.0f) {
		if (adjacencyModeEnabled) {
			applyAdjacencyModification(-1);
		}
		else {
			decTilepos();
		}
	}

	if (mouse.wheelPos() > 0.0f) {
		if (adjacencyModeEnabled) {
			applyAdjacencyModification(1);
		}
		else {
			incTilepos();
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_ENTER)) {
	
		switch (m_selectedTileSet) {
			case Enums::TileClassificationType::FLOOR: {
				const Tile& currentTile = EditorInterface::getTileSet(m_selectedTileSet).getAllTiles()[m_currentTilepos];
				newZone->replaceTile(newZone->locateTile(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])), currentTile);
				break;
			}case Enums::TileClassificationType::ENVIRONMENT: {
				const Tile& currentTile = EditorInterface::getTileSet(m_selectedTileSet).getAllTiles()[m_currentTilepos];
				newZone->addEnvironment(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]), currentTile, true /* centered on pos */);
				break;
			}case Enums::TileClassificationType::SHADOW: {
				const Tile& currentTile = EditorInterface::getTileSet(m_selectedTileSet).getAllTiles()[m_currentTilepos];
				newZone->addShadow(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]), currentTile);
				break;
			}case Enums::TileClassificationType::COLLISION: {
				newZone->addCollisionbox(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]));
				break;
			}case Enums::TileClassificationType::NPC: {
				DawnInterface::addMobSpawnPoint(editorNPCs[m_currentTilepos].first, static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]) - 48, static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]) - 48, 180, 1, Enums::Attitude::HOSTILE);
				break;
			}
		}		
	}

	if (keyboard.keyPressed(Keyboard::KEY_DELETE)) {

		switch (m_selectedTileSet) {
		case Enums::TileClassificationType::FLOOR: // tiles
			newZone->deleteTile(newZone->locateTile(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])));
			break;
		case Enums::TileClassificationType::ENVIRONMENT: // environment
			if (newZone->deleteEnvironment(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])) == 0)
				m_selectedObjectId = -1;
			break;
		case Enums::TileClassificationType::SHADOW: // shadows
			if (newZone->deleteShadow(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])) == 0)
				m_selectedObjectId = -1;
			break;
		case Enums::TileClassificationType::COLLISION: // collisionboxes
			if (newZone->deleteCollisionbox(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])) == 0)
				m_selectedObjectId = -1;
			break;
		case Enums::TileClassificationType::NPC: // NPCs
			if (newZone->deleteNPC(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1])) == 0){
				newZone->cleanupNPCList();
				m_selectedObjectId = -1;
			}
			break;
		}	
	}

	switch (m_selectedTileSet ) {	
		if (m_selectedObjectId >= 0) {
			case Enums::TileClassificationType::ENVIRONMENT: {
				if (keyboard.keyDown(Keyboard::KEY_SHIFT)) {
					if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
						newZone->m_environmentMap[m_selectedObjectId].height -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_UP)) {
						newZone->m_environmentMap[m_selectedObjectId].height += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
						newZone->m_environmentMap[m_selectedObjectId].width -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
						newZone->m_environmentMap[m_selectedObjectId].width += 1;
					}

					if (keyboard.keyDown(Keyboard::KEY_1)) {
						if (newZone->m_environmentMap[m_selectedObjectId].red >= 0.01f) {
							newZone->m_environmentMap[m_selectedObjectId].red -= 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_2)) {
						if (newZone->m_environmentMap[m_selectedObjectId].green >= 0.01f) {
							newZone->m_environmentMap[m_selectedObjectId].green -= 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_3)) {
						if (newZone->m_environmentMap[m_selectedObjectId].blue >= 0.01f) {
							newZone->m_environmentMap[m_selectedObjectId].blue -= 0.01f;
						}
					}
				}else {
					bool moveonce = keyboard.keyDown(Keyboard::KEY_CTRL);

					if (moveonce ? keyboard.keyPressed(Keyboard::KEY_DOWN) : keyboard.keyDown(Keyboard::KEY_DOWN)) {
						newZone->m_environmentMap[m_selectedObjectId].y_pos -= 1;
					}
					if (moveonce ? keyboard.keyPressed(Keyboard::KEY_UP) : keyboard.keyDown(Keyboard::KEY_UP)) {
						newZone->m_environmentMap[m_selectedObjectId].y_pos += 1;
					}
					if (moveonce ? keyboard.keyPressed(Keyboard::KEY_LEFT) : keyboard.keyDown(Keyboard::KEY_LEFT)) {
						newZone->m_environmentMap[m_selectedObjectId].x_pos -= 1;
					}
					if (moveonce ? keyboard.keyPressed(Keyboard::KEY_RIGHT) : keyboard.keyDown(Keyboard::KEY_RIGHT)) {
						newZone->m_environmentMap[m_selectedObjectId].x_pos += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_1)) {
						if (newZone->m_environmentMap[m_selectedObjectId].red <= 0.99f) {
							newZone->m_environmentMap[m_selectedObjectId].red += 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_2)) {
						if (newZone->m_environmentMap[m_selectedObjectId].green <= 0.99f) {
							newZone->m_environmentMap[m_selectedObjectId].green += 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_3)) {
						if (newZone->m_environmentMap[m_selectedObjectId].blue <= 0.99f) {
							newZone->m_environmentMap[m_selectedObjectId].blue += 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_COMMA)) {
						if (newZone->m_environmentMap[m_selectedObjectId].transparency <= 0.99f) {
							newZone->m_environmentMap[m_selectedObjectId].transparency += 0.01f;
						}
					}

					if (keyboard.keyDown(Keyboard::KEY_PERIOD)) {
						if (newZone->m_environmentMap[m_selectedObjectId].transparency >= 0.01f) {
							newZone->m_environmentMap[m_selectedObjectId].transparency -= 0.01f;
						}
					}

					if (keyboard.keyDown(Keyboard::KEY_B)) {
						newZone->m_environmentMap[m_selectedObjectId].z_pos++;
					}

					if (keyboard.keyDown(Keyboard::KEY_N)) {
						if (newZone->m_environmentMap[m_selectedObjectId].z_pos > 0) {
							newZone->m_environmentMap[m_selectedObjectId].z_pos--;
						}
					}
				}
				break;
			}case Enums::TileClassificationType::SHADOW: {
				if (keyboard.keyDown(Keyboard::KEY_SHIFT)) {
					if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
						newZone->m_shadowMap[m_selectedObjectId].height -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_UP)) {
						newZone->m_shadowMap[m_selectedObjectId].height += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
						newZone->m_shadowMap[m_selectedObjectId].width -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
						newZone->m_shadowMap[m_selectedObjectId].width += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_1)) {
						if (newZone->m_shadowMap[m_selectedObjectId].red >= 0.01f) {
							newZone->m_shadowMap[m_selectedObjectId].red -= 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_2)) {
						if (newZone->m_shadowMap[m_selectedObjectId].green >= 0.01f) {
							newZone->m_shadowMap[m_selectedObjectId].green -= 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_3)) {
						if (newZone->m_shadowMap[m_selectedObjectId].blue >= 0.01f) {
							newZone->m_shadowMap[m_selectedObjectId].blue -= 0.01f;
						}
					}
				}else {
					if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
						newZone->m_shadowMap[m_selectedObjectId].y_pos -= 1;
					}

					if (keyboard.keyDown(Keyboard::KEY_UP)) {
						newZone->m_shadowMap[m_selectedObjectId].y_pos += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
						newZone->m_shadowMap[m_selectedObjectId].x_pos -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
						newZone->m_shadowMap[m_selectedObjectId].x_pos += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_1)) {
						if (newZone->m_shadowMap[m_selectedObjectId].red <= 0.99f) {
							newZone->m_shadowMap[m_selectedObjectId].red += 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_2)) {
						if (newZone->m_shadowMap[m_selectedObjectId].green <= 0.99f) {
							newZone->m_shadowMap[m_selectedObjectId].green += 0.01f;
						}
					}
					if (keyboard.keyDown(Keyboard::KEY_3)) {
						if (newZone->m_shadowMap[m_selectedObjectId].blue <= 0.99f) {
							newZone->m_shadowMap[m_selectedObjectId].blue += 0.01f;
						}
					}

					if (keyboard.keyDown(Keyboard::KEY_COMMA)) {
						if (newZone->m_shadowMap[m_selectedObjectId].transparency <= 0.99f) {
							newZone->m_shadowMap[m_selectedObjectId].transparency += 0.01f;
						}
					}

					if (keyboard.keyDown(Keyboard::KEY_PERIOD)) {
						if (newZone->m_shadowMap[m_selectedObjectId].transparency >= 0.01f) {
							newZone->m_shadowMap[m_selectedObjectId].transparency -= 0.01f;
						}
					}
				}
				break;
			}case Enums::TileClassificationType::COLLISION: {
				if (keyboard.keyDown(Keyboard::KEY_SHIFT)) {
					if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
						newZone->m_collisionMap[m_selectedObjectId].h -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_UP)) {
						newZone->m_collisionMap[m_selectedObjectId].h += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
						newZone->m_collisionMap[m_selectedObjectId].w -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
						newZone->m_collisionMap[m_selectedObjectId].w += 1;
					}
				}else {
					if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
						newZone->m_collisionMap[m_selectedObjectId].y -= 1;
					}

					if (keyboard.keyDown(Keyboard::KEY_UP)) {
						newZone->m_collisionMap[m_selectedObjectId].y += 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
						newZone->m_collisionMap[m_selectedObjectId].x -= 1;
					}
					if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
						newZone->m_collisionMap[m_selectedObjectId].x += 1;
					}					
				}
				break;
			}case Enums::TileClassificationType::NPC: {
				if (keyboard.keyDown(Keyboard::KEY_DOWN)) {
					newZone->getNPCs()[m_selectedObjectId]->y_spawn_pos--;
					newZone->getNPCs()[m_selectedObjectId]->y_pos--;
				}

				if (keyboard.keyDown(Keyboard::KEY_UP)) {
					newZone->getNPCs()[m_selectedObjectId]->y_spawn_pos++;
					newZone->getNPCs()[m_selectedObjectId]->y_pos++;
				}
				if (keyboard.keyDown(Keyboard::KEY_LEFT)) {
					newZone->getNPCs()[m_selectedObjectId]->x_spawn_pos--;
					newZone->getNPCs()[m_selectedObjectId]->x_pos--;
				}
				if (keyboard.keyDown(Keyboard::KEY_RIGHT)) {
					newZone->getNPCs()[m_selectedObjectId]->x_spawn_pos++;
					newZone->getNPCs()[m_selectedObjectId]->x_pos++;
				}
				break;
			}
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_F1)) {
		m_currentTilepos = 0;
		m_tileposOffset = 0;
		m_selectedObjectId = -1;
		m_selectedTileSet = static_cast<Enums::TileClassificationType>((m_selectedTileSet + 1) % Enums::TileClassificationType::COUNT);
	}

	if (keyboard.keyPressed(Keyboard::KEY_M)) {
		adjacencyModeEnabled = !adjacencyModeEnabled;
		if (adjacencyModeEnabled){
			updateAdjacencyList();
		}
	}

	if (mouse.buttonPressed(Mouse::BUTTON_MIDDLE)) {
		adjacencyModeEnabled = !adjacencyModeEnabled;
		if (adjacencyModeEnabled) {
			updateAdjacencyList();
		}
	}

	if (keyboard.keyPressed(Keyboard::KEY_C)) {
		saveZone();
		Message::Get().addText(ViewPort::get().getWidth() / 2, ViewPort::get().getHeight() / 2, 1.0f, 0.625f, 0.71f, 1.0f, 15, 3.0f, "Zone saved ...");
	}
}

void Editor::printShortText(const CharacterSet& characterSet, const std::string &printText, int left, int width, int bottom, int height){
	int curY = bottom + height - characterSet.lineHeight;
	const int lineHeight = characterSet.lineHeight * 1.5f;
	const int bottomMargin = characterSet.lineHeight * 0.5f;
	size_t curStringPos = 0;
	while (curY - bottomMargin > bottom) {
		std::string curLine = "";
		curLine.push_back(printText.at(curStringPos));
		++curStringPos;
		while (curStringPos < printText.size() && printText.at(curStringPos) != '\n') {
			if (printText.at(curStringPos) == '\r'){
				++curStringPos;
				continue;
			}

			curLine.push_back(printText.at(curStringPos));
			if (characterSet.getWidth(curLine) > width){
				curLine.erase(curLine.size() - 1, 1);
				break;
			}
			++curStringPos;
		}
		// skip until end of line
		while (curStringPos < printText.size() && printText.at(curStringPos) != '\n') {
			++curStringPos;
		}
		++curStringPos;

		// print current line
		Fontrenderer::get().bindTexture(characterSet);
		Fontrenderer::get().drawText(characterSet, left, curY, curLine, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
		curY -= lineHeight;

		if (curStringPos >= printText.size())
			break;
	}
}

void Editor::render(unsigned int &frameBuffer) {
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	
	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);

	newZone->drawZoneBatched();
		
	Batchrenderer::get().bindTexture(m_textureAtlas, true);
		
	if (m_selectedTileSet == Enums::TileClassificationType::COLLISION) {

		// Collison boxes
		for (unsigned int x = 0; x < newZone->m_collisionMap.size(); x++) {
			TextureManager::DrawTextureBatched(m_interfacetexture[4], newZone->m_collisionMap[x].x, newZone->m_collisionMap[x].y, newZone->m_collisionMap[x].w, newZone->m_collisionMap[x].h, m_selectedObjectId == (signed int)x ? Vector4f(0.9f, 0.2f, 0.8f, 0.8f) : Vector4f(0.7f, 0.1f, 0.6f, 0.8f));
		}

		// Interaction Regions
		std::vector<InteractionRegion>& zoneInteractionRegions = newZone->getInteractionRegions();
		for (size_t curInteractionRegionNr = 0; curInteractionRegionNr < zoneInteractionRegions.size(); ++curInteractionRegionNr){
			InteractionRegion& curInteractionRegion = zoneInteractionRegions[curInteractionRegionNr];
			int left, bottom, width, height;
			curInteractionRegion.getPosition(left, bottom, width, height);
			if (!TextureManager::IsRectOnScreen(left - 4, width + 8, bottom - 4, height + 8)){
				continue;
			}

			// draw border around the region
			TextureManager::DrawTextureBatched(m_interfacetexture[4], left, bottom, width, height, Vector4f(0.0f, 0.8f, 0.0f, 0.6f));
			
			// draw region
			if (width > 8 && height > 8){
				TextureManager::DrawTextureBatched(m_interfacetexture[4], left + 4, bottom + 4, width - 8, height - 8, Vector4f(0.0f, 0.3f, 0.0f, 0.6f));
			}
			Batchrenderer::get().drawBuffer(true);

			// draw text for region
			int tinyFontHeight = Globals::fontManager.get("verdana_5").lineHeight;
			if (width > 28 && height > tinyFontHeight * 3 + 8){

				std::string curEnterText = curInteractionRegion.getOnEnterText();
				std::string curLeaveText = curInteractionRegion.getOnLeaveText();
				bool printEnterAndLeaveText = (curEnterText.size() > 0 && curLeaveText.size() > 0 && height > tinyFontHeight * 6);
				if (curEnterText.size() > 0){

					std::string printText = std::string("Enter:\n").append(curEnterText);
					int printHeight = height - 8;
					int printBottom = bottom + 4;
					if (printEnterAndLeaveText){

						printBottom = printBottom - 4 + height - height / 2;
						printHeight = (height - 8) / 2;
					}
					printShortText(Globals::fontManager.get("verdana_5"), printText, left + 4, width - 8, printBottom, printHeight);
				}

				if (curLeaveText.size() > 0){

					std::string printText = std::string("Leave:\n").append(curLeaveText);
					int printHeight = height - 8;
					int printBottom = bottom + 4;
					if (printEnterAndLeaveText){

						printHeight = (height - 8) / 2;
					}
					printShortText(Globals::fontManager.get("verdana_5"), printText, left + 4, width - 8, printBottom, printHeight);
				}
			}
		}

		//  Wander radius
		std::vector<Npc*> npcs = newZone->getNPCs();
		for (size_t curNPCNr = 0; curNPCNr < npcs.size(); ++curNPCNr) {

			Npc* curNPC = npcs[curNPCNr];
			int wanderRadius = curNPC->getWanderRadius();
			double rootX = curNPC->x_spawn_pos + curNPC->getWidth() / 2;
			double rootY = curNPC->y_spawn_pos + curNPC->getHeight() / 2;
			double collisionRadius = wanderRadius + 0.5*sqrt(curNPC->getWidth()*curNPC->getWidth() + curNPC->getHeight()*curNPC->getHeight());

			TextureManager::DrawTextureBatched(m_interfacetexture[5], rootX - collisionRadius, rootY - collisionRadius, 2 * collisionRadius, 2 * collisionRadius, Vector4f(0.0f, 0.0f, 0.5f, 0.4f));
		}

		Batchrenderer::get().drawBuffer(true);
	}

	TextureManager::DrawTextureBatched(m_interfacetexture[0], m_originalFocus[0], m_originalFocus[1] + ViewPort::get().getHeight() - 100, ViewPort::get().getWidth(), 100.0f, false, false);
	TextureManager::DrawTextureBatched(m_interfacetexture[0], m_originalFocus[0], m_originalFocus[1], ViewPort::get().getWidth(), 100.0f, false, false);
	TextureManager::DrawTextureBatched(m_interfacetexture[1], m_originalFocus[0] + ViewPort::get().getWidth() * 0.5f - 5.0f, m_originalFocus[1] + ViewPort::get().getHeight() - 65, 50.0f, 50.0f, false, false);
	Batchrenderer::get().drawBuffer(false);

	if (m_selectedObjectId >= 0) { // we have selected an object to edit it's properties, show the edit-screen.
		switch (m_selectedTileSet) {
			case Enums::TileClassificationType::ENVIRONMENT:
				drawEditFrame(&(newZone->m_environmentMap[m_selectedObjectId]));
				break;
			case Enums::TileClassificationType::SHADOW:
				drawEditFrame(&(newZone->m_shadowMap[m_selectedObjectId]));
				break;
		}		
	}

	if (m_selectedTileSet == Enums::TileClassificationType::NPC) {
		Batchrenderer::get().bindTexture(TextureManager::GetTextureAtlas("Wolf"), true);

		for (size_t curNPC = 0; curNPC < editorNPCs.size(); curNPC++) {
			const TextureRect& rect = editorNPCs.at(curNPC).second.getTileSet(Enums::ActivityType::Walking, Enums::Direction::S).getAllTiles()[0].textureRect;
			TextureManager::DrawTextureBatched(rect, m_originalFocus[0] + ViewPort::get().getWidth() * 0.5f + (curNPC * 50) + (m_tileposOffset * 50) - 48 + 20, m_originalFocus[1] + ViewPort::get().getHeight() - 40 - 48, rect.width, rect.height, false, false);
		}

	}else {

		Batchrenderer::get().bindTexture(TextureManager::GetTextureAtlas(newZone->getName()), true);
		const std::vector<Tile>& curTiles = EditorInterface::getTileSet(m_selectedTileSet).getAllTiles();


		for (m_tilepos = 0; m_tilepos < curTiles.size(); ++m_tilepos) {
			const Tile& curTile = curTiles[m_tilepos];
			TextureManager::DrawTextureBatched(curTile.textureRect, m_originalFocus[0] + ViewPort::get().getWidth() * 0.5f + (m_tilepos * 50) + (m_tileposOffset * 50), m_originalFocus[1] + ViewPort::get().getHeight() - 60, 40.0f, 40.0f, false, false);
		}
	}
	Batchrenderer::get().drawBuffer(false);

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

	int fontHeight = Globals::fontManager.get("verdana_9").lineHeight;
	Fontrenderer::get().bindTexture(Globals::fontManager.get("verdana_9"));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 90 - fontHeight, "[ Scoll Up/Down ]  Select previous/next object", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 80 - fontHeight, "[ F1 ]  Next set of objects", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 70 - fontHeight, "[ DEL ]  Delete object at mouse position", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 60 - fontHeight, "[ ENTER ]  Place object at mouse position", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 50 - fontHeight, "[ C ]  Saves the changes into zone1-files", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 40 - fontHeight, "[ O ]  Load a different zone (not yet implemented)", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 30 - fontHeight, "[ L ]  Exit the editor", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	//Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + 20 - fontHeight, "//Press the left mouse button near the sides to scroll around ;-)", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + ViewPort::get().getHeight() - 20, "x: " + Fontrenderer::get().FloatToString(ViewPort::get().getCursorPos()[0], 0) + ", y: " + Fontrenderer::get().FloatToString(ViewPort::get().getCursorPos()[1], 0));
	Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + 10, m_originalFocus[1] + ViewPort::get().getHeight() - 60, adjacencyModeEnabled ? "Adjacency Mode: enabled" : "Adjacency Mode: disabled");
	
	if (m_selectedObjectId < 0) {
		Fontrenderer::get().drawBuffer();
	}else {
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 90 - fontHeight, "[ UP, DOWN, LEFT, RIGHT ]  Move the object",Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 80 - fontHeight, "[ Left Shift + UP, DOWN, LEFT, RIGHT ]  Change scale of object", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 70 - fontHeight, "[ . ]  Increase transparency", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 60 - fontHeight, "[ , ]  Decrease transparency", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 50 - fontHeight, "[ 1/2/3 ]  Increase color RED/GREEN/BLUE", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 40 - fontHeight, "[ Left Shift + 1/2/3 ]  Decrease color RED/GREEN/BLUE)", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 30 - fontHeight, "[ B/N ] Increase / decrease Z-position", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_9"), m_originalFocus[0] + ViewPort::get().getWidth() - 500, m_originalFocus[1] + 20 - fontHeight, "[ M ] Toggle Adjacency Modus", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		Fontrenderer::get().drawBuffer();
	}

	Fontrenderer::get().unbindTexture();
	Batchrenderer::get().unbindTexture(true);
	
	glDisable(GL_BLEND);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Editor::drawEditFrame(EnvironmentMap* editobject) {

	// Highlight the currently selected tile
	TextureManager::DrawTextureBatched(m_interfacetexture[3], editobject->x_pos, editobject->y_pos, editobject->width, editobject->height, Vector4f(1.0f, 1.0f, 1.0f, 0.2f), false);
	Batchrenderer::get().drawBuffer(true);

	if (adjacencyModeEnabled) {
		Batchrenderer::get().bindTexture(TextureManager::GetTextureAtlas(newZone->getName()), true);
		for (size_t curDir = 0; curDir <= Enums::AdjacencyType::BOTTOM; ++curDir) {

			std::vector<Tile>& curDirectionAdjacencies = curAdjacentTiles[curDir];
			std::vector<std::array<int, 2>>& curDirectionAdjacencyOffsets = curAdjacencyOffsets[curDir];
			if (curDirectionAdjacencies.size() > 0) {

				Tile adjacencyProposal = curDirectionAdjacencies[curDirectionAdjacencySelection[curDir]];

				// draw the adjacent tile
				int drawX = editobject->x_pos + curDirectionAdjacencyOffsets[curDirectionAdjacencySelection[curDir]][0];
				int drawY = editobject->y_pos + curDirectionAdjacencyOffsets[curDirectionAdjacencySelection[curDir]][1];
				switch (curDir) {
					case Enums::AdjacencyType::RIGHT:
						drawX += editobject->tile.textureRect.width;
						break;
					case Enums::AdjacencyType::LEFT:
						drawX -= adjacencyProposal.textureRect.width;
						break;
					case Enums::AdjacencyType::TOP:
						drawY += editobject->tile.textureRect.height;
						break;
					case Enums::AdjacencyType::BOTTOM:
						drawY -= adjacencyProposal.textureRect.height;
						break;
				}

				int drawW = adjacencyProposal.textureRect.width;
				int drawH = adjacencyProposal.textureRect.height;
				

				bool mouseInAdjacencyRect = TextureManager::CheckPointInRect(ViewPort::get().getCursorPos()[0] + ViewPort::get().getPosition()[0], ViewPort::get().getCursorPos()[1] + ViewPort::get().getPosition()[1], drawX, drawW, drawY, drawH);	
				TextureManager::DrawTextureBatched(adjacencyProposal.textureRect, drawX, drawY, drawW, drawH, mouseInAdjacencyRect ? Vector4f(1.0f, 0.8f, 0.8f, 1.0f) : Vector4f(1.0f, 0.8f, 0.8f, 0.5f), false, true);
			}
		}
		Batchrenderer::get().drawBuffer(true);
	}else {
		TextureManager::DrawTextureBatched(m_interfacetexture[3], m_currentFocus[0] + 50.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 200.0f, 350.0f, 200.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		Batchrenderer::get().drawBuffer(true);
		Batchrenderer::get().bindTexture(TextureManager::GetTextureAtlas(newZone->getName()), true);
		TextureManager::DrawTextureBatched(editobject->tile.textureRect, m_currentFocus[0] + 55.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - editobject->tile.textureRect.height - 5.0f, Vector4f(editobject->red, editobject->green, editobject->blue, editobject->transparency), false);
		Batchrenderer::get().drawBuffer(true);

		int fontHeight = Globals::fontManager.get("verdana_10").lineHeight;
		Fontrenderer::get().bindTexture(Globals::fontManager.get("verdana_10"));
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_10"), m_currentFocus[0] + 242.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 10.0f - fontHeight, "Transparency: " + Fontrenderer::get().FloatToString(editobject->transparency, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_10"), m_currentFocus[0] + 242.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 22.0f - fontHeight, "Red: " + Fontrenderer::get().FloatToString(editobject->red, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_10"), m_currentFocus[0] + 242.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 34.0f - fontHeight, "Green: " + Fontrenderer::get().FloatToString(editobject->green, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_10"), m_currentFocus[0] + 242.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 46.0f - fontHeight, "Blue: " + Fontrenderer::get().FloatToString(editobject->blue, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_10"), m_currentFocus[0] + 242.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 58.0f - fontHeight, "Width: " + Fontrenderer::get().FloatToString(editobject->width, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_10"), m_currentFocus[0] + 242.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 70.0f - fontHeight, "Height: " + Fontrenderer::get().FloatToString(editobject->height, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::get().addText(Globals::fontManager.get("verdana_10"), m_currentFocus[0] + 242.0f, m_currentFocus[1] + ViewPort::get().getHeight() * 0.5f - 82.0f - fontHeight, "Z Position: " + Fontrenderer::get().FloatToString(editobject->z_pos, 0), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::get().drawBuffer(true);
	}
}

void Editor::initTextures() {

	TextureAtlasCreator::get().init(1024, 1024);

	TextureManager::Loadimage("res/background_editor.tga", 0, m_interfacetexture);
	TextureManager::Loadimage("res/current_tile_backdrop.tga", 1, m_interfacetexture);
	TextureManager::Loadimage("res/tile.tga", 2, m_interfacetexture);
	TextureManager::Loadimage("res/edit_backdrop.png", 3, m_interfacetexture);
	TextureManager::Loadimage("res/tile_solid.tga", 4, m_interfacetexture);
	TextureManager::Loadimage("res/circle.tga", 5, m_interfacetexture);
	m_textureAtlas = TextureAtlasCreator::get().getAtlas();

	//smoothing out the circle
	m_interfacetexture[4].textureWidth = m_interfacetexture[4].textureWidth - (1.0f / 1024.0f);
	m_interfacetexture[4].textureHeight = m_interfacetexture[4].textureHeight - (1.0f / 1024.0f);

	m_interfacetexture[5].textureOffsetX = m_interfacetexture[5].textureOffsetX + (0.3f / 1024.0f);
	m_interfacetexture[5].textureOffsetY = m_interfacetexture[5].textureOffsetY + (0.3f / 1024.0f);

	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureAtlas);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Editor::loadNPCs(){
	
	std::unordered_map< std::string, CharacterType>::const_iterator curNPC;

	editorNPCs.clear();
	for (curNPC = CharacterTypeManager::Get().getCharacterTypes().begin(); curNPC != CharacterTypeManager::Get().getCharacterTypes().end(); curNPC++) {
		editorNPCs.push_back(std::pair<std::string, CharacterType>((*curNPC).first, (*curNPC).second));
	}
}

void Editor::incTilepos(){

	switch (m_selectedTileSet) {
		case Enums::TileClassificationType::FLOOR: case Enums::TileClassificationType::ENVIRONMENT: case Enums::TileClassificationType::SHADOW: {
			TileSet& curTileSet = EditorInterface::getTileSet(m_selectedTileSet);

			if (m_currentTilepos + 1 < curTileSet.numberOfTiles()) {
				m_currentTilepos++;
				m_tileposOffset--;
			}
			
		}
		break;
		case Enums::TileClassificationType::NPC: {
			if (m_currentTilepos + 1 < editorNPCs.size()) {
				m_currentTilepos++;
				m_tileposOffset--;
			}
		}
		break;
	}
}

void Editor::decTilepos(){
	if (m_currentTilepos > 0){
		m_currentTilepos--;
		m_tileposOffset++;
	}
}

void Editor::updateAdjacencyList(){
	if (!adjacencyModeEnabled || m_selectedObjectId < 0)
		return;

	switch (m_selectedTileSet){
		case Enums::TileClassificationType::ENVIRONMENT:
			EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getAllAdjacentTiles(newZone->m_environmentMap[m_selectedObjectId].tile, curAdjacentTiles, curAdjacencyOffsets);
			for (size_t curDirection = 0; curDirection<4; ++curDirection) {

				if (curDirectionAdjacencySelection[curDirection] >= curAdjacentTiles[curDirection].size()) {
					curDirectionAdjacencySelection[curDirection] = curAdjacentTiles[curDirection].size() - 1;
				}

				if (curDirectionAdjacencySelection[curDirection] < 0) {
					curDirectionAdjacencySelection[curDirection] = 0;
				}
			}
		break;
		case Enums::TileClassificationType::SHADOW:
			EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getAllAdjacentTiles(newZone->m_shadowMap[m_selectedObjectId].tile, curAdjacentTiles, curAdjacencyOffsets);

			for (size_t curDirection = 0; curDirection<4; ++curDirection) {

				if (curDirectionAdjacencySelection[curDirection] >= curAdjacentTiles[curDirection].size()) {
					curDirectionAdjacencySelection[curDirection] = curAdjacentTiles[curDirection].size() - 1;
				}

				if (curDirectionAdjacencySelection[curDirection] < 0) {
					curDirectionAdjacencySelection[curDirection] = 0;
				}
			}
		break;
	}
}

void Editor::placeAdjacentTile() {

	EnvironmentMap* editobject = NULL;
	if (m_selectedObjectId >= 0) {
		switch (m_selectedTileSet) {
		case Enums::TileClassificationType::ENVIRONMENT:
			editobject = &(newZone->m_environmentMap[m_selectedObjectId]);
			break;
		case Enums::TileClassificationType::SHADOW:
			editobject = &(newZone->m_shadowMap[m_selectedObjectId]);
			break;
		}
	}

	if (editobject == NULL) {
		return;
	}

	if (adjacencyModeEnabled) {
		for (size_t curDirection = 0; curDirection <= Enums::AdjacencyType::BOTTOM; ++curDirection) {

			std::vector<Tile> &curDirectionAdjacencies = curAdjacentTiles[curDirection];
			std::vector<std::array<int, 2>> &curDirectionAdjacencyOffsets = curAdjacencyOffsets[curDirection];
			if (curDirectionAdjacencies.size() > 0) {

				Tile adjacencyProposal = curDirectionAdjacencies[curDirectionAdjacencySelection[curDirection]];

				// draw the adjacent tile
				int drawX = editobject->x_pos + curDirectionAdjacencyOffsets[curDirectionAdjacencySelection[curDirection]][0];
				int drawY = editobject->y_pos + curDirectionAdjacencyOffsets[curDirectionAdjacencySelection[curDirection]][1];
				switch (curDirection) {
				case Enums::AdjacencyType::RIGHT:
					drawX += editobject->tile.textureRect.width;
					break;
				case Enums::AdjacencyType::LEFT:
					drawX -= adjacencyProposal.textureRect.width;
					break;
				case Enums::AdjacencyType::TOP:
					drawY += editobject->tile.textureRect.height;
					break;
				case Enums::AdjacencyType::BOTTOM:
					drawY -= adjacencyProposal.textureRect.height;
					break;
				}
				
				int drawW = adjacencyProposal.textureRect.width;
				int drawH = adjacencyProposal.textureRect.height;
				bool mouseInAdjacencyRect = TextureManager::CheckPointInRect(ViewPort::get().getCursorPos()[0] + ViewPort::get().getPosition()[0], ViewPort::get().getCursorPos()[1] + ViewPort::get().getPosition()[1], drawX, drawW, drawY, drawH);
				

				if (mouseInAdjacencyRect) {
					int objectId = newZone->locateEnvironment(static_cast<int>(m_editorFocus[0] + ViewPort::get().getCursorPos()[0]), static_cast<int>(m_editorFocus[1] + ViewPort::get().getCursorPos()[1]));
					if (objectId < 0) {
						newZone->addEnvironment(drawX, drawY, adjacencyProposal, false /* not centered on pos */);
					}else {
						newZone->replaceEnvironment(drawX, drawY, adjacencyProposal, false, objectId);
					}
				}
			}
		}
	}
}

void Editor::applyAdjacencyModification(int modification) {

	EnvironmentMap* editobject = NULL;
	if (m_selectedObjectId >= 0) {
		switch (m_selectedTileSet) {
		case Enums::TileClassificationType::ENVIRONMENT:
			editobject = &(newZone->m_environmentMap[m_selectedObjectId]);
			break;
		case Enums::TileClassificationType::SHADOW:
			editobject = &(newZone->m_shadowMap[m_selectedObjectId]);
			break;
		}
	}

	if (editobject == NULL) {
		return;
	}

	if (adjacencyModeEnabled) {

		for (size_t curDirection = 0; curDirection <= Enums::AdjacencyType::BOTTOM; ++curDirection) {

			std::vector<Tile> &curDirectionAdjacencies = curAdjacentTiles[curDirection];
			std::vector<std::array<int, 2>> &curDirectionAdjacencyOffsets = curAdjacencyOffsets[curDirection];

			if (curDirectionAdjacencies.size() > 0){
				Tile adjacencyProposal = curDirectionAdjacencies[curDirectionAdjacencySelection[curDirection]];

				// draw the adjacent tile
				int drawX = editobject->x_pos + curDirectionAdjacencyOffsets[curDirectionAdjacencySelection[curDirection]][0];
				int drawY = editobject->y_pos + curDirectionAdjacencyOffsets[curDirectionAdjacencySelection[curDirection]][1];
				switch (curDirection) {
				case Enums::AdjacencyType::RIGHT:
					drawX += editobject->tile.textureRect.width;
					break;
				case Enums::AdjacencyType::LEFT:
					drawX -= adjacencyProposal.textureRect.width;
					break;
				case Enums::AdjacencyType::TOP:
					drawY += editobject->tile.textureRect.height;
					break;
				case Enums::AdjacencyType::BOTTOM:
					drawY -= adjacencyProposal.textureRect.height;
					break;
				}

				int drawW = adjacencyProposal.textureRect.width;
				int drawH = adjacencyProposal.textureRect.height;

				bool mouseInAdjacencyRect = TextureManager::CheckPointInRect(ViewPort::get().getCursorPos()[0] + ViewPort::get().getPosition()[0], ViewPort::get().getCursorPos()[1] + ViewPort::get().getPosition()[1], drawX, drawW, drawY, drawH);


				if (mouseInAdjacencyRect) {
					if ((modification > 0
						&& static_cast<unsigned int>(curDirectionAdjacencies.size()) > curDirectionAdjacencySelection[curDirection] + modification)
						|| (modification < 0 && curDirectionAdjacencySelection[curDirection] >= -modification)) {
						curDirectionAdjacencySelection[curDirection] += modification;
					}
				}
			}
		}
	}
}

void Editor::saveZone() {
	std::string zoneName = ZoneManager::Get().getCurrentZone()->getZoneName();
	// save the ground
	std::string groundTileFileName = zoneName;
	groundTileFileName.append(".ground.lua");

	std::ofstream ofs(groundTileFileName.c_str());
	for (size_t x = 0; x < newZone->m_tileMap.size(); x++){
		TileMap &curTile = newZone->m_tileMap[x];
		ofs << "EditorInterface.addGroundTile("<< curTile.x_pos << ", " << curTile.y_pos << ", "<< curTile.tile.tileId << ");" << std::endl;

		
	}
	ofs.close();

	std::sort(newZone->m_environmentMap.begin(), newZone->m_environmentMap.end());

	std::string environmentTileFileName = zoneName;
	environmentTileFileName.append(".environment.lua");

	ofs.open(environmentTileFileName.c_str());
	for (size_t x = 0; x < newZone->m_environmentMap.size(); x++) {
		EnvironmentMap &curEnv = newZone->m_environmentMap[x];
		ofs << "EditorInterface.addEnvironment( " << curEnv.x_pos << ", " << curEnv.y_pos << ", " << curEnv.z_pos << ", " << curEnv.tile.tileId << ");" << std::endl;
			
		if (curEnv.transparency != 1 ||
			curEnv.red != 1 ||
			curEnv.green != 1 ||
			curEnv.blue != 1) {
			ofs << "EditorInterface.adjustLastRGBA( " << curEnv.red << ", " << curEnv.green << ", " << curEnv.blue << ", " << curEnv.transparency << " );" << std::endl;
		}

		if (static_cast<int>(curEnv.width) != curEnv.tile.textureRect.width || static_cast<int>(curEnv.height) != curEnv.tile.textureRect.height) {
			ofs << "EditorInterface.adjustLastSize(" << curEnv.width<< "," << curEnv.height << ");" << std::endl;
		}
	}
	ofs.close();

	std::string shadowTileFileName = zoneName;
	shadowTileFileName.append(".shadow.lua");

	ofs.open(shadowTileFileName.c_str());
	for (size_t x = 0; x < newZone->m_shadowMap.size(); x++){
		EnvironmentMap& curEnv = newZone->m_shadowMap[x];
		ofs << "EditorInterface.addEnvironment(" << curEnv.x_pos << ", " << curEnv.y_pos << ", " << curEnv.z_pos << ", " << curEnv.tile.tileId << ");" << std::endl;
		if (curEnv.transparency != 1 ||
			curEnv.red != 1 ||
			curEnv.green != 1 ||
			curEnv.blue != 1)
		{
			ofs << "EditorInterface.adjustLastRGBA( " << curEnv.red << ", " << curEnv.green << ", " << curEnv.blue << ", " << curEnv.transparency << " );" << std::endl;
		}

		if (static_cast<int>(curEnv.width) != curEnv.tile.textureRect.width || static_cast<int>(curEnv.height) != curEnv.tile.textureRect.height) {
			ofs << "EditorInterface.adjustLastSize(" << curEnv.width << "," << curEnv.height << ");" << std::endl;
		}
	}
	ofs.close();

	std::string collisionTileFileName = zoneName;
	collisionTileFileName.append(".collision.lua");

	ofs.open(collisionTileFileName.c_str());
	for (unsigned int x = 0; x < newZone->m_collisionMap.size(); x++) {
		CollisionRect& curCollision = newZone->m_collisionMap[x];
		ofs << "EditorInterface.addCollisionRect("<< curCollision.x << ", " << curCollision.y << ", "<< curCollision.w << ", " << curCollision.h << ");" << std::endl;
	}
	ofs.close();

	// save the NPCs
	/* NOTE! We only save NPCs that doesn't have any InteractionType added to
	them or an dieEventHandler. So all interaction NPCs will need to be
	modified manually. */
	//std::string spawnpointFileName = zoneName;
	//spawnpointFileName.append(".spawnpoints.lua");

	//ofs.open(spawnpointFileName.c_str());
	//std::vector<Npc*> currentNPCs = newZone->getNPCs();
	//for (size_t curNPC = 0; curNPC < currentNPCs.size(); curNPC++) {
	//	if (newZone->findInteractionPointForCharacter(dynamic_cast<Character*>(currentNPCs[curNPC])) == false /*&& currentNPCs[curNPC]->hasOnDieEventHandler() == false*/) {
	//		ofs << currentNPCs[curNPC]->getLuaEditorSaveText();
	//	}
	//}
	//ofs.close();
}