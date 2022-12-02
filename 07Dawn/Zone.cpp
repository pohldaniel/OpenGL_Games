#include <iostream>
#include "Zone.h"
#include "Callindirection.h"

Zone::Zone() : groundLoot(&Player::Get()) { }

Zone::~Zone(){	 }

int tmp = 0;
void Zone::loadZone(){

	if (m_mapLoaded) {
		return;
	}
	EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).setOffset(EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR).getAllTiles().size());
	EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).setOffset(EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT).getAllTiles().size());
	EditorInterface::getTileSet(Enums::TileClassificationType::SHADOW).setOffset(EditorInterface::getTileSet(Enums::TileClassificationType::SHADOW).getAllTiles().size());

	LuaFunctions::executeLuaScript(std::string("if ( MapData == nil )\nthen\n    MapData={}\nend"));
	LuaFunctions::executeLuaScript(std::string("if ( MapData.DontSave == nil )\nthen\n    MapData.DontSave={}\nend"));

	TextureAtlasCreator::get().init(m_file);
	LuaFunctions::executeLuaFile(std::string(m_file).append(".tiles_ground.lua"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".tiles_environment.lua"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".tiles_shadow.lua"));

	TextureManager::SetTextureAtlas(TextureAtlasCreator::get().getName(), TextureAtlasCreator::get().getAtlas());
	m_textureAtlas = TextureManager::GetTextureAtlas(m_file);
	

	//ZoneManager::Get().setCurrentZone(this);
	LuaFunctions::executeLuaScript(std::string("DawnInterface.setCurrentZone( \"").append(m_file).append("\");"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".ground.lua"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".environment.lua"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".shadow.lua"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".collision.lua"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".spawnpoints.lua"));
	LuaFunctions::executeLuaFile(std::string(m_file).append(".init.lua"));
	m_mapLoaded = true;

	Spritesheet::Safe("tmp/" + std::to_string(tmp), m_textureAtlas);
	tmp++;
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

std::vector<Npc*> Zone::getNPCs() {
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

void Zone::addInteractionRegion(InteractionRegion* interactionRegionToAdd){
	m_interactionRegions.push_back(interactionRegionToAdd);
}

std::vector<InteractionRegion*>& Zone::getInteractionRegions() {
	return m_interactionRegions;
}

std::vector<InteractionPoint*>& Zone::getInteractionPoints() {
	return m_interactionPoints;
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

void Zone::addInteractionPoint(InteractionPoint* interactionPointToAdd) {
	m_interactionPoints.push_back(interactionPointToAdd);
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

void Zone::update(float deltaTime) {
	for (unsigned int x = 0; x < m_npcs.size(); x++) {
		
		
		m_npcs[x]->update(deltaTime);
	}
}

void Zone::drawZoneBatched() {

	TextureManager::BindTexture(m_textureAtlas, true);

	drawTilesBatched();
	drawEnvironmentBatched();
	drawShadowsBatched();
	TextureManager::DrawBuffer();

	
	//drawNpcsBatched();
	
}

void Zone::drawTilesBatched() {
	for (unsigned int x = 0; x < m_tileMap.size(); x++) {
		TextureManager::DrawTextureBatched(m_tileMap[x].tile.textureRect, m_tileMap[x].x_pos, m_tileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentBatched() {
	for (unsigned int x = 0; x < m_environmentMap.size(); x++) {
		TextureManager::DrawTextureBatched(m_environmentMap[x].tile.textureRect, m_environmentMap[x].x_pos, m_environmentMap[x].y_pos, m_environmentMap[x].width, m_environmentMap[x].height, Vector4f(m_environmentMap[x].red, m_environmentMap[x].green, m_environmentMap[x].blue, m_environmentMap[x].transparency));
	}
}

void Zone::drawShadowsBatched(){
	for (unsigned int x = 0; x < m_shadowMap.size(); x++) {
		TextureManager::DrawTextureBatched(m_shadowMap[x].tile.textureRect, m_shadowMap[x].x_pos, m_shadowMap[x].y_pos);
	}
}

void Zone::drawNpcsBatched() {
	//std::cout << " Size: " << m_npcs.size() << std::endl;
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("mobs"), true);
	for (unsigned int x = 0; x < m_npcs.size(); x++) {
		m_npcs[x]->draw();
	}	

	TextureManager::DrawBuffer();

	TextureManager::UnbindTexture(true);
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
	activeAoESpells.push_back(std::pair<SpellActionBase*, uint32_t>(spell, Globals::clock.getElapsedTimeMilli()));
}

std::vector<std::pair<SpellActionBase*, uint32_t> > Zone::getActiveAoESpells() {
	return activeAoESpells;
}

void Zone::cleanupActiveAoESpells() {
	size_t curSpell = 0;
	while (curSpell < activeAoESpells.size()) {
		if (activeAoESpells[curSpell].first->isEffectComplete() == true) {
			delete activeAoESpells[curSpell].first;
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
		if (activeAoESpells[curSpell].first == activeSpell) {
			activeAoESpells[curSpell].first->markSpellActionAsFinished();
		}
	}
}

GroundLoot* Zone::getGroundLoot() {
	return &groundLoot;
}

void Zone::addEventHandler(CallIndirection *newEventHandler) {
	eventHandlers.push_back(newEventHandler);
}

///////////////////////////////////////////////////////////////////
ZoneManager ZoneManager::s_instance;

ZoneManager& ZoneManager::Get() {
	return s_instance;
}

Zone& ZoneManager::getZone(std::string zoneName) {
	Zone& zone = m_zones[zoneName];
	zone.m_file = zoneName;
	return zone;
}

void ZoneManager::setCurrentZone(Zone* zone) {
	m_currentZone = zone;
}

Zone* ZoneManager::getCurrentZone() {
	return m_currentZone;
}

std::unordered_map<std::string, Zone> ZoneManager::getAllZones() {
	return m_zones;
}