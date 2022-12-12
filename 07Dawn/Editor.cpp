#include "Editor.h"
#include "Game.h"

std::vector<TextureRect> Editor::TextureRects;
bool Editor::s_init = false;

Editor::Editor(StateMachine& machine) : State(machine, CurrentState::EDITOR) {
	Mouse::SetCursorIcon("res/cursors/pointer.cur");

	if (!s_init && !Game::s_init) {
		LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");
		Player::Get().setCharacterType("player_w");
		Player::Get().setClass(Enums::CharacterClass::Liche);

		LuaFunctions::executeLuaFile("res/_lua/spells_l.lua");
		LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");
		LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");
	}

	if (!s_init) {
		Init();
		s_init = true;
	}

	Game::s_init ? DawnInterface::enterZone() : DawnInterface::enterZone("res/_lua/zone1", 512, 400);
	
	LuaFunctions::executeLuaFile("res/_lua/tileAdjacency.lua");
	loadNPCs();

	newZone = ZoneManager::Get().getCurrentZone();
	m_editorFocus = ViewPort::Get().getBottomLeft();
	
	m_tileposOffset = 0;
	m_tilepos = 0;
	m_currentTilepos = 0;
	m_selectedTileSet = Enums::TileClassificationType::FLOOR;
	adjacencyModeEnabled = false;

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("editor"), true, 1);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("editor"), true, 2);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("spells"), true, 3);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("player"), true, 4);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("mobs"), true, 5);
	TextureManager::BindTexture(ZoneManager::Get().getCurrentZone()->getTetureAtlas(), true, 6);

}

Editor::~Editor() {}

void Editor::fixedUpdate() {
	
}

