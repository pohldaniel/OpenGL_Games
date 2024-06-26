#include <iostream>
#include "Zone.h"
#include "Magic.h"
#include "Callindirection.h"
#include "Player.h"

Zone::Zone() : groundLoot(&Player::Get()) { 

	m_tileSetManger = new TileSetManager();
}

Zone::~Zone(){	
	//delete m_tileSetManger;
}

void Zone::loadZone() {

	LuaFunctions::executeLuaScript(std::string("DawnInterface.setCurrentZone( \"").append(m_file).append("\");"));
	
	if (!m_mapLoaded) {
		EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).setOffset(EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).getAllTiles().size());
		EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).setOffset(EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getAllTiles().size());
		EditorInterface::getTileSet(Enums::TileClassificationType::SHADOW).setOffset(EditorInterface::getTileSet(Enums::TileClassificationType::SHADOW).getAllTiles().size());

		LuaFunctions::executeLuaScript(std::string("if ( MapData == nil )\nthen\n    MapData={}\nend"));
		LuaFunctions::executeLuaScript(std::string("if ( MapData.DontSave == nil )\nthen\n    MapData.DontSave={}\nend"));

		TextureAtlasCreator::Get().init(m_file);
		LuaFunctions::executeLuaFile(std::string(m_file).append(".tiles_ground.lua"));
		LuaFunctions::executeLuaFile(std::string(m_file).append(".tiles_environment.lua"));
		LuaFunctions::executeLuaFile(std::string(m_file).append(".tiles_shadow.lua"));
		m_textureAtlas = TextureAtlasCreator::Get().getAtlas();
		TextureManager::SetTextureAtlas(m_file, m_textureAtlas);

		const std::vector<Tile>& tilesF = EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).getAllTiles();
		int offset = EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).getOffset();
		int length = EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).getAllTiles().size() - offset;

		m_tileSetManger->getTileSet(Enums::TileClassificationType::FLOOR).setTiles({ tilesF.begin() + offset , tilesF.begin() + offset + length });

		const std::vector<Tile>& tilesE = EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getAllTiles();
		offset = EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getOffset();
		length = EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getAllTiles().size() - offset;
		m_tileSetManger->getTileSet(Enums::TileClassificationType::ENVIRONMENT).setTiles({ tilesE.begin() + offset ,tilesE.begin() + offset + length });

		const std::vector<Tile>& tilesS = EditorInterface::getTileSet(Enums::TileClassificationType::SHADOW).getAllTiles();
		offset = EditorInterface::getTileSet(Enums::TileClassificationType::SHADOW).getOffset();
		length = EditorInterface::getTileSet(Enums::TileClassificationType::SHADOW).getAllTiles().size() - offset;
		m_tileSetManger->getTileSet(Enums::TileClassificationType::SHADOW).setTiles({ tilesS.begin() + offset ,tilesS.begin() + offset + length });

		LuaFunctions::executeLuaFile(std::string(m_file).append(".ground.lua"));
		LuaFunctions::executeLuaFile(std::string(m_file).append(".environment.lua"));
		LuaFunctions::executeLuaFile(std::string(m_file).append(".shadow.lua"));
		LuaFunctions::executeLuaFile(std::string(m_file).append(".collision.lua"));
		LuaFunctions::executeLuaFile(std::string(m_file).append(".tables.lua"));
		LuaFunctions::executeLuaFile(std::string(m_file).append(".init.lua"));
		m_mapLoaded = true;
	}

	if (!m_init) {
		LuaFunctions::executeLuaFile(std::string(m_file).append(".spawnpoints.lua"));		
		m_init = true;
	}

	TextureManager::BindTexture(TextureManager::GetTextureAtlas(m_file), true, 6);
}

unsigned int Zone::getTetureAtlas(){
	return m_textureAtlas;
}

std::vector<TileMap>& Zone::getTileMap() {
	return m_tileMap;
}

std::vector<EnvironmentMap>& Zone::getEnvironmentMap() {
	return m_environmentMap;
}

std::vector<EnvironmentMap>& Zone::getShadowMap() {
	return m_shadowMap;
}

