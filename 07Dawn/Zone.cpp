
#include <iostream>
#include "Zone.h"

#include "Constants.h"


bool TileMap::operator<(const TileMap& tile1) const{ 
	return tile->tileID < tile1.tile->tileID;
};

Zone::Zone() {
}

Zone::~Zone(){	
}

void Zone::loadZone(std::string file){
	m_name = file;

	bool needOwnTextureFrame = !Globals::initPhase;
	if (needOwnTextureFrame) {		
		Globals::initPhase = true;
		TextureAtlasCreator::get().init();
	}
	
	Globals::allZones[m_name] = this;
	LuaFunctions::executeLuaScript(std::string("DawnInterface.setCurrentZone( \"").append(m_name).append("\");"));
	LuaFunctions::executeLuaScript(std::string("if ( MapData == nil )\nthen\n    MapData={}\nend"));
	LuaFunctions::executeLuaScript(std::string("if ( MapData.DontSave == nil )\nthen\n    MapData.DontSave={}\nend"));

	LuaFunctions::executeLuaFile(std::string(file).append(".tiles_ground.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".tiles_environment.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".tiles_shadow.lua"));

	LuaFunctions::executeLuaFile(std::string(file).append(".ground.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".environment.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".shadow.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".collision.lua"));

	//LuaFunctions::executeLuaFile(std::string(file).append(".init.lua"));

	m_mapLoaded = true;

	if (needOwnTextureFrame) {
		TextureManager::SetTextureAtlas(m_name, TextureAtlasCreator::get().getAtlas());
		Globals::initPhase = false;
	}

	m_textureAtlas = &TextureManager::GetTextureAtlas(m_name);

	LuaFunctions::executeLuaFile("res/_lua/mobdata.lua");
	LuaFunctions::executeLuaFile("res/_lua/zone1.spawnpoints.lua");
	LuaFunctions::executeLuaFile("res/_lua/gameinit.lua");
}

std::string Zone::getName() const {
	return m_name;
}

bool Zone::zoneDataLoaded() const {
	return m_mapLoaded;
}

int Zone::locateTile(int x, int y){
	
	for (unsigned int t = 0; t < tileMap.size(); t++) {
		if ((tileMap[t].x_pos + tileMap[t].tile->textureRect.width > x) && (tileMap[t].x_pos < x)) {
			if ((tileMap[t].y_pos + tileMap[t].tile->textureRect.height > y) && (tileMap[t].y_pos < y)) {
				return t; 	
			}
		}
	}
	return -1;
}

void Zone::changeTile(int iId, Tile *tile_){
	if (iId >= 0) {
		tileMap[iId].tile = tile_;
	}
}

void Zone::deleteTile(int iId) {
	if (iId >= 0) {
		tileMap[iId].tile = EditorInterface::getTileSet(TileClassificationType::FLOOR)->getEmptyTile();
	}
}
void Zone::addEnvironment(int x_pos, int y_pos, Tile *tile, bool centeredOnPos){
	int placePosX = x_pos;
	int placePosY = y_pos;
	if (centeredOnPos) {
		placePosX -= tile->textureRect.width / 2;
		placePosY -= tile->textureRect.height / 2;
	}

	environmentMap.push_back(EnvironmentMap(placePosX, placePosY, tile, 1.0f, 1.0f, 1.0f, 1.0f, static_cast<float>(tile->textureRect.width), static_cast<float>(tile->textureRect.height), 1.0f));

	if (tile->containsCollisionRect == true) {
		collisionMap.push_back({ placePosX + tile->collisionRect.x, placePosY + tile->collisionRect.y, tile->collisionRect.w, tile->collisionRect.h });
	}
}

int Zone::deleteEnvironment(int x, int y) {

	for (unsigned int t = 0; t < environmentMap.size(); t++) {
		if ((environmentMap[t].x_pos + environmentMap[t].tile->textureRect.width > x) && (environmentMap[t].x_pos < x)) {
			if ((environmentMap[t].y_pos + environmentMap[t].tile->textureRect.height > y) &&
				(environmentMap[t].y_pos < y)) {
				environmentMap.erase(environmentMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

int Zone::locateEnvironment(int x, int y) {
	for (unsigned int t = 0; t < environmentMap.size(); t++) {
		if ((environmentMap[t].x_pos + environmentMap[t].tile->textureRect.width > x) &&
			(environmentMap[t].x_pos < x)) {
			if ((environmentMap[t].y_pos + environmentMap[t].tile->textureRect.height > y) &&
				(environmentMap[t].y_pos < y)) {
				return t;
			}
		}
	}
	return -1;
}

void Zone::addShadow(int x_pos, int y_pos, Tile *tile) {
	shadowMap.push_back(EnvironmentMap(x_pos - (tile->textureRect.width / 2), y_pos - (tile->textureRect.height / 2), tile, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0));
}

int Zone::deleteShadow(int x, int y){

	for (unsigned int t = 0; t < shadowMap.size(); t++) {
		if ((shadowMap[t].x_pos + shadowMap[t].tile->textureRect.width > x) && (shadowMap[t].x_pos < x)) {
			if ((shadowMap[t].y_pos + shadowMap[t].tile->textureRect.height > y) &&
				(shadowMap[t].y_pos < y)) {
				shadowMap.erase(shadowMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

int Zone::locateShadow(int x, int y) {
	for (unsigned int t = 0; t < shadowMap.size(); t++) {
		if ((shadowMap[t].x_pos + shadowMap[t].tile->textureRect.width > x) &&
			(shadowMap[t].x_pos < x)) {
			if ((shadowMap[t].y_pos + shadowMap[t].tile->textureRect.height > y) &&
				(shadowMap[t].y_pos < y)) {
				return t;
			}
		}
	}
	return -1;
}

void Zone::addNPC(Npc *npcToAdd) {
	npcs.push_back(npcToAdd);
}

int Zone::deleteNPC(int x, int y) {
	for (unsigned int t = 0; t< npcs.size(); t++) {
		if ((npcs[t]->getXPos() + npcs[t]->m_characterType.getTileSet(ActivityType::ActivityType::Walking, Direction::S).getAllTiles()[0]->textureRect.width > x) &&
			(npcs[t]->getXPos() < x)) {
			if ((npcs[t]->getYPos() + npcs[t]->m_characterType.getTileSet(ActivityType::ActivityType::Walking, Direction::S).getAllTiles()[0]->textureRect.height > y) &&
				(npcs[t]->getYPos() < y)) {
				removeNPC(npcs[t]);
				return 0;
			}
		}
	}

	return 1;
}

void Zone::removeNPC(Npc *npcToDelete) {
	for (size_t curNPCNr = 0; curNPCNr<npcs.size(); ++curNPCNr) {
		Npc *curNPC = npcs[curNPCNr];
		if (curNPC == npcToDelete) {
			curNPC->markAsDeleted();
			break;
		}
	}
}

void Zone::cleanupNPCList() {
	size_t curNPCNr = 0;
	while (curNPCNr < npcs.size()) {
		Npc* curNPC = npcs[curNPCNr];
		if (curNPC->isMarkedAsDeletable()) {
			npcs.erase(npcs.begin() + curNPCNr);
			// TODO: delete curNPC. There seem to be some problems at the moment.
			//delete curNPC;
		}else {
			++curNPCNr;
		}
	}
}

int Zone::locateNPC(int x, int y) {
	
	for (unsigned int t = 0; t < npcs.size(); t++) {
		if ((npcs[t]->getXPos() + npcs[t]->m_characterType.getTileSet(ActivityType::ActivityType::Walking, Direction::S).getAllTiles()[0]->textureRect.width > x) &&
			(npcs[t]->getXPos() < x)) {
			if ((npcs[t]->getYPos() + npcs[t]->m_characterType.getTileSet(ActivityType::ActivityType::Walking, Direction::S).getAllTiles()[0]->textureRect.height > y) &&
				(npcs[t]->getYPos() < y)) {
				return t;
			}
		}
	}
	return -1;
}

std::vector<Npc*> Zone::getNPCs() {
	return npcs;
}

void Zone::addCollisionbox(int x_pos, int y_pos) {
	collisionMap.push_back({ x_pos, y_pos, 100, 100 });
}

int Zone::deleteCollisionbox(int x, int y) {
	for (unsigned int t = 0; t < collisionMap.size(); t++) {
		if ((collisionMap[t].x + collisionMap[t].w > x) && (collisionMap[t].x < x)) {
			if ((collisionMap[t].y + collisionMap[t].h > y) && (collisionMap[t].y < y)) {
				collisionMap.erase(collisionMap.begin() + t);
				return 0;
			}
		}
	}
	return -1;
}


int Zone::locateCollisionbox(int x, int y) {
	for (unsigned int t = 0; t < collisionMap.size(); t++) {
		if ((collisionMap[t].x + collisionMap[t].w > x) && (collisionMap[t].x < x)) {
			if ((collisionMap[t].y + collisionMap[t].h > y) && (collisionMap[t].y < y)) {
				return t;
			}
		}
	}
	return -1;
}

void Zone::addInteractionRegion(InteractionRegion *interactionRegionToAdd){
	interactionRegions.push_back(interactionRegionToAdd);
}

std::vector<InteractionRegion*> Zone::getInteractionRegions() {
	return interactionRegions;
}

void Zone::update(float deltaTime) {
	for (unsigned int x = 0; x < npcs.size(); x++) {
		npcs[x]->Wander();
		npcs[x]->Move();

		npcs[x]->update(deltaTime);
	}
}

void Zone::drawZoneBatched() {
	
	Batchrenderer::get().bindTexture(*m_textureAtlas, true);

	drawTilesBatched();
	drawEnvironmentBatched();
	drawShadowsBatched();
	Batchrenderer::get().drawBuffer();

	Batchrenderer::get().bindTexture(TextureManager::GetTextureAtlas("Wolf"), true);
	drawNpcsBatched();
	Batchrenderer::get().drawBuffer();

	Batchrenderer::get().unbindTexture(true);
}

void Zone::drawTilesBatched() {
	for (unsigned int x = 0; x < tileMap.size(); x++) {
		TextureManager::DrawTextureBatched(tileMap[x].tile->textureRect, tileMap[x].x_pos, tileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentBatched() {
	for (unsigned int x = 0; x < environmentMap.size(); x++) {
		TextureManager::DrawTextureBatched(environmentMap[x].tile->textureRect, environmentMap[x].x_pos, environmentMap[x].y_pos, environmentMap[x].width, environmentMap[x].height, Vector4f(environmentMap[x].red, environmentMap[x].green, environmentMap[x].blue, environmentMap[x].transparency));
	}
}

void Zone::drawShadowsBatched(){
	for (unsigned int x = 0; x < shadowMap.size(); x++) {
		TextureManager::DrawTextureBatched(shadowMap[x].tile->textureRect, shadowMap[x].x_pos, shadowMap[x].y_pos);
	}
}

void Zone::drawNpcsBatched() {
	for (unsigned int x = 0; x < npcs.size(); x++) {
		npcs[x]->draw();
	}
	
}

void Zone::drawZoneInstanced() {
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, *m_textureAtlas);

	drawTilesInstanced();
	drawEnvironmentInstanced();

	Instancedrenderer::get().drawBuffer();

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Zone::drawTilesInstanced() {
	for (unsigned int x = 0; x < tileMap.size(); x++) {
		TextureManager::DrawTextureInstanced(tileMap[x].tile->textureRect, tileMap[x].x_pos, tileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentInstanced() {
	for (unsigned int x = 0; x < environmentMap.size(); x++) {
		TextureManager::DrawTextureInstanced(environmentMap[x].tile->textureRect, environmentMap[x].x_pos, environmentMap[x].y_pos);
	}
}