void Editor::update() {
	//newZone->update(m_dt);

	ViewPort::Get().update(m_dt);
	m_editorFocus = ViewPort::Get().getBottomLeft();

	Mouse &mouse = Mouse::instance();
	Keyboard &keyboard = Keyboard::instance();

	if (mouse.buttonPressed(Mouse::BUTTON_LEFT)) {
		m_currentFocus = m_editorFocus;
		int previousSelectedObjectId = m_selectedObjectId;

		switch (m_selectedTileSet) {
			case Enums::TileClassificationType::ENVIRONMENT:
				m_selectedObjectId = newZone->locateEnvironment(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
				break;
			case Enums::TileClassificationType::SHADOW: // shadows
				m_selectedObjectId = newZone->locateShadow(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
				break;
			case Enums::TileClassificationType::COLLISION:
				m_selectedObjectId = newZone->locateCollisionbox(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
				break;
			case Enums::TileClassificationType::NPC: // NPCs
				m_selectedObjectId = newZone->locateNPC(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
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
				const Tile& currentTile = newZone->getTileSet(m_selectedTileSet).getAllTiles()[m_currentTilepos];
				newZone->replaceTile(newZone->locateTile(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY()), currentTile);
				break;
			}case Enums::TileClassificationType::ENVIRONMENT: {
				const Tile& currentTile = newZone->getTileSet(m_selectedTileSet).getAllTiles()[m_currentTilepos];
				newZone->addEnvironment(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY(), currentTile, true /* centered on pos */);
				break;
			}case Enums::TileClassificationType::SHADOW: {
				const Tile& currentTile = newZone->getTileSet(m_selectedTileSet).getAllTiles()[m_currentTilepos];
				newZone->addShadow(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY(), currentTile);
				break;
			}case Enums::TileClassificationType::COLLISION: {
				newZone->addCollisionbox(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
				break;
			}case Enums::TileClassificationType::NPC: {
				DawnInterface::addMobSpawnPoint(editorNPCs[m_currentTilepos].first, "", ViewPort::Get().getCursorPosX() - 48, ViewPort::Get().getCursorPosY() - 48, 180, 1, Enums::Attitude::HOSTILE);
				break;
			}
		}		
	}

	if (keyboard.keyPressed(Keyboard::KEY_DELETE)) {

		switch (m_selectedTileSet) {
		case Enums::TileClassificationType::FLOOR: // tiles
			newZone->deleteTile(newZone->locateTile(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY()));
			break;
		case Enums::TileClassificationType::ENVIRONMENT: // environment
			if (newZone->deleteEnvironment(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY()) == 0)
				m_selectedObjectId = -1;
			break;
		case Enums::TileClassificationType::SHADOW: // shadows
			if (newZone->deleteShadow(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY()) == 0)
				m_selectedObjectId = -1;
			break;
		case Enums::TileClassificationType::COLLISION: // collisionboxes
			if (newZone->deleteCollisionbox(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY()) == 0)
				m_selectedObjectId = -1;
			break;
		case Enums::TileClassificationType::NPC: // NPCs
			if (newZone->deleteNPC(ViewPort::Get().getCursorPosY(), ViewPort::Get().getCursorPosY()) == 0){
				newZone->cleanupNPCList();
				m_selectedObjectId = -1;
			}
			break;
		}	
	}

	if (m_selectedObjectId >= 0) {
		switch (m_selectedTileSet ) {	
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
		Message::Get().addText(ViewPort::Get().getWidth() / 2, ViewPort::Get().getHeight() / 2, 1.0f, 0.625f, 0.71f, 1.0f, 15, 3.0f, "Zone saved ...");
	}

	if (keyboard.keyPressed(Keyboard::KEY_L)) {
		m_machine.addStateAtTop(new Game(m_machine));
	}
}

void Editor::printShortText(const CharacterSet& characterSet, const std::string &printText, int left, int width, int bottom, int height){
	int curY = bottom + height - characterSet.lineHeight;
	int lineHeight = characterSet.lineHeight / 2 + characterSet.lineHeight;
	int bottomMargin = characterSet.lineHeight / 2;
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
		Fontrenderer::Get().addText(characterSet, left, curY, curLine, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), true);
		curY -= lineHeight;

		if (curStringPos >= printText.size())
			break;
	}
}

void Editor::render() {

	glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	newZone->drawZoneBatched();
	InteractionPoint::Draw();
	Zone::DrawNpcs();
	Player::Get().draw();
		
	if (m_selectedTileSet == Enums::TileClassificationType::COLLISION) {

		// Collison boxes
		for (unsigned int x = 0; x < newZone->m_collisionMap.size(); x++) {
			TextureManager::DrawTextureBatched(TextureRects[4], newZone->m_collisionMap[x].x, newZone->m_collisionMap[x].y, static_cast<float>(newZone->m_collisionMap[x].w), static_cast<float>(newZone->m_collisionMap[x].h), m_selectedObjectId == static_cast<int>(x) ? Vector4f(0.9f, 0.2f, 0.8f, 0.8f) : Vector4f(0.7f, 0.1f, 0.6f, 0.8f));
		}

		// Interaction Regions
		std::vector<InteractionRegion*>& zoneInteractionRegions = newZone->getInteractionRegions();
		for (size_t curInteractionRegionNr = 0; curInteractionRegionNr < zoneInteractionRegions.size(); ++curInteractionRegionNr){
			InteractionRegion* curInteractionRegion = zoneInteractionRegions[curInteractionRegionNr];
			int left, bottom, width, height;
			curInteractionRegion->getPosition(left, bottom, width, height);
			if (!TextureManager::IsRectOnScreen(left - 4, width + 8, bottom - 4, height + 8)){
				continue;
			}

			// draw border around the region
			TextureManager::DrawTextureBatched(TextureRects[4], left, bottom, static_cast<float>(width), static_cast<float>(height), Vector4f(0.0f, 0.8f, 0.0f, 0.6f));
			
			// draw region
			if (width > 8 && height > 8){
				TextureManager::DrawTextureBatched(TextureRects[4], left + 4, bottom + 4, static_cast<float>(width - 8), static_cast<float>(height - 8), Vector4f(0.0f, 0.3f, 0.0f, 0.6f));
			}

			// draw text for region
			int tinyFontHeight = Globals::fontManager.get("verdana_5").lineHeight;
			if (width > 28 && height > tinyFontHeight * 3 + 8){

				std::string curEnterText = curInteractionRegion->getOnEnterText();
				std::string curLeaveText = curInteractionRegion->getOnLeaveText();
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
			float rootX = curNPC->x_spawn_pos + curNPC->getWidth() * 0.5f;
			float rootY = curNPC->y_spawn_pos + curNPC->getHeight() * 0.5f;
			float collisionRadius = static_cast<float>(wanderRadius) + 0.5f * sqrtf(static_cast<float>(curNPC->getWidth()*curNPC->getWidth() + curNPC->getHeight()*curNPC->getHeight()));

			TextureManager::DrawTextureBatched(TextureRects[5], static_cast<int>(rootX - collisionRadius), static_cast<int>(rootY - collisionRadius), 2.0f * collisionRadius, 2.0f * collisionRadius, Vector4f(0.0f, 0.0f, 0.5f, 0.4f));
		}

	}

	TextureManager::DrawTextureBatched(TextureRects[0], 0,  ViewPort::Get().getHeight() - 100, static_cast<float>(ViewPort::Get().getWidth()), 100.0f, false, false);
	TextureManager::DrawTextureBatched(TextureRects[0], 0, 0, static_cast<float>(ViewPort::Get().getWidth()), 100.0f, false, false);
	TextureManager::DrawTextureBatched(TextureRects[1], ViewPort::Get().getWidth() / 2 - 5, static_cast<int>(ViewPort::Get().getHeight() - 65u), 50.0f, 50.0f, false, false);

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

		for (unsigned int curNPC = 0; curNPC < editorNPCs.size(); curNPC++) {			
			const TextureRect& rect = editorNPCs.at(curNPC).second.getTileSet(Enums::ActivityType::Walking, Enums::Direction::S).getAllTiles()[0].textureRect;
			TextureManager::DrawTextureBatched(rect, ViewPort::Get().getWidth() / 2 + (curNPC * 50) + (m_tileposOffset * 50) - 48 + 20,  ViewPort::Get().getHeight() - 40 - 48, false, false, 5u);
		}
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() / 2 -5, ViewPort::Get().getHeight() - 90, editorNPCs[m_currentTilepos].first);
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() / 2 - 5, ViewPort::Get().getHeight() - 100, "Level:" + std::to_string(editorNPCs[m_currentTilepos].second.getLevel()) + " (" + Character::GetCharacterClassName(editorNPCs[m_currentTilepos].second.getClass()).c_str() + ")");
		
	}else {

		const std::vector<Tile>& curTiles = newZone->getTileSet(m_selectedTileSet).getAllTiles();
		for (m_tilepos = 0; m_tilepos < curTiles.size(); ++m_tilepos) {
			const Tile& curTile = curTiles[m_tilepos];
			TextureManager::DrawTextureBatched(curTile.textureRect, ViewPort::Get().getWidth() / 2 + (m_tilepos * 50) + (m_tileposOffset * 50), ViewPort::Get().getHeight() - 60, 40.0f, 40.0f, false, false, 6u);
		}
	}

	int fontHeight = Globals::fontManager.get("verdana_9").lineHeight;
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, 90 - fontHeight, "[ Scoll Up/Down ]  Select previous/next object", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, 80 - fontHeight, "[ F1 ]  Next set of objects", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, 70 - fontHeight, "[ DEL ]  Delete object at mouse position", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, 60 - fontHeight, "[ ENTER ]  Place object at mouse position", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, 50 - fontHeight, "[ C ]  Saves the changes into zone1-files", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, 40 - fontHeight, "[ O ]  Load a different zone (not yet implemented)", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10,30 - fontHeight, "[ L ]  Exit the editor", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	//Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, 20 - fontHeight, "//Press the left mouse button near the sides to scroll around ;-)", Vector4f(1.0f, 1.0f, 0.13f, 1.0f));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, ViewPort::Get().getHeight() - 20, "x: " + Fontrenderer::Get().FloatToString(ViewPort::Get().getCursorPos()[0], 0) + ", y: " + Fontrenderer::Get().FloatToString(ViewPort::Get().getCursorPos()[1], 0));
	Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), 10, ViewPort::Get().getHeight() - 60, adjacencyModeEnabled ? "Adjacency Mode: enabled" : "Adjacency Mode: disabled");

	if (m_selectedObjectId >= 0) {
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 90 - fontHeight, "[ UP, DOWN, LEFT, RIGHT ]  Move the object",Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		
		if (m_selectedTileSet != Enums::TileClassificationType::NPC) {
			Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 80 - fontHeight, "[ Left Shift + UP, DOWN, LEFT, RIGHT ]  Change scale of object", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 70 - fontHeight, "[ . ]  Increase transparency", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 60 - fontHeight, "[ , ]  Decrease transparency", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 50 - fontHeight, "[ 1/2/3 ]  Increase color RED/GREEN/BLUE", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 40 - fontHeight, "[ Left Shift + 1/2/3 ]  Decrease color RED/GREEN/BLUE)", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 30 - fontHeight, "[ B/N ] Increase / decrease Z-position", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
			Fontrenderer::Get().addText(Globals::fontManager.get("verdana_9"), ViewPort::Get().getWidth() - 500, 20 - fontHeight, "[ M ] Toggle Adjacency Modus", Vector4f(0.5f, 1.0f, 0.5f, 1.0f));
		}
	}

	Message::Get().draw();
	TextureManager::DrawBuffer();
}