std::vector<CollisionRect>& Zone::getCollisionMap() {
	return m_collisionMap;
}

std::string Zone::getName() const {
	return m_file;
}

bool Zone::zoneDataLoaded() const {
	return m_mapLoaded;
}

int Zone::locateTile(int x, int y){
	
	for (unsigned int t = 0; t < m_tileMap.size(); t++) {
		if ((m_tileMap[t].x_pos + m_tileMap[t].tile.textureRect.width > x) && (m_tileMap[t].x_pos < x)) {
			if ((m_tileMap[t].y_pos + m_tileMap[t].tile.textureRect.height > y) && (m_tileMap[t].y_pos < y)) {
				return t; 	
			}
		}
	}
	return -1;
}

void Zone::replaceTile(int iId, Tile tile_){
	if (iId >= 0) {
		m_tileMap[iId].tile = tile_;
	}
}

void Zone::deleteTile(int iId) {
	if (iId >= 0) {
		m_tileMap[iId].tile = EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).getEmptyTile();
	}
}

void Zone::addEnvironment(int x_pos, int y_pos, Tile tile, bool centeredOnPos){
	int placePosX = x_pos;
	int placePosY = y_pos;
	if (centeredOnPos) {
		placePosX -= tile.textureRect.width / 2;
		placePosY -= tile.textureRect.height / 2;
	}

	m_environmentMap.push_back({ placePosX, placePosY, 0, tile, 1.0f, 1.0f, 1.0f, 1.0f, static_cast<float>(tile.textureRect.width), static_cast<float>(tile.textureRect.height)});

	if (tile.containsCollisionRect == true) {
		m_collisionMap.push_back({ placePosX + tile.collisionRect.x, placePosY + tile.collisionRect.y, tile.collisionRect.w, tile.collisionRect.h });
	}
}

void Zone::replaceEnvironment(int x_pos, int y_pos, Tile& tile, bool centeredOnPos, int replaceId) {
	int placePosX = x_pos;
	int placePosY = y_pos;
	if (centeredOnPos) {
		placePosX -= tile.textureRect.width / 2;
		placePosY -= tile.textureRect.height / 2;
	}

	m_environmentMap[replaceId] = { placePosX, placePosY, 0, tile, 1.0f, 1.0f, 1.0f, 1.0f, static_cast<float>(tile.textureRect.width), static_cast<float>(tile.textureRect.height) };
}