void Editor::drawEditFrame(EnvironmentMap* editobject) {

	// Highlight the currently selected tile
	TextureManager::DrawTextureBatched(TextureRects[3], editobject->x_pos, editobject->y_pos, editobject->width, editobject->height, Vector4f(1.0f, 1.0f, 1.0f, 0.2f), false);

	if (adjacencyModeEnabled) {
		TextureManager::BindTexture(TextureManager::GetTextureAtlas(newZone->getName()), true);
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
				

				bool mouseInAdjacencyRect = TextureManager::CheckPointInRect(static_cast<float>(ViewPort::Get().getCursorPosX()), static_cast<float>(ViewPort::Get().getCursorPosY()), drawX, drawW, drawY, drawH);
				TextureManager::DrawTextureBatched(adjacencyProposal.textureRect, drawX, drawY, mouseInAdjacencyRect ? Vector4f(1.0f, 0.8f, 0.8f, 1.0f) : Vector4f(1.0f, 0.8f, 0.8f, 0.5f), false, true, 6u);
			}
		}

	}else {
		TextureManager::DrawTextureBatched(TextureRects[3], static_cast<int>(m_currentFocus[0]) + 50, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 200, 350.0f, 200.0f, Vector4f(1.0f, 1.0f, 1.0f, 1.0f), false);
		TextureManager::DrawTextureBatched(editobject->tile.textureRect, static_cast<int>(m_currentFocus[0]) + 55, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - editobject->tile.textureRect.height - 5, Vector4f(editobject->red, editobject->green, editobject->blue, editobject->transparency), false, true, 6u);

		int fontHeight = Globals::fontManager.get("verdana_10").lineHeight;
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_10"), static_cast<int>(m_currentFocus[0]) + 242, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 10 - fontHeight, "Transparency: " + Fontrenderer::Get().FloatToString(editobject->transparency, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_10"), static_cast<int>(m_currentFocus[0]) + 242, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 22 - fontHeight, "Red: " + Fontrenderer::Get().FloatToString(editobject->red, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_10"), static_cast<int>(m_currentFocus[0]) + 242, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 34 - fontHeight, "Green: " + Fontrenderer::Get().FloatToString(editobject->green, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_10"), static_cast<int>(m_currentFocus[0]) + 242, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 46 - fontHeight, "Blue: " + Fontrenderer::Get().FloatToString(editobject->blue, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_10"), static_cast<int>(m_currentFocus[0]) + 242, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 58 - fontHeight, "Width: " + Fontrenderer::Get().FloatToString(editobject->width, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_10"), static_cast<int>(m_currentFocus[0]) + 242, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 70 - fontHeight, "Height: " + Fontrenderer::Get().FloatToString(editobject->height, 2), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
		Fontrenderer::Get().addText(Globals::fontManager.get("verdana_10"), static_cast<int>(m_currentFocus[0]) + 242, static_cast<int>(m_currentFocus[1]) + ViewPort::Get().getHeight() / 2 - 82 - fontHeight, "Z Position: " + Fontrenderer::Get().FloatToString(static_cast<float>(editobject->z_pos), 0), Vector4f(0.0f, 0.0f, 0.0f, 1.0f), true);
	}
}

void Editor::loadNPCs(){
	
	std::unordered_map< std::string, CharacterType>::const_iterator curNPC;

	editorNPCs.clear();
	for (curNPC = CharacterTypeManager::Get().getCharacterTypes().begin(); curNPC != CharacterTypeManager::Get().getCharacterTypes().end(); curNPC++) {
		if ((*curNPC).first.compare("player_w") == 0 || (*curNPC).first.compare("player_r") == 0 || (*curNPC).first.compare("player_s") == 0) continue;
		editorNPCs.push_back(std::pair<std::string, CharacterType>((*curNPC).first, (*curNPC).second));
	}
}

void Editor::incTilepos(){

	switch (m_selectedTileSet) {
		case Enums::TileClassificationType::FLOOR: case Enums::TileClassificationType::ENVIRONMENT: case Enums::TileClassificationType::SHADOW: {
			TileSet& curTileSet = newZone->getTileSet(m_selectedTileSet);

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
			for (unsigned int curDirection = 0; curDirection<4; ++curDirection) {

				if (curDirectionAdjacencySelection[curDirection] >= curAdjacentTiles[curDirection].size()) {
					curDirectionAdjacencySelection[curDirection] = static_cast<unsigned int>(curAdjacentTiles[curDirection].size()) - 1;
				}

				if (curDirectionAdjacencySelection[curDirection] < 0) {
					curDirectionAdjacencySelection[curDirection] = 0;
				}
			}
		break;
		case Enums::TileClassificationType::SHADOW:
			EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getAllAdjacentTiles(newZone->m_shadowMap[m_selectedObjectId].tile, curAdjacentTiles, curAdjacencyOffsets);

			for (unsigned int curDirection = 0; curDirection<4; ++curDirection) {

				if (curDirectionAdjacencySelection[curDirection] >= curAdjacentTiles[curDirection].size()) {
					curDirectionAdjacencySelection[curDirection] = static_cast<unsigned int>(curAdjacentTiles[curDirection].size()) - 1;
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
				bool mouseInAdjacencyRect = TextureManager::CheckPointInRect(static_cast<float>(ViewPort::Get().getCursorPosX()), static_cast<float>(ViewPort::Get().getCursorPosY()), drawX, drawW, drawY, drawH);
				

				if (mouseInAdjacencyRect) {
					int objectId = newZone->locateEnvironment(ViewPort::Get().getCursorPosX(), ViewPort::Get().getCursorPosY());
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

				bool mouseInAdjacencyRect = TextureManager::CheckPointInRect(static_cast<float>(ViewPort::Get().getCursorPosX()), static_cast<float>(ViewPort::Get().getCursorPosY()), drawX, drawW, drawY, drawH);


				if (mouseInAdjacencyRect) {
					if ((modification > 0
						&& static_cast<unsigned int>(curDirectionAdjacencies.size()) > curDirectionAdjacencySelection[curDirection] + modification)
						|| (modification < 0 && static_cast<int>(curDirectionAdjacencySelection[curDirection]) >= -modification)) {
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

void Editor::Init() {
	TextureAtlasCreator::Get().init("editor", 1024, 1024);

	TextureManager::Loadimage("res/background_editor.tga", 0, TextureRects);
	TextureManager::Loadimage("res/current_tile_backdrop.tga", 1, TextureRects);
	TextureManager::Loadimage("res/tile.tga", 2, TextureRects);
	TextureManager::Loadimage("res/edit_backdrop.png", 3, TextureRects);
	TextureManager::Loadimage("res/tile_solid.tga", 4, TextureRects);
	TextureManager::Loadimage("res/circle.tga", 5, TextureRects);
	TextureManager::SetTextureAtlas(TextureAtlasCreator::Get().getName(), TextureAtlasCreator::Get().getAtlas());


	//smoothing out the circle
	TextureRects[4].textureWidth = TextureRects[4].textureWidth - (1.0f / 1024.0f);
	TextureRects[4].textureHeight = TextureRects[4].textureHeight - (1.0f / 1024.0f);

	TextureRects[5].textureOffsetX = TextureRects[5].textureOffsetX + (0.3f / 1024.0f);
	TextureRects[5].textureOffsetY = TextureRects[5].textureOffsetY + (0.3f / 1024.0f);

	/*glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureAtlas);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);*/
}