int Zone::deleteEnvironment(int x, int y) {

	for (unsigned int t = 0; t < m_environmentMap.size(); t++) {
		if ((m_environmentMap[t].x_pos + m_environmentMap[t].tile.textureRect.width > x) && (m_environmentMap[t].x_pos < x)) {
			if ((m_environmentMap[t].y_pos + m_environmentMap[t].tile.textureRect.height > y) &&
				(m_environmentMap[t].y_pos < y)) {
				m_environmentMap.erase(m_environmentMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

int Zone::locateEnvironment(int x, int y) {
	for (unsigned int t = 0; t < m_environmentMap.size(); t++) {
		if ((m_environmentMap[t].x_pos + m_environmentMap[t].tile.textureRect.width > x) &&
			(m_environmentMap[t].x_pos < x)) {
			if ((m_environmentMap[t].y_pos + m_environmentMap[t].tile.textureRect.height > y) &&
				(m_environmentMap[t].y_pos < y)) {
				return t;
			}
		}
	}
	return -1;
}

void Zone::addShadow(int x_pos, int y_pos, Tile tile) {
	m_shadowMap.push_back({ x_pos - (tile.textureRect.width / 2), y_pos - (tile.textureRect.height / 2), 0, tile, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f });
}

int Zone::deleteShadow(int x, int y){

	for (unsigned int t = 0; t < m_shadowMap.size(); t++) {
		if ((m_shadowMap[t].x_pos + m_shadowMap[t].tile.textureRect.width > x) && (m_shadowMap[t].x_pos < x)) {
			if ((m_shadowMap[t].y_pos + m_shadowMap[t].tile.textureRect.height > y) &&
				(m_shadowMap[t].y_pos < y)) {
				m_shadowMap.erase(m_shadowMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

int Zone::locateShadow(int x, int y) {
	for (unsigned int t = 0; t < m_shadowMap.size(); t++) {
		if ((m_shadowMap[t].x_pos + m_shadowMap[t].tile.textureRect.width > x) &&
			(m_shadowMap[t].x_pos < x)) {
			if ((m_shadowMap[t].y_pos + m_shadowMap[t].tile.textureRect.height > y) &&
				(m_shadowMap[t].y_pos < y)) {
				return t;
			}
		}
	}
	return -1;
}

void Zone::addNPC(Npc *npcToAdd) {
	m_npcs.push_back(npcToAdd);
}

int Zone::deleteNPC(int x, int y) {
	for (unsigned int t = 0; t < m_npcs.size(); t++) {
		if ((m_npcs[t]->getXPos() + m_npcs[t]->getCharacterType()->getTileSet(Enums::ActivityType::Walking, Enums::Direction::S).getAllTiles()[0].textureRect.width > x) &&
			(m_npcs[t]->getXPos() < x)) {
			if ((m_npcs[t]->getYPos() + m_npcs[t]->getCharacterType()->getTileSet(Enums::ActivityType::Walking, Enums::Direction::S).getAllTiles()[0].textureRect.height > y) &&
				(m_npcs[t]->getYPos() < y)) {
				removeNPC(m_npcs[t]);
				return 0;
			}
		}
	}

	return 1;
}

void Zone::removeNPC(Npc *npcToDelete) {
	for (size_t curNPCNr = 0; curNPCNr < m_npcs.size(); ++curNPCNr) {
		Npc *curNPC = m_npcs[curNPCNr];
		if (curNPC == npcToDelete) {
			curNPC->markAsDeleted();
			break;
		}
	}
}

void Zone::cleanupNPCList() {
	size_t curNPCNr = 0;
	while (curNPCNr < m_npcs.size()) {
		Npc* curNPC = m_npcs[curNPCNr];
		if (curNPC->isMarkedAsDeletable()) {
			m_npcs.erase(m_npcs.begin() + curNPCNr);
			// TODO: delete curNPC. There seem to be some problems at the moment.
			//delete curNPC;
		}else {
			++curNPCNr;
		}
	}
}

int Zone::locateNPC(int x, int y) {
	
	for (unsigned int t = 0; t < m_npcs.size(); t++) {
		if ((m_npcs[t]->getXPos() + m_npcs[t]->getCharacterType()->getTileSet(Enums::ActivityType::Walking, Enums::Direction::S).getAllTiles()[0].textureRect.width > x) &&
			(m_npcs[t]->getXPos() < x)) {
			if ((m_npcs[t]->getYPos() + m_npcs[t]->getCharacterType()->getTileSet(Enums::ActivityType::Walking, Enums::Direction::S).getAllTiles()[0].textureRect.height > y) &&
				(m_npcs[t]->getYPos() < y)) {
				return t;
			}
		}
	}
	return -1;
}

std::vector<Npc*>& Zone::getNPCs() {
	return m_npcs;
}

std::string Zone::getZoneName() const {
	return m_file;
}

void Zone::addCollisionbox(int x_pos, int y_pos) {
	m_collisionMap.push_back({ x_pos, y_pos, 100, 100 });
}

int Zone::deleteCollisionbox(int x, int y) {
	for (unsigned int t = 0; t < m_collisionMap.size(); t++) {
		if ((m_collisionMap[t].x + m_collisionMap[t].w > x) && (m_collisionMap[t].x < x)) {
			if ((m_collisionMap[t].y + m_collisionMap[t].h > y) && (m_collisionMap[t].y < y)) {
				m_collisionMap.erase(m_collisionMap.begin() + t);
				return 0;
			}
		}
	}
	return -1;
}


int Zone::locateCollisionbox(int x, int y) {
	for (unsigned int t = 0; t < m_collisionMap.size(); t++) {
		if ((m_collisionMap[t].x + m_collisionMap[t].w > x) && (m_collisionMap[t].x < x)) {
			if ((m_collisionMap[t].y + m_collisionMap[t].h > y) && (m_collisionMap[t].y < y)) {
				return t;
			}
		}
	}
	return -1;
}

bool Zone::findInteractionPointForCharacter(Character *character) const {
	for (size_t curIP = 0; curIP < m_interactionPoints.size(); curIP++) {
		const CharacterInteractionPoint* curCharacterIP = dynamic_cast<const CharacterInteractionPoint*>(m_interactionPoints[curIP]);
		if (&curCharacterIP != NULL) {
			if (character == curCharacterIP->getCharacter()) {
				return true;
			}
		}
	}
	return false;
}

void Zone::findCharacter(Character *character, bool &found, size_t &foundPos) const {
	for (size_t curNpcNr = 0; curNpcNr < m_npcs.size(); ++curNpcNr) {
		if (m_npcs[curNpcNr] == character) {
			found = true;
			foundPos = curNpcNr;
			return;
		}
	}
	found = false;
}

void Zone::findInteractionPoint(InteractionPoint *interactionPoint, bool &found, size_t &foundPos) const {
	for (size_t curInteractionNr = 0; curInteractionNr < m_interactionPoints.size(); ++curInteractionNr) {
		if (m_interactionPoints[curInteractionNr] == interactionPoint) {
			found = true;
			foundPos = curInteractionNr;
			return;
		}
	}
	found = false;
}

void Zone::findInteractionRegion(InteractionRegion *interactionRegion, bool &found, size_t &foundPos) const {
	for (size_t curInteractionNr = 0; curInteractionNr < m_interactionRegions.size(); ++curInteractionNr) {
		if (m_interactionRegions[curInteractionNr] == interactionRegion) {
			found = true;
			foundPos = curInteractionNr;
			return;
		}
	}
	found = false;
}

void Zone::findEventHandler(CallIndirection *eventHandler, bool &found, size_t &foundPos) const {
	for (size_t curEventHandlerNr = 0; curEventHandlerNr < eventHandlers.size(); ++curEventHandlerNr) {
		if (eventHandlers[curEventHandlerNr] == eventHandler) {
			found = true;
			foundPos = curEventHandlerNr;
			return;
		}
	}
	found = false;
}

void Zone::update(float deltaTime) {

 	for (unsigned int i = 0; i < MagicMap.size(); ++i) {
		MagicMap[i]->process();
		MagicMap[i]->getSpell()->inEffect(deltaTime);
		cleanupActiveAoESpells();

		if (MagicMap[i]->isDone()) {
			MagicMap.erase(ZoneManager::Get().getCurrentZone()->MagicMap.begin() + i);
		}
	}


	for (unsigned int x = 0; x < m_npcs.size(); x++) {
		m_npcs[x]->update(deltaTime);
	}

	updateInteractionRegion();
	
}

void Zone::drawZoneBatched() {
	drawTilesBatched();
	drawEnvironmentBatched();
	drawShadowsBatched();
}

void Zone::drawTilesBatched() {
	for (unsigned int x = 0; x < m_tileMap.size(); x++) {
		TextureManager::DrawTextureBatched(m_tileMap[x].tile.textureRect, m_tileMap[x].x_pos, m_tileMap[x].y_pos, true, true, 6u);
	}
}

void Zone::drawEnvironmentBatched() {
	for (unsigned int x = 0; x < m_environmentMap.size(); x++) {
		TextureManager::DrawTextureBatched(m_environmentMap[x].tile.textureRect, m_environmentMap[x].x_pos, m_environmentMap[x].y_pos, m_environmentMap[x].width, m_environmentMap[x].height, Vector4f(m_environmentMap[x].red, m_environmentMap[x].green, m_environmentMap[x].blue, m_environmentMap[x].transparency), true, true, 6u);
	}
}

void Zone::drawShadowsBatched(){
	for (unsigned int x = 0; x < m_shadowMap.size(); x++) {
		TextureManager::DrawTextureBatched(m_shadowMap[x].tile.textureRect, m_shadowMap[x].x_pos, m_shadowMap[x].y_pos, true, true, 6u);
	}
}

void Zone::drawNpcsBatched() {
	for (unsigned int x = 0; x < m_npcs.size(); x++) {
		m_npcs[x]->draw();
	}	
}

void Zone::drawZoneInstanced() {
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureAtlas);

	drawTilesInstanced();
	drawEnvironmentInstanced();

	Instancedrenderer::Get().drawBuffer();

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Zone::drawTilesInstanced() {
	for (unsigned int x = 0; x < m_tileMap.size(); x++) {
		TextureManager::DrawTextureInstanced(m_tileMap[x].tile.textureRect, m_tileMap[x].x_pos, m_tileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentInstanced() {
	for (unsigned int x = 0; x < m_environmentMap.size(); x++) {
		TextureManager::DrawTextureInstanced(m_environmentMap[x].tile.textureRect, m_environmentMap[x].x_pos, m_environmentMap[x].y_pos);
	}
}

void Zone::addActiveAoESpell(SpellActionBase *spell) {
	activeAoESpells.push_back(spell);
}

std::vector<SpellActionBase*>& Zone::getActiveAoESpells() {
	return activeAoESpells;
}

void Zone::cleanupActiveAoESpells() {
	size_t curSpell = 0;
	while (curSpell < activeAoESpells.size()) {
		if (activeAoESpells[curSpell]->isEffectComplete() == true) {
			delete activeAoESpells[curSpell];
			activeAoESpells.erase(activeAoESpells.begin() + curSpell);
		}else {
			curSpell++;
		}
	}
}

void Zone::clearActiveAoESpells() {
	activeAoESpells.clear();
}

void Zone::removeActiveAoESpell(SpellActionBase* activeSpell) {
	for (size_t curSpell = 0; curSpell < activeAoESpells.size(); curSpell++) {
		if (activeAoESpells[curSpell] == activeSpell) {
			activeAoESpells[curSpell]->markSpellActionAsFinished();
		}
	}
}

void Zone::updateInteractionRegion() {
	for (size_t curInteractionRegionNr = 0; curInteractionRegionNr < m_interactionRegions.size(); ++curInteractionRegionNr) {
		InteractionRegion* curInteractionRegion = m_interactionRegions[curInteractionRegionNr];
		curInteractionRegion->interactWithPlayer();
	}
}

void Zone::processInput(int mouseX, int mouseY, int characterXpos, int characterYpos) {
	Mouse &mouse = Mouse::instance();

	if (mouse.buttonPressed(Mouse::BUTTON_RIGHT)) {
		for (size_t curInteractionNr = 0; curInteractionNr< m_interactionPoints.size(); ++curInteractionNr) {
			InteractionPoint *curInteraction = m_interactionPoints[curInteractionNr];
			if (curInteraction->isMouseOver(mouseX, mouseY)) {
				curInteraction->startInteraction(characterXpos, characterYpos);
			}
		}
	}
}

void Zone::addInteractionRegion(InteractionRegion* interactionRegion) {
	m_interactionRegions.push_back(interactionRegion);
}

std::vector<InteractionRegion*>& Zone::getInteractionRegions() {
	return m_interactionRegions;
}

void Zone::addInteractionPoint(InteractionPoint* interactionPoint) {
	m_interactionPoints.push_back(interactionPoint);
}

std::vector<InteractionPoint*>& Zone::getInteractionPoints() {
	return m_interactionPoints;
}

std::vector<CallIndirection*>& Zone::getEventHandlers() {
	return eventHandlers;
}

void Zone::addCharacterInteractionPoint(CharacterInteractionPoint *characterInteractionPoint) {
	m_interactionPoints.push_back(characterInteractionPoint);
}

GroundLoot& Zone::getGroundLoot() {
	return groundLoot;
}

TileSet& Zone::getTileSet(Enums::TileClassificationType tileType) {
	return m_tileSetManger->getTileSet(tileType);
}

void Zone::addEventHandler(CallIndirection *newEventHandler) {
	eventHandlers.push_back(newEventHandler);
}

void Zone::purgeInteractionList() {
	m_interactionPoints.clear();
	m_interactionPoints.shrink_to_fit();
}

void Zone::purgeInteractionRegionList() {
	m_interactionRegions.clear();
	m_interactionRegions.shrink_to_fit();
}

void Zone::purgeNpcList() {
	m_npcs.clear();
	m_npcs.shrink_to_fit();
}

void Zone::purgeGroundloot() {
	groundLoot.getGroundItems().clear();
	groundLoot.getGroundItems().shrink_to_fit();
}

void Zone::Draw() {
	ZoneManager::Get().getCurrentZone()->drawZoneBatched();
}

void Zone::DrawActiveAoESpells() {
	std::vector<SpellActionBase*>& activeAoESpells = ZoneManager::Get().getCurrentZone()->getActiveAoESpells();
	for (size_t curActiveAoESpellNr = 0; curActiveAoESpellNr < activeAoESpells.size(); ++curActiveAoESpellNr) {
		if (!activeAoESpells[curActiveAoESpellNr]->isEffectComplete()) {
			activeAoESpells[curActiveAoESpellNr]->draw();
		}
	}
}

void Zone::DrawNpcs() {
	ZoneManager::Get().getCurrentZone()->drawNpcsBatched();
}

void Zone::setInit(bool flag) {
	m_init = flag;
}

std::string Zone::getLuaSaveText() const {

	std::string zoneNameNoPrefix = m_file;
	if (zoneNameNoPrefix.find_last_of('/') != std::string::npos) {
		zoneNameNoPrefix = zoneNameNoPrefix.substr(zoneNameNoPrefix.find_last_of('/') + 1);
	}

	std::ostringstream oss;
	oss << "DawnInterface.setCurrentZone( \"" << m_file << "\" );" << std::endl;
	
	oss << std::endl;

	// save ground loot
	oss << "-- " << zoneNameNoPrefix << " ground loot" << std::endl;
	for (size_t curGroundItemNr = 0; curGroundItemNr < groundLoot.groundItems.size(); ++curGroundItemNr) {
		sGroundItems curGroundItem = groundLoot.groundItems[curGroundItemNr];
		Item *item = curGroundItem.item;
		if (dynamic_cast<GoldHeap*>(item) != NULL) {
			GoldHeap *goldHeap = dynamic_cast<GoldHeap*>(item);
			oss << "DawnInterface.restoreGroundGold( "
				<< goldHeap->numCoins() << ", "
				<< curGroundItem.xpos << ", "
				<< curGroundItem.ypos << " );" << std::endl;
		}
		else {
			oss << "DawnInterface.restoreGroundLootItem( "
				<< "itemDatabase[ \"" << item->getID() << "\" ], "
				<< curGroundItem.xpos << ", "
				<< curGroundItem.ypos << " );" << std::endl << std::endl;
		}
	}
	return oss.str();
}

Character* Zone::getCharacterPointer(size_t posInArray) const {
	return m_npcs.at(posInArray);
}

InteractionPoint* Zone::getInteractionPointPointer(size_t posInArray) const {
	return m_interactionPoints.at(posInArray);
}

InteractionRegion* Zone::getInteractionRegionPointer(size_t posInArray) const {
	return m_interactionRegions.at(posInArray);
}

CallIndirection* Zone::getEventHandlerPointer(size_t posInArray) const {
	return eventHandlers.at(posInArray);
}

///////////////////////////////////////////////////////////////////
ZoneManager ZoneManager::s_instance;

ZoneManager& ZoneManager::Get() {
	return s_instance;
}

Zone& ZoneManager::getZone(std::string zoneName) {
	Zone& zone = m_zones[zoneName];
	zone.m_file = zoneName;
	return m_zones[zoneName];
}

void ZoneManager::setCurrentZone(Zone* zone) {
	m_currentZone = zone;
}

Zone* ZoneManager::getCurrentZone() {
	return m_currentZone;
}

std::unordered_map<std::string, Zone>& ZoneManager::getAllZones() {
	return m_zones;
}